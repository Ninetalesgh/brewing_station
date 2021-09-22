#include "font.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "extern/stb_truetype.h"

namespace font
{
  Glyph make_glyph( memory::Arena* arena, stbtt_fontinfo const* fontInfo, float scale, char c )
  {
    Glyph glyph;

    if ( scale > 0.0f )
    {
      s32 glyphIndex = stbtt_FindGlyphIndex( fontInfo, c );

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
        stbtt_vertex* vertices = nullptr;

        stbtt_GetGlyphBitmapBoxSubpixel( fontInfo, glyphIndex, scale_x, scale_y, shift_x, shift_y, &ix0, &iy0, &ix1, &iy1 );

        gbm.w = (ix1 - ix0);
        gbm.h = (iy1 - iy0);
        gbm.pixels = nullptr;

        glyph.width = gbm.w;
        glyph.height = gbm.h;
        glyph.offsetX = ix0;
        glyph.offsetY = iy0;

        if ( gbm.w && gbm.h )
        {
          gbm.pixels = (unsigned char*) arena->alloc( gbm.w * gbm.h );
          if ( gbm.pixels )
          {
            gbm.stride = gbm.w;

            s32 num_verts = stbtt_GetGlyphShapeThreadSafe( arena, fontInfo, glyphIndex, &vertices );

            stbtt_RasterizeThreadSafe( arena, &gbm, flatnessInPixels, vertices, num_verts, scale_x, scale_y, shift_x, shift_y, ix0, iy0, 1, fontInfo->userdata );
          }
          else
          {
            BREAK;
          }
        }
        else
        {
          BREAK;
        }
        arena->free( vertices );
        glyph.data = gbm.pixels;
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

  void GlyphTable::make_glyph( char glyphCharacter )
  {
    asciiGlyphs[glyphCharacter] = font::make_glyph( arena, (stbtt_fontinfo*) fontInfo, scale, glyphCharacter );
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

