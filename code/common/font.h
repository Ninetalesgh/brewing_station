#pragma once

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

  struct RasterizedGlyphCollection
  {
    enum class ASSET_TYPE : u32
    {
      ERROR = 0,
      STB_TTF = 1,
      BSA = 2,
    } asset_type;
  };

  RasterizedGlyphCollection* load_ttf( memory::Arena permanentStorage, u8 const* ttf_data );

  GlyphMap load_from_ttf( memory::Arena arena, u8 const* ttf_data, char const* text );

}