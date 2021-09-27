#include "font.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "extern/stb_truetype.h"

namespace font
{
  Glyph make_glyph( memory::Arena* arena, stbtt_fontinfo const* fontInfo, float scale, s32 unicodeCodepoint )
  {
    Glyph glyph {};

    if ( scale > 0.0f )
    {
      s32 glyphIndex = stbtt_FindGlyphIndex( fontInfo, unicodeCodepoint );

      if ( glyphIndex > 0 )
      {
        stbtt_GetGlyphHMetrics( fontInfo, glyphIndex, &glyph.advance, &glyph.lsb );

        s32 ix0, ix1, iy0, iy1;
        float scale_x = scale;
        float scale_y = scale;
        float shift_x = 0;
        float shift_y = 0;
        float flatnessInPixels = 0.35f;
        stbtt__bitmap gbm;

        stbtt_GetGlyphBitmapBoxSubpixel( fontInfo, glyphIndex, scale_x, scale_y, shift_x, shift_y, &ix0, &iy0, &ix1, &iy1 );

        gbm.w = (ix1 - ix0);
        gbm.h = (iy1 - iy0);
        gbm.pixels = nullptr;

        if ( gbm.w && gbm.h )
        {
          gbm.pixels = (unsigned char*) arena->alloc( gbm.w * gbm.h );

          glyph.width = gbm.w;
          glyph.height = gbm.h;
          glyph.offsetX = ix0;
          glyph.offsetY = iy0;
          glyph.data = gbm.pixels;
          if ( gbm.pixels )
          {
            gbm.stride = gbm.w;
            stbtt_vertex* vertices = nullptr;
            s32 num_verts = stbtt_GetGlyphShapeThreadSafe( arena, fontInfo, glyphIndex, &vertices );
            stbtt_RasterizeThreadSafe( arena, &gbm, flatnessInPixels, vertices, num_verts, scale_x, scale_y, shift_x, shift_y, ix0, iy0, 1, fontInfo->userdata );
            arena->free( vertices );
          }
          else
          {
            BREAK;
          }
        }
        else if ( glyph.advance )
        {
          //it's a space
        }
        else
        {
          BREAK;
        }
      }
      else // !( glyphIndex > 0 )
      {
        BREAK;
      }
    }
    else // !( scale > 0.0f )
    {
      BREAK;
    }

    return glyph;
  }

  void GlyphTable::make_glyph( s32 unicodeCodepoint )
  {
    Glyph glyph = font::make_glyph( arena, (stbtt_fontinfo*) fontInfo, scale, unicodeCodepoint );
    asciiGlyphs[unicodeCodepoint] = glyph;
  }

  GlyphTable* load_glyph_table_from_ttf( memory::Arena* arena, u8 const* ttf_data )
  {
    GlyphTable* result = nullptr;

    if ( stbtt_GetFontOffsetForIndex( ttf_data, 0 ) == 0 )
    {
      result = (GlyphTable*) arena->alloc_set_zero( sizeof( GlyphTable ) );

      stbtt_fontinfo* stbFontInfo = (stbtt_fontinfo*) arena->alloc( sizeof( stbtt_fontinfo ) );
      *stbFontInfo = {};

      if ( stbtt_InitFont( stbFontInfo, ttf_data, 0 ) )
      {
        result->asset_type = GlyphTable::ASSET_TYPE::STB_TTF;
        result->scale = stbtt_ScaleForPixelHeight( stbFontInfo, 128.0f );
        result->arena = arena;
        result->fontInfo = stbFontInfo;
      }
      else
      {
        arena->free( stbFontInfo );
        arena->free( result );
        result = nullptr;
        BREAK;
      }
    }
    else
    {
      BREAK;
    }

    return result;
  }

  void unload_glyph_table( GlyphTable* glyphTable )
  {
    memory::Arena* arena = glyphTable->arena;

    for ( s32 i = 0; i < array_size( glyphTable->asciiGlyphs ); ++i )
    {
      Glyph& glyph = glyphTable->asciiGlyphs[i];
      if ( glyph.data )
      {
        arena->free( glyph.data );
      }
      glyph = {};
    }

    arena->free( glyphTable->fontInfo );
    arena->free( glyphTable );
  }

  s32 get_unicode_codepoint( char const* string )
  {
    s32 result = 0;
    u8 const* reader = (u8 const*) string;

    u8 const extraByteCheckMask = 0b10000000;
    u8 const extraByteValueMask = 0b00111111;

    u8 unicodeMask = 0b11000000;
    u8 check = 0b00100000;

    s32 extraBytes = 0;

    result = *(u8*) (reader);
    while ( *string & unicodeMask )
    {
      unicodeMask >>= 1;
      check >>= 1;

      ++reader;
      if ( *reader & extraByteCheckMask )
      {
        result <<= 6;
        result += (*reader) & extraByteValueMask;

        ++extraBytes;
      }
      else
      {
        break;
      }
    }

    if ( extraBytes )
    {
      s32 maskLength = 1 << (5 * extraBytes + 6);
      result &= (maskLength - 1);
    }

    return result;
  }

  void DEBUG_unicode_codepoints()
  {
    assert( font::get_unicode_codepoint( "ک" ) == 1705 );
    assert( font::get_unicode_codepoint( "ক" ) == 2453 );
    assert( font::get_unicode_codepoint( "߷" ) == 2039 );
    assert( font::get_unicode_codepoint( "ӂ" ) == 1218 );
    assert( font::get_unicode_codepoint( "֍" ) == 1421 );
    assert( font::get_unicode_codepoint( "ཚ" ) == 3930 );
    assert( font::get_unicode_codepoint( "ጪ" ) == 4906 );
    assert( font::get_unicode_codepoint( "€" ) == 8364 );
    assert( font::get_unicode_codepoint( "∑" ) == 8721 );
    assert( font::get_unicode_codepoint( "✊" ) == 9994 );
  }



  // GlyphMap load_from_ttf( memory::Arena* arena, u8 const* ttf_data, char const* text )
  // {
  //   GlyphMap glyphMap {};

  //   GlyphTable* glyphCollection = load_glyph_table_from_ttf( arena, ttf_data );

  //   stbtt_fontinfo* stbFontInfo = (stbtt_fontinfo*) glyphCollection->fontInfo;

  //   float scale = stbtt_ScaleForPixelHeight( stbFontInfo, 128.0f );
  //   Glyph glyph {};

  //   char const* reader = text;
  //   int writeIndex = 0;
  //   while ( *reader )
  //   {
  //     glyphMap.glyphs[writeIndex++] = make_glyph( arena, stbFontInfo, scale, *reader );
  //     ++reader;
  //   }

  //   return glyphMap;
  // }

}

