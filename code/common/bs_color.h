#pragma once
#include "bsmath.h"

namespace color
{
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

  INLINE u32 rgba_to_bgra( u32 color ) { return (MASK_RED & (color << 16)) | (MASK_GREEN & color) | (MASK_BLUE & (color >> 16)) | (MASK_ALPHA & color); }

  //TODO alpha channel placement is confusing
  INLINE u32 rgba( u8 r, u8 g, u8 b, u8 a ) { return (u32) (a << 24) | (u32) (r << 16) | (u32) (g << 8) | b; }
  INLINE u32 rgb( u8 r, u8 g, u8 b ) { return (u32) (r << 16) | (u32) (g << 8) | b; }

  INLINE u8 get_red( u32 color ) { return u8( (color & MASK_RED) >> 16 ); }
  INLINE u8 get_green( u32 color ) { return u8( (color & MASK_GREEN) >> 8 ); }
  INLINE u8 get_blue( u32 color ) { return u8( color & MASK_BLUE ); }
  INLINE u8 get_alpha( u32 color ) { return u8( (color & MASK_ALPHA) >> 24 ); }

  INLINE float4 float4_from_rgba( u32 color ) { return float4 { (float) get_red( color ),(float) get_green( color ),(float) get_blue( color ),(float) get_alpha( color ) } / 255.0f; }

  INLINE float get_alpha_float( u32 color ) { return float( (color & MASK_ALPHA) >> 24 ) / 255.0f; }

  INLINE u32 interpolate( float t, u32 colorA, u32 colorB )
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

  INLINE u32 interpolate( u8 t, u32 colorA, u32 colorB )
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
}

//TODO put this somewhere useful
namespace identifiercolors
{
  //u32 const text            = color::rgba( 212, 212, 212, 255 );
  //u32 const text2           = color::rgba( 50, 187, 176, 255 );
  //u32 const textDark        = color::rgba( 133, 133, 133, 255 );
  //u32 const function        = color::rgba( 224, 154, 53, 255 );
  //u32 const basicType       = color::rgba( 44, 122, 214, 255 );
  //u32 const comment         = color::rgba( 94, 153, 62, 255 );
  //u32 const preprocessor    = color::rgba( 155, 45, 129, 255 );
  //u32 const operator        = color::rgba( 197, 134, 192, 255 );
  //u32 const backgroundDark  = color::rgba( 30, 30, 30, 255 );
  //u32 const backgroundLight = color::rgba( 66, 66, 66, 255 )
};