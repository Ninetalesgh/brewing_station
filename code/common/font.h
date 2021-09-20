#pragma once

#include <common/bitmap.h>
#include <common/memory.h>

namespace font
{
  struct Glyph
  {
    u8* data;
    s32 advance;
    s32 lsb;

    s32 width;
    s32 height;
    s32 offsetX;
    s32 offsetY;
  };

  struct GlyphMap
  {
    Glyph glyphs[16];
  };

  struct TrueTypeFont
  {
  };

  GlyphMap load_from_ttf( memory::Arena arena, u8 const* ttf_data, char const* text );

  TrueTypeFont* load_ttf( memory::Arena permanentStorage, u8 const* ttf_data );


}
