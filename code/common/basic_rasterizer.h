#pragma once

#include "color.h"
#include "basic_math.h"

#include "bitmap.h"

struct Rect
{
  float2 min;
  float2 max;
};

enum struct RasterizeMode : u32
{
  EMPTY = 0,
  FILL
};


void draw_rect( Bitmap* target, Rect rect, u32 color = color::RED, RasterizeMode mode = RasterizeMode::FILL )
{
  assert( mode == RasterizeMode::FILL );//TODO

  int2 min = (int2) rect.min;
  int2 max = (int2) rect.max;

  for ( s32 y = min.y; y < max.y; ++y )
  {
    //u32* write = &(target->pixel[y * target->width]);
    u32* write = &(((u32*) target->pixel)[min.x + y * target->width]);
    for ( s32 x = min.x; x < max.x; ++x )
    {
      *write++ = color;
    }
  }
}

void draw_rect( Bitmap* target, float2 start, float2 end, u32 color = color::RED, RasterizeMode mode = RasterizeMode::FILL ) { draw_rect( target, { min( start, end ), max( start, end ) }, color ); }
