#pragma once

#include <core/bs_texture.h>
#include <common/bscommon.h>

namespace bsm
{
  struct Font;

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

  struct GlyphTable
  {
    Glyph* glyphs;
    s32 glyphCount;
    bs::TextureID textureID;
    s32 width;
    s32 height;
  };

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



  [[nodiscard]]
  /// @brief 
  /// @param ttfPath .TTF asset
  /// @param fs optional custom filesystem
  /// @return 
  Font* create_font_from_ttf_file( char const* ttfPath, FileSystem* fs = nullptr );

  void destroy_font( Font* );
  void set_font_scale( Font*, float scale );

  [[nodiscard]]
  GlyphTable* create_glyph_table_for_utf8_characters( Font*, char const* utf8String );

  void destroy_glyph_table( GlyphTable* );

  Glyph* get_glyph_for_codepoint( GlyphTable*, s32 codepoint );

  [[nodiscard]]
  RawGlyphData* create_raw_glyph_data( Font*, s32 unicodeCodepoint );
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////cpp//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include <platform/bs_platform.h>
#include <module/bs_filesystem.h>
#include <common/bscolor.h>
#include <common/bs_bitmap.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "internal/stb_truetype.h"

namespace bsm
{
  struct Font
  {
    float scale;
    float scaleForPixelHeight;
    FileSystem* fileSystem;
    stbtt_fontinfo fontInfo;
  };
  Glyph get_glyph_dimensions( Font* font, s32 unicodeCodepoint );
  Font* create_font_from_ttf_data( void* ttfData );

  Font* create_font_from_ttf_file( char const* ttfPath, FileSystem* fs )
  {
    if ( !fs ) fs = bsp::platform->default.fileSystem;

    File* ttfFile = load_file( fs, ttfPath );
    Font* font = nullptr;
    if ( ttfFile )
    {
      font = create_font_from_ttf_data( ttfFile->data );
      if ( font )
      {
        set_font_scale( font, 64.0f );
      }
      else
      {
        bsm::free( bsp::platform->default.allocator, ttfFile->data );
        font = nullptr;
        BREAK;
      }
    }
    return font;
  }

  Font* create_font_from_ttf_data( void* ttfData )
  {
    Font* font = nullptr;
    if ( stbtt_GetFontOffsetForIndex( (u8 const*) ttfData, 0 ) == 0 )
    {
      u32 allocSize = sizeof( Font );
      u8* allocation = (u8*) bsm::allocate( bsp::platform->default.allocator, allocSize );
      memset( allocation, 0, allocSize );
      font = (Font*) allocation;

      if ( !stbtt_InitFont( &font->fontInfo, (u8 const*) ttfData, 0 ) )
      {
        bsm::free( bsp::platform->default.allocator, allocation );
        font = nullptr;
        BREAK;
      }
    }
    else
    {
      BREAK;
    }

    return font;
  }

  void destroy_font( Font* font )
  {
    if ( font )
    {
      if ( font->fontInfo.data )
      {
        bsm::free( bsp::platform->default.allocator, font->fontInfo.data );
      }

      bsm::free( bsp::platform->default.allocator, font );
    }
  }

  void set_font_scale( Font* font, float scale )
  {
    font->scale = scale;
    font->scaleForPixelHeight = stbtt_ScaleForPixelHeight( &font->fontInfo, scale );
  }

  GlyphTable* create_glyph_table_for_utf8_characters( Font* font, char const* utf8String )
  {
    char const* reader = utf8String;
    s32 glyphCount = bs::string_length_utf8( utf8String );

    Glyph* rects = nullptr;
    GlyphTable* resultGlyphTable = nullptr;
    {
      s32 allocSize = sizeof( GlyphTable ) + sizeof( Glyph ) * glyphCount;
      u8* allocation = (u8*) bsm::allocate( bsp::platform->default.allocator, allocSize );
      memset( allocation, 0, allocSize );
      resultGlyphTable = (GlyphTable*) allocation;
      rects = (Glyph*) (allocation + sizeof( GlyphTable ));
    }

    int2 sheetDims = 0;
    int2 currentRow = 0;
    constexpr s32 MAX_WIDTH = 512;

    s32 rectsIndex = 0;
    while ( *reader )
    {
      s32 codepoint;
      reader = bs::string_parse_utf8( reader, &codepoint );

      Glyph newGlyph = get_glyph_dimensions( font, codepoint );
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
    bs::Bitmap* sheetBMP = nullptr;
    {
      u8* allocation = (u8*) bsm::allocate( bsp::platform->default.allocator, sizeof( bs::Bitmap ) + sizeof( u32 ) * sheetDims.x * sheetDims.y );
      sheetBMP = (bs::Bitmap*) allocation;
      sheetBMP->width = sheetDims.x;
      sheetBMP->height = sheetDims.y;
      sheetBMP->pixel = (u32*) (allocation + sizeof( bs::Bitmap ));
    }

    //rasterize glyphs
    for ( s32 i = 0; i < glyphCount; ++i )
    {
      RawGlyphData* rawGlyphData = create_raw_glyph_data( font, rects[i].codepoint );
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

        bsm::free( bsp::platform->default.allocator, rawGlyphData );
      }
    }

    bs::TextureData texData {};
    texData.pixel = sheetBMP->pixel;
    texData.width = sheetBMP->width;
    texData.height = sheetBMP->height;
    texData.format = bs::TextureFormat::RGBA8;

    resultGlyphTable->glyphs =     rects;
    resultGlyphTable->glyphCount = glyphCount;
    resultGlyphTable->textureID =  bsp::platform->allocate_texture( &texData );
    resultGlyphTable->width =      sheetBMP->width;
    resultGlyphTable->height =     sheetBMP->height;

    bsm::free( bsp::platform->default.allocator, sheetBMP );

    return resultGlyphTable;

  }

  void destroy_glyph_table( GlyphTable* table )
  {
    if ( table )
    {
      if ( table->textureID )
      {
        bsp::platform->free_texture( table->textureID );
      }
      if ( table->glyphs )
      {
        bsm::free( bsp::platform->default.allocator, table->glyphs );
      }
    }
  }

  Glyph* get_glyph_for_codepoint( GlyphTable* table, s32 codepoint )
  {
    Glyph* glyph = table->glyphs;
    Glyph* end = glyph + table->glyphCount;
    //TODO optimize this
    while ( glyph->codepoint != codepoint && glyph != end ) { ++glyph; }

    return glyph == end ? nullptr : glyph;
  }

  RawGlyphData* create_raw_glyph_data( Font* font, s32 unicodeCodepoint )
  {
    stbtt_fontinfo* fontInfo = &font->fontInfo;
    float scale = font->scaleForPixelHeight;

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
          u8* allocation = (u8*) bsm::allocate( bsp::platform->default.allocator, sizeof( RawGlyphData ) + gbm.w * gbm.h );
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
            bsm::free( bsp::platform->default.allocator, vertices );
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

  Glyph get_glyph_dimensions( Font* font, s32 unicodeCodepoint )
  {
    stbtt_fontinfo* fontInfo = &font->fontInfo;
    float scale = font->scaleForPixelHeight;

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

};