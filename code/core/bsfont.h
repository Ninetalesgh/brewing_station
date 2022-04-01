#pragma once

#include <common/bscommon.h>

//todo
/*
textfields


*/


namespace bs
{
  namespace font
  {
    //GlyphTable holds reference to unrasterized font data
    struct GlyphTable;

    struct Glyph
    {
      s32 codepoint;
      int2 uvBegin;
      int2 uvSize;
      float advance;
      float lsb;
      s32 offsetX;
      s32 offsetY;
    };

    struct GlyphSheet
    {
      Glyph* glyphs;
      s32 glyphCount;
      graphics::TextureID textureID;
      s32 width;
      s32 height;
    };

    //returns nullptr on fail
    GlyphTable* create_glyph_table_from_ttf( void const* data );

    //set scale for create_raw_glyph_data() and create_glyph_sheet() calls
    void set_scale_for_glyph_creation( GlyphTable*, float scale );

    //returns a rasterized sheet with exactly the unicode characters supplied 
    //glyphs are of scale previously set with set_scale_for_glyph_creation()
    GlyphSheet* create_glyph_sheet( GlyphTable* glyphTable, char const* utf8String );

    Glyph* get_glyph_for_codepoint( GlyphSheet* glyphSheet, s32 codepoint );

    struct RawGlyphData
    {
      u8* data;
      s32 advance;
      s32 lsb;
      s32 width;
      s32 height;
      s32 offsetX;
      s32 offsetY;
    };
    //returns nullptr if the table doesn't have the glyph to the requested codepoint
    //returns a rasterized glyph at the scale previously set with set_scale_for_glyph_creation()
    RawGlyphData* create_raw_glyph_data( GlyphTable*, s32 unicodeCodepoint );
  };
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////cpp//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include <platform/platform_callbacks.h>
#include <core/bsmemory.h>
#include <common/bscolor.h>
#include <common/bsstring.h>

//#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "internal/stb_truetype.h"

namespace bs
{
  namespace font
  {
    struct GlyphTable
    {
      graphics::ShaderProgram shaderProgram;
      float scale;
      float scaleForPixelHeight;
      stbtt_fontinfo* fontInfo;
    };

    font::GlyphTable* create_glyph_table_from_ttf( void const* data )
    {
      font::GlyphTable* glyphTable = nullptr;
      if ( stbtt_GetFontOffsetForIndex( (u8 const*) data, 0 ) == 0 )
      {
        u8* allocation = (u8*) platform::allocate_to_zero( sizeof( GlyphTable ) + sizeof( stbtt_fontinfo ) );
        glyphTable = (GlyphTable*) allocation;
        stbtt_fontinfo* stbFontInfo = (stbtt_fontinfo*) (allocation + sizeof( GlyphTable ));

        if ( stbtt_InitFont( stbFontInfo, (u8 const*) data, 0 ) )
        {
          glyphTable->fontInfo = stbFontInfo;
          set_scale_for_glyph_creation( glyphTable, 64.0f );
        }
        else
        {
          platform::free( allocation );
          glyphTable = nullptr;
          BREAK;
        }
      }
      else
      {
        BREAK;
      }

      return glyphTable;
    }

    void set_scale_for_glyph_creation( GlyphTable* glyphTable, float scale )
    {
      if ( glyphTable && glyphTable->fontInfo )
      {
        glyphTable->scale = scale;
        glyphTable->scaleForPixelHeight = stbtt_ScaleForPixelHeight( (stbtt_fontinfo*) glyphTable->fontInfo, scale );
      }
      else
      {
        BREAK;
      }
    }

