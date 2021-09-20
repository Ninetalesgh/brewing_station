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

  struct TrueTypeFontSTB : public TrueTypeFont
  {
    stbtt_fontinfo stbFontInfo;
  };

  TrueTypeFont* load_ttf( memory::Arena permanentStorage, u8 const* ttf_data )
  {
    TrueTypeFontSTB* result = nullptr;

    if ( stbtt_GetFontOffsetForIndex( ttf_data, 0 ) == 0 )
    {
      result = (TrueTypeFontSTB*) permanentStorage.alloc( sizeof( TrueTypeFontSTB ) );
      *result = {};
      stbtt_InitFont( &result->stbFontInfo, ttf_data, 0 );
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

    TrueTypeFont* font = load_ttf( arena, ttf_data );

    stb_truetype::init_memory_arena( &arena );

    stbtt_fontinfo fontInfo;


    if ( stbtt_GetFontOffsetForIndex( ttf_data, 0 ) == 0 )
    {
      stbtt_InitFont( &fontInfo, ttf_data, stbtt_GetFontOffsetForIndex( ttf_data, 0 ) );
    }

    // s32 codepoint = 'A';
     //Bitmap8 bmp {};

    // {
       // PROFILE_SCOPE( profile_codepoint );
    // }


    float scale = stbtt_ScaleForPixelHeight( &fontInfo, 128.0f );
    Glyph glyph {};

    char const* reader = text;
    int writeIndex = 0;
    while ( *reader )
    {
      glyphMap.glyphs[writeIndex++] = make_glyph( &arena, &fontInfo, scale, *reader );
      ++reader;
    }



    // for ( char c = 'a'; c < 128; ++c )
    // {
       // s32 glyphIndex = stbtt_FindGlyphIndex( &fontInfo, c );

       // stbtt_GetGlyphHMetrics( &fontInfo, glyphIndex, &glyph.advance, &glyph.lsb );
       // s32 x0, x1, y0, y1;
       // stbtt_GetGlyphBitmapBoxSubpixel( &fontInfo, glyphIndex, /*scalex*/ scale, scale, /*shiftx*/ 0,/*shifty*/ 0, &x0, &y0, &x1, &y1 );

       // glyph.width = x1 - x0;
       // glyph.height = y1 - y0;
       // glyph.offsetX = x0;
       // glyph.offsetY = y0;
       // glyph.data = (u8*) arena.alloc( glyph.width * glyph.height );

       // stbtt_MakeGlyphBitmapSubpixel( &fontInfo, glyph.data, glyph.width, glyph.height, glyph.width, scale, scale, /*shiftx*/ 0,/*shifty*/ 0, glyphIndex );
       // //stbtt_MakeCodepointBitmapSubpixel( &fontInfo, glyph.data, glyph.width, glyph.height, glyph.width, scale, scale, /*shiftx*/ 0, /*shifty*/ 0, c );

       // break;
    // }



     // float xpos = 2;
     // int advance, lsb, x0, y0, x1, y1;
     // float x_shift = xpos - (float) floor( xpos );
 //    stbtt_GetCodepointHMetrics( &fontInfo, codepoint, &advance, &lsb );
     // stbtt_GetCodepointBitmapBoxSubpixel( &fontInfo, codepoint, scale, scale, x_shift, 0, &x0, &y0, &x1, &y1 );
      //stbtt_MakeCodepointBitmapSubpixel( &fontInfo, &screen[baseline + y0][(int) xpos + x0], x1 - x0, y1 - y0, 79, scale, scale, x_shift, 0, codepoint );

     //Glyph

    // local_persist s32 glyphIndex = 1;
    // bmp.pixel = stbtt_GetGlyphBitmap( &fontInfo, 0, stbtt_ScaleForPixelHeight( &fontInfo, scale ), glyphIndex++, &bmp.width, &bmp.height, 0, 0 );


     // int glyphIndexA = stbtt_FindGlyphIndex( &fontInfo, codepoint );
     // int glyphIndexB = stbtt_FindGlyphIndex( &fontInfo, 'B' );
      // bmp.pixel = stbtt_GetGlyphBitmapSubpixel( fontInfo, scale_x, scale_y, shift_x, shift_y, glyphIndex, width, height, xoff, yoff );


    stb_truetype::deinit_memory_arena();

    return glyphMap;
  }

}

