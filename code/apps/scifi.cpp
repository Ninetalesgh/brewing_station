#include "scifi.h"

#include "file.h"

#include <ui/debugui.h>

#include <common/string.h>
#include <common/profile.h>
#include <common/basic_rasterizer.h>
#include <common/font.h>
#include <common/memory.h>

#include <platform.h>

struct Animation
{
  u64 debug_StartFrame;
  u64 debug_EndFrame;
  float step;
  float t;
};

u32 CheckMouseCollision( float2 mouse, Rect rect )
{
  bool2 smaller = mouse < rect.max;
  bool2 bigger = mouse > rect.min;
  return smaller.both() && bigger.both();
}

//TODO remove math :(
#include <math.h>
namespace debug
{
  void bitmap_draw_text( Bitmap* target, font::GlyphTable* glyphTable, float2 pos, u32 color, char const* text )
  {
    if ( glyphTable )
    {
      float currentX = pos.x;
      float currentY = pos.y;

      font::ParserUTF8 parser( text );

      s32 codepoint = parser.get_next_codepoint();
      while ( codepoint )
      {
        if ( codepoint == '\n' )
        {
          currentX = pos.x;
          currentY += glyphTable->scale;
        }
        else
        {
          font::Glyph const* glyph = glyphTable->get_glyph( codepoint );

          if ( glyph )
          {
            float2 glyphPos = pos + float2( currentX + (float) glyph->offsetX, currentY + (float) glyph->offsetY );
            currentX += glyph->advance * glyphTable->scaleForPixelHeight;

            if ( glyph->data )
            {
              glyph_draw( target, glyph->data, glyph->width, glyph->height, glyphPos, color );
            }
          }
        }
        codepoint = parser.get_next_codepoint();
      }
    }
  }

  void RenderUI( Bitmap* target, u64 currentFrame, UIState& state, float2 mouse )
  {
    memory::debug::ArenaDebugData arenaDebugData;
    state.debugDisplay->arenaObserver.fetch_debug_data( &arenaDebugData );
    char uiText[2048];
    //
    string_format( uiText,
    "Arena Memory\n-Size:   ", arenaDebugData.size,
    "\n-Unused: ", arenaDebugData.capacity - arenaDebugData.size,
    "\nArena Entries\n-Active:   ", arenaDebugData.totalEntries,
    "\n-Inactive: ", arenaDebugData.inactiveEntries );

    float2 baseline = { 0.0f, state.glyphTable->scale * 0.5f };
    bitmap_draw_text( target, state.glyphTable, baseline, color::LIGHT_GRAY, uiText );
    //bitmap_draw_text( target, state.glyphTable, { 0.0f ,height * 2 }, color::WHITE, "How are you? :)" );
  }

  void RenderTestScreen( Bitmap* target, u64 currentFrame, UIState& state, float2 mouse )
  {
    s32 height = target->height;
    s32 width = target->width;

    Rect button0;
    Rect button1;

    local_persist u64 startFrame;
    local_persist u64 endFrame;
    local_persist u32 inAnimation = false;
    constexpr u64 animationDuration = 15;
    constexpr u32 startColor = color::LIGHT_GRAY;
    constexpr u32 endColor = color::RED;
    local_persist float t;
    local_persist float step = 1.f / float( animationDuration );

    button0.min = { width - 300, height / 2 };
    button0.max = { width,       height / 2 + 100 };

    button1.min = { width - 300, height / 2 - 110 };
    button1.max = { width,       height / 2 - 10 };

    u32 button0collide = CheckMouseCollision( mouse, button0 );
    u32 colorButton0 = color::LIGHT_GRAY;

    if ( button0collide )
    {
      if ( inAnimation )
      {
        t += step;
        colorButton0 = color::interpolate( t, startColor, endColor );
      }
      else
      {
        t = 0.f;
        inAnimation = true;
        startFrame = currentFrame;
        endFrame = currentFrame + animationDuration;
      }
    }
    else
    {
      inAnimation = false;
    }

    u32 button1collide = CheckMouseCollision( mouse, button1 );

    u32 colorButton1 = button1collide ? color::RED : color::LIGHT_GRAY;

    draw_rect( target, { 0, 0 }, { target->width , target->height }, color::DARK_GRAY );

    if ( state.image.pixel )
    {
      //   bitmap_draw_no_alpha( target, state.image, { 0, 0 } );
    }

    // if ( state.fontImage.pixel )
    // {
    //   bitmap_draw( target, state.fontImage, { 0,0 } );
    // }

    draw_rect( target, button0, colorButton0 );
    draw_rect( target, button1, colorButton1 );

  }