    Glyph get_glyph_dimensions( GlyphTable* glyphTable, s32 unicodeCodepoint )
    {
      stbtt_fontinfo* fontInfo = glyphTable->fontInfo;
      float scale = glyphTable->scaleForPixelHeight;

      Glyph result = {};

      if ( scale > 0.0f )
      {
        s32 glyphIndex = stbtt_FindGlyphIndex( fontInfo, unicodeCodepoint );

        if ( glyphIndex > 0 )
        {
          s32 advance;
          s32 lsb;
          stbtt_GetGlyphHMetrics( fontInfo, glyphIndex, &advance, &lsb );

          s32 ix0, ix1, iy0, iy1;
          stbtt_GetGlyphBitmapBoxSubpixel( fontInfo, glyphIndex, scale, scale, 0, 0, &ix0, &iy0, &ix1, &iy1 );
          result.uvSize.x = (ix1 - ix0);
          result.uvSize.y = (iy1 - iy0);
          result.offsetX = ix0;
          result.offsetY = iy0;

          result.advance = float( advance ) * scale;
          result.lsb = float( lsb );
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

      return result;
    }

    RawGlyphData* create_raw_glyph_data( GlyphTable* glyphTable, s32 unicodeCodepoint )
    {
      stbtt_fontinfo* fontInfo = glyphTable->fontInfo;
      float scale = glyphTable->scaleForPixelHeight;

      RawGlyphData* glyph = nullptr;
      s32 advance;
      s32 lsb;

      if ( scale > 0.0f )
      {
        s32 glyphIndex = stbtt_FindGlyphIndex( fontInfo, unicodeCodepoint );

        if ( glyphIndex > 0 )
        {
          stbtt_GetGlyphHMetrics( fontInfo, glyphIndex, &advance, &lsb );

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
            u8* allocation = (u8*) platform::allocate( sizeof( RawGlyphData ) + gbm.w * gbm.h );
            glyph = (RawGlyphData*) allocation;
            gbm.pixels = allocation + sizeof( RawGlyphData );
            glyph->data = gbm.pixels;
            glyph->advance = advance;
            glyph->lsb = lsb;
            glyph->width = gbm.w;
            glyph->height = gbm.h;
            glyph->offsetX = ix0;
            glyph->offsetY = iy0;
            if ( gbm.pixels )
            {
              gbm.stride = gbm.w;
              stbtt_vertex* vertices = nullptr;
              s32 num_verts = stbtt_GetGlyphShape( fontInfo, glyphIndex, &vertices );
              stbtt_Rasterize( &gbm, flatnessInPixels, vertices, num_verts, scale_x, scale_y, shift_x, shift_y, ix0, iy0, 1, fontInfo->userdata );
              platform::free( vertices );
            }
            else
            {
              BREAK;
            }
          }
          else if ( advance )
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

    GlyphSheet* create_glyph_sheet( GlyphTable* glyphTable, char const* utf8String )
    {
      char const* reader = utf8String;
      s32 glyphCount = bs::string::length_utf8( utf8String );

      Glyph* rects = nullptr;
      GlyphSheet* resultSheet = nullptr;
      {
        u8* allocation = (u8*) memory::allocate_to_zero( sizeof( GlyphSheet ) + sizeof( Glyph ) * glyphCount );
        resultSheet = (GlyphSheet*) allocation;
        rects = (Glyph*) (allocation + sizeof( GlyphSheet ));
      }

      int2 sheetDims = 0;
      int2 currentRow = 0;
      constexpr s32 MAX_WIDTH = 512;

      s32 rectsIndex = 0;
      while ( *reader )
      {
        s32 codepoint;
        reader = bs::string::parse_utf8( reader, &codepoint );

        Glyph newGlyph = get_glyph_dimensions( glyphTable, codepoint );
        newGlyph.codepoint = codepoint;

        if ( currentRow.x + (s32) newGlyph.uvSize.x > MAX_WIDTH )
        {
          sheetDims.y += currentRow.y + 1;
          sheetDims.x = max( sheetDims.x, currentRow.x );
          currentRow = { 0, 0 };
        }

        newGlyph.uvBegin = { currentRow.x, sheetDims.y };
        rects[rectsIndex++] = newGlyph;

        currentRow.x += (s32) newGlyph.uvSize.x + 1;
        currentRow.y = max( currentRow.y, (s32) newGlyph.uvSize.y );
      }

      sheetDims.x = max( sheetDims.x, currentRow.x );
      sheetDims.y = sheetDims.y + currentRow.y;
      #define LUL
      graphics::Bitmap* sheetBMP = nullptr;
      {
        u8* allocation = (u8*) memory::allocate( sizeof( graphics::Bitmap ) + sizeof( u32 ) * sheetDims.x * sheetDims.y );
        sheetBMP = (graphics::Bitmap*) allocation;
        sheetBMP->width = sheetDims.x;
        sheetBMP->height = sheetDims.y;
        sheetBMP->pixel = (u32*) (allocation + sizeof( graphics::Bitmap ));
      }

      //rasterize glyphs
      for ( s32 i = 0; i < glyphCount; ++i )
      {
        RawGlyphData* rawGlyphData = create_raw_glyph_data( glyphTable, rects[i].codepoint );
        if ( rawGlyphData )
        {
          assert( rawGlyphData->width == (s32) rects[i].uvSize.x );
          assert( rawGlyphData->height == (s32) rects[i].uvSize.y );

          for ( s32 y = 0; y < (s32) rects[i].uvSize.y; ++y )
          {
            u32* writer = sheetBMP->pixel + (s32) rects[i].uvBegin.x + (((s32) rects[i].uvBegin.y + y) * sheetBMP->width);
            for ( s32 x = 0; x < (s32) rects[i].uvSize.x; ++x )
            {
              s32 index = x + y * rawGlyphData->width;
              //*writer++ = color::rgba( 0xff, 0xff, 0xff, rawGlyphData->data[index] );
              *writer++ = color::rgba( rawGlyphData->data[index], rawGlyphData->data[index], rawGlyphData->data[index], rawGlyphData->data[index] );
            }
          }

          memory::free( rawGlyphData );
        }
      }

      resultSheet->glyphs =     rects;
      resultSheet->glyphCount = glyphCount;
      resultSheet->textureID =  platform::allocate_texture( sheetBMP->pixel, sheetBMP->width, sheetBMP->height );
      resultSheet->width =      sheetBMP->width;
      resultSheet->height =     sheetBMP->height;

      memory::free( sheetBMP );

      return resultSheet;
    }

    Glyph* get_glyph_for_codepoint( GlyphSheet* glyphSheet, s32 codepoint )
    {
      //TODO filter and sort glypharray and binary search

      Glyph* glyph = glyphSheet->glyphs;
      Glyph* end = glyph + glyphSheet->glyphCount;
      while ( glyph->codepoint != codepoint && glyph != end ) { ++glyph; }

      return glyph == end ? nullptr : glyph;
    }
  };
};


