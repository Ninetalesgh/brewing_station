#pragma once

#include <common/bscommon.h>

namespace bs
{
  namespace font
  {
    using RawTTF = file::Data;
    struct GlyphTable;

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

    struct GlyphSheet
    {
      u32* data;
      s32 width;
      s32 height;
    };

    struct GlyphSheetRect
    {
      s32 codepoint;
      int2 pos;
      int2 size;
    };

    //returns nullptr on fail
    GlyphTable* create_glyph_table_from_ttf( void const* data );

    //set scale for create_glyph() calls
    void set_scale_for_glyph_creation( GlyphTable*, float scale );

    //returns nullptr if the table doesn't have the glyph to the requested codepoint
    //returns a rasterized glyph at the scale previously set with set_scale_for_glyph_creation()
    Glyph* create_glyph( GlyphTable*, s32 unicodeCodepoint );

    //returns a rasterized sheet with exactly the unicode characters supplied 
    graphics::TextureID create_glyph_sheet( GlyphTable*, char const* utf8String, GlyphSheetRect** out_rects, s32* out_rectCount );
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
      float scale;
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
          platform::free( stbFontInfo );
          platform::free( glyphTable );
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
        glyphTable->scale = stbtt_ScaleForPixelHeight( (stbtt_fontinfo*) glyphTable->fontInfo, scale );
      }
      else
      {
        BREAK;
      }
    }

    int2 get_glyph_dimensions( GlyphTable* glyphTable, s32 unicodeCodepoint )
    {
      stbtt_fontinfo* fontInfo = glyphTable->fontInfo;
      float scale = glyphTable->scale;

      int2 result = { -1,-1 };
      if ( scale > 0.0f )
      {
        s32 glyphIndex = stbtt_FindGlyphIndex( fontInfo, unicodeCodepoint );

        if ( glyphIndex > 0 )
        {
          s32 ix0, ix1, iy0, iy1;
          stbtt_GetGlyphBitmapBoxSubpixel( fontInfo, glyphIndex, scale, scale, 0, 0, &ix0, &iy0, &ix1, &iy1 );
          result.x = (ix1 - ix0);
          result.y = (iy1 - iy0);
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

    Glyph* create_glyph( GlyphTable* glyphTable, s32 unicodeCodepoint )
    {
      stbtt_fontinfo* fontInfo = glyphTable->fontInfo;
      float scale = glyphTable->scale;

      Glyph* glyph = nullptr;
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
            u8* allocation = (u8*) platform::allocate( sizeof( Glyph ) + gbm.w * gbm.h );
            glyph = (Glyph*) allocation;
            gbm.pixels = allocation + sizeof( Glyph );
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

    GlyphSheet* create_glyph_sheet_internal( GlyphTable* glyphTable, char const* utf8String, GlyphSheetRect** out_rects, s32* out_rectCount )
    {
      char const* reader = utf8String;
      s32 glyphCount = bs::string_utf8_length( utf8String );
      GlyphSheetRect* rects = (GlyphSheetRect*) memory::allocate_to_zero( sizeof( GlyphSheetRect ) * glyphCount );

      int2 sheetDims = 0;
      int2 currentRow = 0;
      constexpr s32 MAX_WIDTH = 512;

      s32 rectsIndex = 0;
      while ( *reader )
      {
        s32 codepoint;
        reader = bs::parse_utf8( reader, &codepoint );

        int2 dims = get_glyph_dimensions( glyphTable, codepoint );

        if ( currentRow.x + dims.x > MAX_WIDTH )
        {
          sheetDims.y += currentRow.y;
          sheetDims.x = max( sheetDims.x, currentRow.x );
          currentRow = { 0, 0 };
          rects[rectsIndex++] = { codepoint, { 0, sheetDims.y }, dims };
        }
        else
        {
          rects[rectsIndex++] = { codepoint, { currentRow.x, sheetDims.y }, dims };
        }

        currentRow.x += dims.x;
        currentRow.y = max( currentRow.y, dims.y );
      }

      sheetDims.x = max( sheetDims.x, currentRow.x );
      sheetDims.y = sheetDims.y + currentRow.y;

      u8* allocation = (u8*) memory::allocate( sizeof( GlyphSheet ) + sizeof( u32 ) * sheetDims.x * sheetDims.y );
      GlyphSheet* sheet = (GlyphSheet*) allocation;
      sheet->width = sheetDims.x;
      sheet->height = sheetDims.y;
      sheet->data = (u32*) (allocation + sizeof( GlyphSheet ));

      //rasterize glyphs

      for ( s32 i = 0; i < glyphCount; ++i )
      {
        Glyph* glyph = create_glyph( glyphTable, rects[i].codepoint );
        if ( glyph )
        {
          assert( glyph->width == rects[i].size.x );
          assert( glyph->height == rects[i].size.y );

          for ( s32 y = 0; y < rects[i].size.y; ++y )
          {
            u32* writer = sheet->data + rects[i].pos.x + ((rects[i].pos.y + y) * sheet->width);
            for ( s32 x = 0; x < rects[i].size.x; ++x )
            {
              s32 index = x + y * glyph->width;
              *writer++ = color::rgba( glyph->data[index], glyph->data[index], glyph->data[index], glyph->data[index] );
            }
          }

          memory::free( glyph );
        }
      }

      assert( out_rects != nullptr );
      assert( out_rectCount != nullptr );
      *out_rects = rects;
      *out_rectCount = glyphCount;
      return sheet;
    }

    graphics::TextureID create_glyph_sheet( GlyphTable* glyphTable, char const* utf8String, GlyphSheetRect** out_rects, s32* out_rectCount )
    {
      GlyphSheet* sheet = create_glyph_sheet_internal( glyphTable, utf8String, out_rects, out_rectCount );
      graphics::TextureID textureID = platform::allocate_texture( sheet->data, sheet->width, sheet->height );
      memory::free( sheet );
      return textureID;
    }

  };
};


