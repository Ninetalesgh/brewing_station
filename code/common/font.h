#pragma once

#include <common/memory.h>

namespace font
{
  struct GlyphTable;

  GlyphTable* init_glyph_table_from_ttf( memory::Arena* arena, u8 const* ttf_data );
  void        delete_glyph_table( GlyphTable* );

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
      //assert( unicodeCodepoint > 0 && unicodeCodepoint < 128 );

      Glyph& result = asciiGlyphs[unicodeCodepoint];
      if ( !result.advance ) make_glyph( unicodeCodepoint );
      return &result;
    }

    void set_scale( float scale );

    //TODO variable scale
    float scale;
    float scaleForPixelHeight;
    memory::Arena* arena;
    void* fontInfo;
    //TODO hash table this 
    Glyph asciiGlyphs[1024];

  private:
    void make_glyph( s32 unicodeCodepoint );
    GlyphTable( GlyphTable const& ) {}
  };


  //////////////


  s32 get_unicode_codepoint( char const* string, s32* out_extraBytes = nullptr );


  struct FormattingToken
  {
    char const* value;
  };

  struct TextFormatterBasic
  {

    FormattingToken token[16];
  };

  struct ParserUTF8
  {
    ParserUTF8( char const* string ) : reader( string ) {}
    ParserUTF8( char const* string, char const* formattingToken ) : reader( string ), formattingToken( formattingToken ) {}

    s32 get_next_codepoint()
    {
      s32 extraBytes = 0;
      s32 result = 0;
      if ( reader )
      {
        result = get_unicode_codepoint( reader, &extraBytes );
        reader = reader + 1 + extraBytes;
      }

      return result;
    }

    INLINE s32 peek_next_codepoint() const
    {
      return get_unicode_codepoint( reader );
    }

    char const* reader;
    char const* formattingToken;
  };

  void DEBUG_unicode_codepoints();
};