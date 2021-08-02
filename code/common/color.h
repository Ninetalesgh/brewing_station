#pragma once
#include "basic_types.h"
#include "preprocessor.h"

constexpr u32 WHITE      = 0xffffffff;
constexpr u32 BLACK      = 0xff000000;
constexpr u32 DARK_GRAY  = 0xff1e1e1e;
constexpr u32 LIGHT_GRAY = 0xffaaaaaa;
constexpr u32 RED        = 0xffff0000;
constexpr u32 GREEN      = 0xff00ff00;
constexpr u32 BLUE       = 0xff0000ff;
constexpr u32 TEAL       = 0xff00ffff;
constexpr u32 PURPLE     = 0xffff00ff;
constexpr u32 YELLOW     = 0xffffff00;

constexpr u32 MASK_ALPHA = 0xff000000;
constexpr u32 MASK_RED   = 0x00ff0000;
constexpr u32 MASK_GREEN = 0x0000ff00;
constexpr u32 MASK_BLUE  = 0x000000ff;


INLINE u32 color_RGBAtoBGRA( u32 color ) { return (MASK_RED & (color << 16)) | (MASK_GREEN & color) | (MASK_BLUE & (color >> 16)) | (MASK_ALPHA & color); }
INLINE u32 color_RGBA( u8 r, u8 g, u8 b, u8 a ) { return (u32) (a << 24) | (u32) (r << 16) | (u32) (g << 8) | b; }
INLINE u32 color_RGB( u8 r, u8 g, u8 b ) { return (u32) (r << 16) | (u32) (g << 8) | b; }

INLINE u32 color_interpolate( float t, u32 colorA, u32 colorB )
{
  t = clamp( t, 0.0f, 1.0f );

  float const aa = float( (colorA & MASK_ALPHA) >> 24 );
  float const ab = float( (colorB & MASK_ALPHA) >> 24 );
  u8 const a = u8( ab * t + aa * (1.0f - t) );

  float const ra = float( (colorA & MASK_RED) >> 16 );
  float const rb = float( (colorB & MASK_RED) >> 16 );
  u8 const r = u8( rb * t + ra * (1.0f - t) );

  float const ga = float( (colorA & MASK_GREEN) >> 8 );
  float const gb = float( (colorB & MASK_GREEN) >> 8 );
  u8 const g = u8( gb * t + ga * (1.0f - t) );

  float const ba = float( colorA & MASK_BLUE );
  float const bb = float( colorB & MASK_BLUE );
  u8 const b = u8( bb * t + ba * (1.0f - t) );

  return (u32( a ) << 24) | (u32( r ) << 16) | (u32( g ) << 8) | u32( b );
}

INLINE u32 color_interpolate( u8 t, u32 colorA, u32 colorB )
{
  u32 const t32 = u32( t );

  u32 const aa = ((colorA & MASK_ALPHA) >> 24);
  u32 const ab = ((colorB & MASK_ALPHA) >> 24);
  u32 const a = ((ab * t32) / 255 + (aa * (255 - t32)) / 255) << 24;

  u32 const ra = ((colorA & MASK_RED) >> 16);
  u32 const rb = ((colorB & MASK_RED) >> 16);
  u32 const r = ((rb * t32) / 255 + (ra * (255 - t32)) / 255) << 16;

  u32 const ga = ((colorA & MASK_GREEN) >> 8);
  u32 const gb = ((colorB & MASK_GREEN) >> 8);
  u32 const g = ((gb * t32) / 255 + (ga * (255 - t32)) / 255) << 8;

  u32 const ba = (colorA & MASK_BLUE);
  u32 const bb = (colorB & MASK_BLUE);
  u32 const b = ((bb * t32) / 255 + (ba * (255 - t32)) / 255);

  return a | r | g | b;
}

INLINE u32 color_blend( u32 bottom, u32 top )
{
  u8 t = u8( (top & MASK_ALPHA) >> 24 );

  //TODO: no clue if this is actually correct 
  return color_interpolate( t, bottom, top );
}