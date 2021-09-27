#include "scifi.h"

#include "file.h"

#include <platform.h>
#include <common/string.h>
#include <common/profile.h>
#include <common/basic_rasterizer.h>
#include <common/font.h>

#include <common/memory.h>

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
      //char const* reader = text;
      float currentX = pos.x;
      font::UnicodeParser parser( text );
      s32 codepoint = parser.get_next_codepoint();
      while ( codepoint )
      {
        font::Glyph const& glyph = *(glyphTable->get_glyph( codepoint ));

        float2 currentPos = pos + float2( currentX + glyph.offsetX, (float) glyph.offsetY );
        currentX += glyph.advance * glyphTable->scale;

        if ( glyph.data )
        {
          glyph_draw( target, glyph.data, glyph.width, glyph.height, currentPos, color );
        }

        codepoint = parser.get_next_codepoint();
      }

      // while ( *reader )
      // {
      //   font::Glyph const& glyph = *(glyphTable->get_glyph( *reader++ ));

      //   float2 currentPos = pos + float2( currentX + glyph.offsetX, (float) glyph.offsetY );
      //   currentX += glyph.advance * glyphTable->scale;

      //   if ( glyph.data )
      //   {
      //     glyph_draw( target, glyph.data, glyph.width, glyph.height, currentPos, RED );
      //   }
      // }
    }
  }

  void RenderUI( Bitmap* target, u64 currentFrame, UIState& state, float2 mouse )
  {
    s32 height = target->height;
    s32 width = target->width;

    Rect button0;
    Rect button1;

    local_persist u64 startFrame;
    local_persist u64 endFrame;
    local_persist u32 inAnimation = false;
    constexpr u64 animationDuration = 15;
    constexpr u32 startColor = LIGHT_GRAY;
    constexpr u32 endColor = RED;
    local_persist float t;
    local_persist float step = 1.f / float( animationDuration );

    button0.min = { width - 300, height / 2 };
    button0.max = { width,       height / 2 + 100 };

    button1.min = { width - 300, height / 2 - 110 };
    button1.max = { width,       height / 2 - 10 };

    u32 button0collide = CheckMouseCollision( mouse, button0 );
    u32 colorButton0 = LIGHT_GRAY;

    if ( button0collide )
    {
      if ( inAnimation )
      {
        t += step;
        colorButton0 = color_interpolate( t, startColor, endColor );
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

    u32 colorButton1 = button1collide ? RED : LIGHT_GRAY;

    draw_rect( target, { 0, 0 }, { target->width , target->height }, DARK_GRAY );

    if ( state.image.pixel )
    {
      //   bitmap_draw_no_alpha( target, state.image, { 0, 0 } );
    }

    // if ( state.fontImage.pixel )
    // {
    //   bitmap_draw( target, state.fontImage, { 0,0 } );
    // }

    float2 baseline = { 50, 200 };
    bitmap_draw_text( target, state.glyphTable, baseline, RED, "Hello Klara" );
    bitmap_draw_text( target, state.glyphTable, { 50,300 }, BLUE, "How are you? :)" );

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

        *pixel++ = color_RGBA( R, G, B, 0xff );
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

void app_on_load( PlatformData const& platformData, AppData& appData )
{
  assert( sizeof( GameState ) <= appData.staticBufferSize );
  GameState& state = *(GameState*) (appData.staticBuffer);

  font::DEBUG_unicode_codepoints();

  if ( state.uiState.glyphTable )
  {
    font::unload_glyph_table( state.uiState.glyphTable );
  }

  char filename[] = "w:/data/Inconsolata-Regular.ttf";
  platform::ReadFileResult file = platformData.read_file( filename, 0, 0 );
  state.uiState.glyphTable = font::load_glyph_table_from_ttf( &appData.generalPurposeArena, (u8 const*) file.data );
}

void app_tick( PlatformData const& platformData, AppData& appData, BackBuffer& backBuffer )
{
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
  appData.debug_trigger[0] = input.down[Input::KEY_0] ? !appData.debug_trigger[0] : appData.debug_trigger[0];
  appData.debug_trigger[1] = input.down[Input::KEY_1] ? !appData.debug_trigger[1] : appData.debug_trigger[1];
  appData.debug_trigger[2] = input.down[Input::KEY_2] ? !appData.debug_trigger[2] : appData.debug_trigger[2];
  appData.debug_trigger[3] = input.down[Input::KEY_3] ? !appData.debug_trigger[3] : appData.debug_trigger[3];
  appData.debug_trigger[4] = input.down[Input::KEY_4] ? !appData.debug_trigger[4] : appData.debug_trigger[4];
  appData.debug_trigger[5] = input.down[Input::KEY_5] ? !appData.debug_trigger[5] : appData.debug_trigger[5];
  appData.debug_trigger[6] = input.down[Input::KEY_6] ? !appData.debug_trigger[6] : appData.debug_trigger[6];
  appData.debug_trigger[7] = input.down[Input::KEY_7] ? !appData.debug_trigger[7] : appData.debug_trigger[7];
  appData.debug_trigger[8] = input.down[Input::KEY_8] ? !appData.debug_trigger[8] : appData.debug_trigger[8];
  appData.debug_trigger[9] = input.down[Input::KEY_9] ? !appData.debug_trigger[9] : appData.debug_trigger[9];

  if ( input.down[Input::KEY_2] )
  {
    //char filename[] = "w:/data/Inconsolata-Regular.ttf";
   // platform::ReadFileResult file = platformData.read_file( filename, 0, 0 );
   // state.uiState.glyphTable = font::load_glyph_table_from_ttf( &appData.generalPurposeArena, (u8 const*) file.data );

    //state.uiState.fontImage = load_font( (u8 const*) file.data, file.size );
  }

  if ( (appData.currentFrameIndex % 50) == 0 )
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

  if ( appData.debug_trigger[0] )
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

    debug::RenderUI( &bmp, appData.currentFrameIndex, uiState, float2( input.mousePos[0].end ) );
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

void app_sample_sound( AppData& appData, SoundBuffer& soundBuffer )
{
  GameState& state = *(GameState*) (appData.staticBuffer);
  debug::OutputSound( soundBuffer, state.toneHz );
}

void app_receive_udp_packet( PlatformData const& platformData, AppData& appData, net::UDPReceiveParameter const& parameter )
{

  // platformData.write_file( "test.txt", state.receiverBuffer, size - 1 );
}


#include "vfs.cpp"
#include "file.cpp"
#include <common/font.cpp>