  void RenderGradient( BackBuffer& buffer, s32 xOffset, s32 yOffset )
  {
    u8* row = (u8*) buffer.data;
    for ( s32 y = 0; y < buffer.height; ++y )
    {
      u32* pixel = (u32*) row;
      for ( s32 x = 0; x < buffer.width; ++x )
      {
        // 0x BB GG RR xx
        u8 R = u8( x + yOffset );
        u8 G = u8( x / 2 + xOffset );
        u8 B = u8( y + yOffset );

        *pixel++ = color::RGBA( R, G, B, 0xff );
      }

      row += buffer.pitch;
    }
  }

  void OutputSound( SoundBuffer& soundBuffer, float hz )
  {
    local_persist float tSin = 0;
    s16 toneVolume = 0;
    float toneHz = hz;
    float wavePeriod = float( soundBuffer.samplesPerSecond ) / toneHz;

    s16* sampleOut = soundBuffer.samples;
    for ( s32 iSample = 0; iSample < soundBuffer.sampleCount; ++iSample )
    {
      float sinValue = sinf( tSin );
      s16 sampleValue = s16( sinValue * float( toneVolume ) );
      *sampleOut++ = sampleValue;
      *sampleOut++ = sampleValue;
      tSin += PIx2 / wavePeriod;
      tSin -= (tSin > PIx2) * PIx2;
    }
  }
};

void app_on_load( AppOnLoadParameter& parameter )
{
  PlatformData const& platformData = *parameter.platformData;
  AppData& appData = *parameter.appData;

  assert( sizeof( GameState ) <= appData.staticBufferSize );
  GameState& state = *(GameState*) (appData.staticBuffer);


  #if !BS_BUILD_RELEASE
  {
    font::DEBUG_unicode_codepoints();

    if ( !state.uiState.debugDisplay )
    {
      state.uiState.debugDisplay = (ui::debug::DebugDisplay*) appData.debug.arena.alloc_set_zero( sizeof( ui::debug::DebugDisplay ) );
      state.uiState.debugDisplay->arenaObserver.arena = &appData.generalPurposeArena;
    }
  }
  #endif

  {
    if ( state.uiState.glyphTable )
    {
      font::delete_glyph_table( state.uiState.glyphTable );
    }

    //char filename[] = "w:/data/Inconsolata-Regular.ttf";
    char filename[] = "w:/data/SourceCodePro/SourceCodePro-Light.ttf";
    //char filename[] = "w:/data/SourceCodePro/SourceCodePro-LightItalic.ttf";
    //char filename[] = "w:/data/SourceCodePro/SourceCodePro-SemiBold.ttf";
    platform::ReadFileResult file = platformData.read_file( filename, 0, 0 );
    state.uiState.glyphTable = font::init_glyph_table_from_ttf( &appData.generalPurposeArena, (u8 const*) file.data );
    state.uiState.glyphTable->set_scale( 40.0f );
  }
}

