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

  struct GlyphTable
  {
    //TODO maybe this enum isn't needed
    enum class ASSET_TYPE : u32
    {
      ERROR = 0,
      STB_TTF = 1,
      BSA = 2,
    } asset_type;

    INLINE Glyph const* get_glyph( char asciiChar )
    {
      assert( asciiChar > 0 );

      Glyph& result = asciiGlyphs[asciiChar];
      if ( !result.data ) make_glyph( asciiChar );
      return &result;
    }

    //TODO variable scale
    float scale;
    memory::Arena* arena;
    void* fontInfo;
    Glyph asciiGlyphs[128];

  private:
    void make_glyph( char asciiChar );
    GlyphTable( GlyphTable const& ) {}
  };

  GlyphTable* load_glyph_table_from_ttf( memory::Arena* arena, u8 const* ttf_data );

}