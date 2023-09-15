#pragma once

#include "platform/bs_platform.h"
#include "common/bsmath.h"
#include "common/bs_bitmap.h"
#include "common/bs_color.h"
#include "brewing_station_core.h"
#include "core/bsinput.h"
#include "core/bs_allocator.h"
#include "core/bs_filesystem.h"

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
void draw_bitmap( int2 pos, bs::Bitmap const& bmp );
void draw_sprite( Sprite const& sprite );

Sprite load_sprite( char const* imagePath );
void unload_sprite( Sprite& sprite );


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


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

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

#define STB_IMAGE_IMPLEMENTATION

namespace stb_image
{
  void* bs_realloc( void* data, size_t oldSize, size_t newSize )
  {
    void* newData = ALLOCATE( newSize );
    if ( data )
    {
      memcpy( newData, data, oldSize );
      FREE( data );
    }
    return newData;
  }
}

#define STBI_MALLOC(sz)           ALLOCATE(sz)
#define STBI_REALLOC_SIZED(p,oldsz,newsz)     stb_image::bs_realloc(p,oldsz,newsz)
#define STBI_FREE(p)              FREE(p)

#include "core/internal/stb_image.h"

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
    platform->render_custom_bitmap( &app->bmp );
    clear( color::BLACK );
  }

  Bitmap load_image( char const* path )
  {
    File* imageFile = load_file( path );

    Bitmap result = {};
    s32 channelsPerSample;
    constexpr s32 bytesPerChannel = 4;
    assert( imageFile->size <= INT_MAX );
    result.pixel = (u32*) stbi_load_from_memory( (u8*) imageFile->data, s32( imageFile->size ), &result.width, &result.height, &channelsPerSample, bytesPerChannel );
    unload_file( imageFile );

    u32 const* end = result.pixel + result.width * result.height;
    u32* writer = result.pixel;
    while ( writer < end )
    {
      *writer = color::srgba_to_rgba( *writer );
      ++writer;
    }
    return result;
  }

  void unload_image( Bitmap& bmp )
  {
    STBI_FREE( bmp.pixel );
  }
};

Sprite load_sprite( char const* imagePath )
{
  Sprite result {};
  result.image = bs::load_image( imagePath );
  result.position = { 0,0 };
  return result;
}

void unload_sprite( Sprite& sprite )
{
  bs::unload_image( sprite.image );
}
