#pragma once

#include "platform/bs_platform.h"
#include "common/bsmath.h"
#include "core/bsinput.h"
#include "common/bs_bitmap.h"
#include "common/bs_color.h"
#include "core/bs_allocator.h"
#include "first_app_image.h"
#include "barbie.h"

struct AppUserData
{
  float rotator = 0;
  bs::Bitmap bmp;
};

AppUserData* app;

static bs::input::State* inputPtr;
int const windowWidth = DEFAULT_WINDOW_SIZE.x;
int const windowHeight = DEFAULT_WINDOW_SIZE.y - 40;

struct Sprite
{
  bs::Bitmap image;
  int2 position;
};

void start();
void update();
u32 is_key_held( u32 key );
void plot( int2 pos, u32 color );
void draw_rect( int2 begin, int2 end, u32 color );
void draw_circle( int2 pos, float radius, u32 color );
void clear( u32 clearColor );


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
  float dy = float( end.y - begin.y ) / float( end.x - begin.x );

  // if (fabsf(dy) > 1.0f)
  // {
  //   //switch to dx
  // }
  // else
  // {
  //   if ()
  // }
  // float const beginy = min( begin.y, end.y );
  // float dx = 1.0f;
  // if ( dy > 1.0f )
  // {
  //   dx = 1.0f / dy;
  //   dy = 1.0f;
  // }

  float y = float( begin.y );

  {
    for ( s32 x = begin.x; x < end.x; ++x )
    {
      plot( { x, s32( y ) }, color );
      y += dy;
    }
  }
}

void draw_bitmap( int2 pos, bs::Bitmap const& bmp )
{
  u32 const targetWidth = app->bmp.width;

  int2 begin = max( pos, int2 { 0, 0 } );
  int2 end = min( pos + bmp.dimensions, DEFAULT_WINDOW_SIZE );

  bs::Bitmap& target = app->bmp;

  s32 row = 0;
  for ( s32 y = begin.y; y < end.y; ++y )
  {
    u32* write = &(((u32*) target.pixel)[begin.x + y * target.width]);
    u32* read = &(((u32*) bmp.pixel)[row++ * bmp.width]);

    for ( s32 x = begin.x; x < end.x; ++x )
    {
      *write = *read;
      ++write;
      ++read;
    }
  }

}

void draw_sprite( Sprite const& sprite )
{
  draw_bitmap( sprite.position, sprite.image );
}

void draw_pokeball_optimized( s32 radius, int2 pos )
{
  //TODO
}

void draw_pokeball( s32 radius, int2 pos )
{
  s32 begin = -radius;
  s32 end =  radius;
  int2 squarebegin = { pos.x - radius, pos.y - radius / 10 };
  int2 squareend = { pos.x + radius, pos.y + radius / 10 };

  for ( s32 y = begin; y < end; ++y )
    for ( s32 x = begin; x < end; ++x )
    {
      if ( x * x + y * y < radius * radius )
      {
        int2 tmpPos = pos + int2 { x, y };
        plot( tmpPos, color::interpolate( float( x + radius ) / float( 2.0f * radius ), color::PINK, color::WHITE ) );
      }
    }
  for ( s32 y = begin; y < end / end; ++y )
    for ( s32 x = begin; x < end; ++x )
    {
      if ( x * x + y * y < radius * radius )
      {
        int2 tmpPos = pos - int2 { x, y };
        plot( tmpPos, color::WHITE );
      }
    }
  // for(s32 y = begin; y < end / end; ++y )
  //  for ( s32 x = begin; x < end; ++x )
  // {
  //   if ( x * x + y * y < radius * radius )
  //   {
  //     int2 tmpPos = pos - int2 { x, y };
  //     plot( tmpPos, color::BLACK );
  //   }
  // }
  for ( s32 y = squarebegin.y; y < squareend.y; ++y )
    for ( s32 x = squarebegin.x; x < squareend.x; ++x )
    {
      int2 tmpPos = { x, y };
      plot( tmpPos, color::BLACK );
    }
  for ( s32 y = begin; y < end; ++y )
    for ( s32 x = begin; x < end; ++x )
    {
      if ( x * x + y * y < radius / 4 * radius / 4 )
      {
        int2 tmpPos = pos - int2 { x, y };
        plot( tmpPos, color::BLACK );
      }
    }
  for ( s32 y = begin; y < end; ++y )
    for ( s32 x = begin; x < end; ++x )
    {
      if ( x * x + y * y < radius / 6 * radius / 6 )
      {
        int2 tmpPos = pos - int2 { x, y };
        plot( tmpPos, color::WHITE );
      }
    }

}













namespace bs
{
  void app_on_load( bsp::AppData* appData, bsp::PlatformCallbacks* platform )
  {
    if ( appData->userData == nullptr )
    {
      appData->userData = (AppUserData*) allocate( platform->default.allocator, sizeof( AppUserData ) );
      app = (AppUserData*) appData->userData;
      *app = AppUserData();
      app->bmp.pixel = (u32*) allocate( platform->default.allocator, sizeof( u32 ) * windowWidth * windowHeight );
      app->bmp.height = windowHeight;
      app->bmp.width = windowWidth;
    }

    app = (AppUserData*) appData->userData;
    inputPtr = &appData->input;

    start();
  }

  void app_tick( bsp::AppData* appData, bsp::PlatformCallbacks* platform )
  {
    update();
    // graphics::RenderGroup rg = graphics::render_group_from_custom_bitmap( &app->bmp );
  //  platform::render( nullptr, &rg, nullptr );
    platform->render_custom_bitmap( &app->bmp );
    clear( color::BLACK );
  }
};