#pragma once

#include <core/bsgraphics.h>
#include <core/bsmemory.h>
#include <core/bsdebuglog.h>
#include <common/bscolor.h>
#include <platform/platform.h>


void update();
void start();
void print_text( char const* text );

static bs::graphics::Bitmap* bmp;
static bs::input::State* inputPtr;

INLINE u32 is_key_held( u32 key )
{
  return bs::input::STATE_COUNT && inputPtr->held[key];
}

INLINE void clear( u32 clearColor )
{
  s32 end = bmp->width * bmp->height;
  for ( s32 i = 0; i < end; ++i )
  {
    bmp->pixel[i] = clearColor;
  }
}

INLINE void plot( int2 pos, u32 color )
{
  if ( pos.x >= 0.0f && pos.x < bmp->width &&
       pos.y >= 0.0f && pos.y < bmp->height )
  {
    bmp->pixel[pos.x + pos.y * bmp->width] = color;
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
  graphics::Bitmap* create_bitmap( s32 width, s32 height )
  {
    u8* allocation = (u8*) memory::allocate_to_zero( sizeof( graphics::Bitmap ) + sizeof( u32 ) * width * height );
    graphics::Bitmap* resultBmp = (graphics::Bitmap*) allocation;
    resultBmp->width = width;
    resultBmp->height = height;
    resultBmp->pixel = (u32*) allocation + sizeof( graphics::Bitmap );
    return resultBmp;
  }

  void app_sample_sound( PrmAppSampleSound prm )
  {
    BREAK;
  }

  void app_on_load( PrmAppOnLoad prm )
  {
    bmp = create_bitmap( DEFAULT_WINDOW_SIZE.x, DEFAULT_WINDOW_SIZE.y );
    inputPtr = &prm.appData->input;
    start();
  }

  void app_tick( PrmAppTick prm )
  {
    update();
    graphics::RenderGroup rg = graphics::render_group_from_custom_bitmap( bmp );
    platform::render( nullptr, &rg, nullptr );
    clear( color::BLACK );
  }


  void app_receive_udp_packet( PrmAppReceiveUDPPacket prm )
  {
    BREAK;
  }
};

