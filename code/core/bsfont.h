#pragma once

#include "bsfile.h"

#include <core/bsmemory.h>
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

    //returns nullptr on fail
    GlyphTable* create_glyph_table_from_ttf( void const* data );

    //set scale for create_glyph() calls
    void set_scale_for_glyph_creation( GlyphTable* glyphTable, float scale );

    //returns nullptr if the table doesn't have the glyph to the requested codepoint
    //returns a rasterized glyph at the scale previously set with set_scale_for_glyph_creation()
    Glyph* create_glyph( GlyphTable* glyphTable, s32 unicodeCodepoint );

    //returns the next character in the string after parsing the codepoint
    char const* parse_utf8( char const* utf8String, s32* out_codepoint );

    s32 get_unicode_codepoint( char const* utf8String, s32* out_extraBytes = nullptr );
  }
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////cpp//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


#include <platform/platform_callbacks.h>

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
        glyphTable->scale = scale;
        glyphTable->scaleForPixelHeight = stbtt_ScaleForPixelHeight( (stbtt_fontinfo*) glyphTable->fontInfo, scale );
      }
      else
      {
        BREAK;
      }
    }

    Glyph* create_glyph( GlyphTable* glyphTable, s32 unicodeCodepoint )
    {
      stbtt_fontinfo* fontInfo = glyphTable->fontInfo;
      float scale = glyphTable->scaleForPixelHeight;

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

    s32 get_unicode_codepoint( char const* utf8String, s32* out_extraBytes /*= nullptr*/ )
    {
      s32 result = 0;
      u8 const* reader = (u8 const*) utf8String;
      u8 const extraByteCheckMask = 0b10000000;
      u8 const extraByteValueMask = 0b00111111;

      u8 unicodeMask = 0b11000000;

      s32 extraBytes = 0;
      result = *(u8*) (reader);
      while ( *utf8String & unicodeMask )
      {
        unicodeMask >>= 1;
        ++reader;
        if ( (*reader & ~extraByteValueMask) == extraByteCheckMask )
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
      else
      {
        //TODO maybe check whether it's a valid 1 byte character? aka: < 128
      }

      if ( out_extraBytes ) *out_extraBytes = extraBytes;
      return result;
    }

    char const* parse_utf8( char const* utf8String, s32* out_codepoint )
    {
      char const* nextChar = nullptr;
      if ( *utf8String != '\0' )
      {
        s32 extraBytes = 0;
        *out_codepoint = get_unicode_codepoint( utf8String, &extraBytes );
        nextChar = utf8String + 1 + extraBytes;
      }
      return nextChar;
    }
  };
};


