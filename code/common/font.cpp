#include "font.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "extern/stb_truetype.h"

namespace font
{

  Glyph make_glyph( memory::Arena* arena, stbtt_fontinfo const* fontInfo, float scale, char c )
  {
    Glyph glyph {};

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

  struct RasterizedGlyphCollectionSTB : public RasterizedGlyphCollection
  {
    stbtt_fontinfo stbFontInfo;
  };

  RasterizedGlyphCollection* load_ttf( memory::Arena permanentStorage, u8 const* ttf_data )
  {
    RasterizedGlyphCollectionSTB* result = nullptr;

    if ( stbtt_GetFontOffsetForIndex( ttf_data, 0 ) == 0 )
    {
      result = (RasterizedGlyphCollectionSTB*) permanentStorage.alloc( sizeof( RasterizedGlyphCollectionSTB ) );
      *result = {};
      if ( stbtt_InitFont( &result->stbFontInfo, ttf_data, 0 ) )
      {
        result->asset_type = RasterizedGlyphCollection::ASSET_TYPE::STB_TTF;
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

    return result;
  }

  GlyphMap load_from_ttf( memory::Arena arena, u8 const* ttf_data, char const* text )
  {
    GlyphMap glyphMap {};

    RasterizedGlyphCollectionSTB* glyphCollection = (RasterizedGlyphCollectionSTB*) load_ttf( arena, ttf_data );

    //stb_truetype::init_memory_arena( &arena );
    //stbtt_fontinfo fontInfo;

    // if ( stbtt_GetFontOffsetForIndex( ttf_data, 0 ) == 0 )
    // {
    //   stbtt_InitFont( &fontInfo, ttf_data, stbtt_GetFontOffsetForIndex( ttf_data, 0 ) );
    // }

    stbtt_fontinfo& fontInfo = glyphCollection->stbFontInfo;

    float scale = stbtt_ScaleForPixelHeight( &fontInfo, 128.0f );
    Glyph glyph {};

    char const* reader = text;
    int writeIndex = 0;
    while ( *reader )
    {
      glyphMap.glyphs[writeIndex++] = make_glyph( &arena, &fontInfo, scale, *reader );
      ++reader;
    }



    // stb_truetype::deinit_memory_arena();

    return glyphMap;
  }

}