void app_tick( AppTickParameter& parameter )
{
  PlatformData const& platformData = *parameter.platformData;
  AppData& appData = *parameter.appData;
  BackBuffer& backBuffer = *parameter.backBuffer;

  assert( sizeof( GameState ) <= appData.staticBufferSize );
  Input const& input = platformData.input;

  GameState& state = *(GameState*) (appData.staticBuffer);

  {
    ControllerInput const& input0 = input.controller[0];
    state.xOffset += s32( -input0.end.x * 4.f );
    state.yOffset += s32( input0.end.y * 4.f );
    state.toneHz = 512.f + (256.f * input0.end.y);

    s32 speed = 4;
    state.xOffset += (input.held[Input::KEY_LEFT] - input.held[Input::KEY_RIGHT]) * speed;
    state.yOffset += (input.held[Input::KEY_DOWN] - input.held[Input::KEY_UP]) * speed;

    state.toneHz += 256.f * (input.held[Input::KEY_DOWN] - input.held[Input::KEY_UP]);
  }

  #if !BS_BUILD_RELEASE

  Bitmap bmp = {};
  bmp.pixel = (u32*) backBuffer.data;
  bmp.width = backBuffer.width;
  bmp.height = backBuffer.height;

  UIState& uiState = state.uiState;
  appData.debug.trigger[0] = input.down[Input::KEY_0] ? !appData.debug.trigger[0] : appData.debug.trigger[0];
  appData.debug.trigger[1] = input.down[Input::KEY_1] ? !appData.debug.trigger[1] : appData.debug.trigger[1];
  appData.debug.trigger[2] = input.down[Input::KEY_2] ? !appData.debug.trigger[2] : appData.debug.trigger[2];
  appData.debug.trigger[3] = input.down[Input::KEY_3] ? !appData.debug.trigger[3] : appData.debug.trigger[3];
  appData.debug.trigger[4] = input.down[Input::KEY_4] ? !appData.debug.trigger[4] : appData.debug.trigger[4];
  appData.debug.trigger[5] = input.down[Input::KEY_5] ? !appData.debug.trigger[5] : appData.debug.trigger[5];
  appData.debug.trigger[6] = input.down[Input::KEY_6] ? !appData.debug.trigger[6] : appData.debug.trigger[6];
  appData.debug.trigger[7] = input.down[Input::KEY_7] ? !appData.debug.trigger[7] : appData.debug.trigger[7];
  appData.debug.trigger[8] = input.down[Input::KEY_8] ? !appData.debug.trigger[8] : appData.debug.trigger[8];
  appData.debug.trigger[9] = input.down[Input::KEY_9] ? !appData.debug.trigger[9] : appData.debug.trigger[9];

  if ( input.down[Input::KEY_2] )
  {
    //char filename[] = "w:/data/Inconsolata-Regular.ttf";
   // platform::ReadFileResult file = platformData.read_file( filename, 0, 0 );
   // state.uiState.glyphTable = font::load_glyph_table_from_ttf( &appData.generalPurposeArena, (u8 const*) file.data );

    //state.uiState.fontImage = load_font( (u8 const*) file.data, file.size );
  }

  if ( (platformData.currentFrameIndex % 50) == 0 )
  {
    char hello[200] = {};

    string_format( hello + sizeof( net::PacketHeader ), "hello world!" );

    net::UDPSendParameter udpSendParameter = {};
    udpSendParameter.packet = hello;
    udpSendParameter.packetSize =  sizeof( net::PacketHeader ) + string_format( hello + sizeof( net::PacketHeader ), "hello world!" );

    platformData.send_udp( udpSendParameter );
  }

  if ( input.down[Input::KEY_1] )
  {
    char filename[] = "scifi.dll";
    platform::ReadFileResult file = platformData.read_file( filename, 0, 0 );

    net::TCPSendParameter tcpSendParameter = {};
    tcpSendParameter.filename = filename;
    tcpSendParameter.fileSize = file.size;
    tcpSendParameter.fileData = (char const*) file.data;
    tcpSendParameter.to = {};
    platformData.send_tcp( tcpSendParameter );
  }

  if ( appData.debug.trigger[2] )
  {
    debug::RenderGradient( backBuffer, state.xOffset, state.yOffset );
  }
  else
  {
    // local_persist int fileIndex = 0;
    // local_persist int lastFileIndex = 1;
    // char const* filename[] =
    // {
    //   "w:/data/test.bmp",
    //    "w:/data/pic.jpg",
    //    "w:/data/bread.jpg",
    // };

    // if ( input.down[Input::KEY_UP] )
    // {
    //   fileIndex = (fileIndex + 1) % array_size( filename );
    // }
    // else if ( input.down[Input::KEY_DOWN] )
    // {
    //   fileIndex = (fileIndex + array_size( filename ) - 1) % array_size( filename );
    // }

    // if ( fileIndex != lastFileIndex )
    // {
    //   lastFileIndex = fileIndex;

    //   FileInfo fileInfo;
    //   platformData.get_file_info( filename[fileIndex], &fileInfo );

    //   platform::ReadFileResult readResult = platformData.read_file( filename[fileIndex], 0, 0 );
    //   if ( readResult.data )
    //   {
    //     state.uiState.image = load_image( (u8 const*) readResult.data, readResult.size );
    //   }
    // }

    debug::RenderTestScreen( &bmp, platformData.currentFrameIndex, uiState, float2( input.mousePos[0].end ) );
  }

  if ( !appData.debug.trigger[0] )
  {
    debug::RenderUI( &bmp, platformData.currentFrameIndex, uiState, float2( input.mousePos[0].end ) );
  }

  if ( input.down[Input::KEY_F1] )
  {
    char screenshotfilename[] = "w:/data/test2.bmp";
    file_save_bmp( platformData.write_file, screenshotfilename, bmp );
  }

  if ( input.down[Input::KEY_3] )
  {
    char filename[] = "w:/data/test.bmp";
    char filename2[] = "w:/data/test2.bmp";
    Bitmap bitmp = file_load_bmp( platformData.read_file, filename );

    Bitmap bmpsave = {};
    bmpsave.pixel = (u32*) backBuffer.data;
    bmpsave.width = 1;
    bmpsave.height = 1;
    file_save_bmp( platformData.write_file, filename2, bmp );
  }
  #endif

}

void app_render( AppRenderParameter& parameter )
{

}

void app_sample_sound( AppSampleSoundParameter& parameter )
{
  AppData& appData = *parameter.appData;
  SoundBuffer& soundBuffer = *parameter.soundBuffer;

  GameState& state = *(GameState*) (appData.staticBuffer);
  debug::OutputSound( soundBuffer, state.toneHz );
}

void app_receive_udp_packet( AppReceiveUDPPacketParameter& parameter )
{
  //PlatformData const& platformData = *parameter.platformData;
  // platformData.write_file( "test.txt", state.receiverBuffer, size - 1 );
}


#include "vfs.cpp"
#include "file.cpp"
#include <common/font.cpp>