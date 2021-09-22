#pragma once

#include <platform.h>
#include <common/font.h>
#include <common/basic_math.h>
#include <common/color.h>

//only alpha
struct Bitmap8
{
  u8* pixel;
  s32 width;
  s32 height;
};

//alpha + grayscale
struct Bitmap16
{
  u16* pixel;
  s32 width;
  s32 height;
};

struct Bitmap24
{
  #pragma pack(push, 1)
  struct RGB
  {
    u8 r;
    u8 g;
    u8 b;
  };
  #pragma pack(pop)

  Bitmap24::RGB* pixel;
  s32 width;
  s32 height;
};

struct Bitmap
{
  u32* pixel;
  s32 width;
  s32 height;
};


///////// ALL STILL DEBUG MATERIAL


void bitmap_draw_no_alpha( Bitmap* target, Bitmap const& source, float2 pos )
{
  int2 start = clamp( int2( pos ), { 0, 0 }, { target->width, target->height } );
  int2 end = clamp( int2 { s32( pos.x ) + source.width, s32( pos.y ) + source.height }, { 0, 0 }, { target->width, target->height } );

  s32 row = 0;
  for ( s32 y = start.y; y < end.y; ++y )
  {
    u32* write = &(((u32*) target->pixel)[y * target->width]);
    u32* read = &(((u32*) source.pixel)[row++ * source.width]);

    for ( s32 x = start.x; x < end.x; ++x )
    {
      //*write = color_blend( *write, *read );
      *write =  *read;
      ++write;
      ++read;
    }
  }
}

void bitmap_draw( Bitmap* target, Bitmap const& source, float2 pos )
{
  int2 start = clamp( int2( pos ), { 0, 0 }, { target->width, target->height } );
  int2 end = clamp( int2 { s32( pos.x ) + source.width, s32( pos.y ) + source.height }, { 0, 0 }, { target->width, target->height } );

  s32 row = 0;
  for ( s32 y = start.y; y < end.y; ++y )
  {
    u32* write = &(((u32*) target->pixel)[start.x + y * target->width]);
    u32* read = &(((u32*) source.pixel)[row++ * source.width]);

    for ( s32 x = start.x; x < end.x; ++x )
    {
      *write = color_blend( *write, *read );
      ++write;
      ++read;
    }
  }
}

void bitmap_draw( Bitmap* target, Bitmap8 const& source, float2 pos )
{
  int2 start = clamp( int2( pos ), { 0, 0 }, { target->width, target->height } );
  int2 end = clamp( int2 { s32( pos.x ) + source.width, s32( pos.y ) + source.height }, { 0, 0 }, { target->width, target->height } );

  s32 row = 0;
  for ( s32 y = start.y; y < end.y; ++y )
  {
    u32* write = &(((u32*) target->pixel)[start.x + y * target->width]);
    u8* read = &(((u8*) source.pixel)[row++ * source.width]);

    for ( s32 x = start.x; x < end.x; ++x )
    {
      u8 alpha = 0xff - *read;
      u8 R = 0xff * alpha;
      u8 G = 0xff * alpha;
      u8 B = 0xff * alpha;
      *write = color_RGB( R, G, B );
      ++write;
      ++read;
    }
  }
}


