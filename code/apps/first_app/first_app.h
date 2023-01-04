#pragma once

DEPRECATED

#include <core/bsdebuglog.h>
#include <common/bscolor.h>
#include <core/bsfont.h>
#include <platform/platform.h>

struct FirstAppData
{
  bs::graphics::Bitmap bmp;
  bs::font::GlyphTable* glyphTable;
  bs::font::GlyphSheet* glyphSheet;
};

FirstAppData* app;

void update();
void start();
void print_text( char const* text );



static bs::input::State* inputPtr;
const int windowWidth = DEFAULT_WINDOW_SIZE.x;
const int windowHeight = DEFAULT_WINDOW_SIZE.y - 40;



INLINE u32 is_key_held( u32 key )
{
  return bs::input::STATE_COUNT && inputPtr->held[key];
}

INLINE void clear( u32 clearColor )
{
  s32 end = app->bmp.width * app->bmp.height;
  for ( s32 i = 0; i < end; ++i )
  {
    app->bmp.pixel[i] = clearColor;
  }
}

INLINE void plot( int2 pos, u32 color )
{
  if ( pos.x >= 0.0f && pos.x < app->bmp.width &&
       pos.y >= 0.0f && pos.y < app->bmp.height )
  {
    app->bmp.pixel[pos.x + pos.y * app->bmp.width] = color;
  }
}

INLINE void draw_rect( int2 begin, int2 end, u32 color )
{
  begin = max( begin, int2 { 0, 0 } );
  end = min( end, DEFAULT_WINDOW_SIZE );

  for ( s32 y = begin.y; y < end.y; ++y )
    for ( s32 x = begin.x; x < end.x; ++x )
    {
      int2 pos = { x, y };
      plot( pos, color );
    }
}

INLINE void draw_circle( int2 pos, float radius, u32 color )
{
  s32 begin = -(s32) radius;
  s32 end = (s32) radius;

  for ( s32 y = begin; y < end; ++y )
    for ( s32 x = begin; x < end; ++x )
    {
      if ( x * x + y * y < radius * radius )
      {
        int2 tmpPos = pos + int2 { x, y };
        plot( tmpPos, color );
      }
    }
}

void draw_line( int2 begin, int2 end, u32 color )
{
  //TODO

}


namespace bs
{

  void app_sample_sound( PrmAppSampleSound prm )
  {
    BREAK;
  }

  void app_on_load( PrmAppOnLoad prm )
  {
    if ( prm.appData->userData == nullptr )
    {
      prm.appData->userData = memory::allocate_to_zero( sizeof( FirstAppData ) );
      app = (FirstAppData*) prm.appData->userData;
      app->bmp.pixel = (u32*) memory::allocate_to_zero( sizeof( u32 ) * windowWidth * windowHeight );
      app->bmp.height = windowHeight;
      app->bmp.width = windowWidth;

      bs::file::Data ttf;
      platform::load_file_into_memory( "w:/data/bs.ttf", &ttf );

      char const chars[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
      app->glyphTable = bs::font::create_glyph_table_from_ttf( ttf.data );
      bs::font::set_scale_for_glyph_creation( app->glyphTable, 64.0f );
      app->glyphSheet = bs::font::create_glyph_sheet( app->glyphTable, chars );
    }

    app = (FirstAppData*) prm.appData->userData;
    inputPtr = &prm.appData->input;

    start();
  }

  void app_tick( PrmAppTick prm )
  {
    update();
    graphics::RenderGroup rg = graphics::render_group_from_custom_bitmap( &app->bmp );
    platform::render( nullptr, &rg, nullptr );
    clear( color::BLACK );
  }


  void app_receive_udp_packet( PrmAppReceiveUDPPacket prm )
  {
    BREAK;
  }
};

