#pragma once

#include <common/memory.h>

namespace font
{
  struct GlyphTable;

  GlyphTable* load_glyph_table_from_ttf( memory::Arena* arena, u8 const* ttf_data );
  void        unload_glyph_table( GlyphTable* );

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

    INLINE Glyph const* get_glyph( s32 unicodeCodepoint )
    {
      assert( unicodeCodepoint > 0 && unicodeCodepoint < 128 );

      Glyph& result = asciiGlyphs[unicodeCodepoint];
      if ( !result.advance ) make_glyph( unicodeCodepoint );
      return &result;
    }

    //TODO variable scale
    float scale;
    memory::Arena* arena;
    void* fontInfo;
    Glyph asciiGlyphs[128];

  private:
    void make_glyph( s32 unicodeCodepoint );
    GlyphTable( GlyphTable const& ) {}
  };

  s32 get_unicode_codepoint( char const* string );

  struct UnicodeParser
  {
    UnicodeParser( char const* string ) : reader( string ) {}

    s32 get_next_codepoint()
    {
      s32 result = get_unicode_codepoint( reader );
      if ( result )
      {
        while ( *reader++ < 0 ) {} //TODO
      }
      return result;
    }

    char const* reader;
  };



  void DEBUG_unicode_codepoints();
}