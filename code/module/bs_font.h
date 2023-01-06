#pragma once

#include <core/bs_texture.h>
#include <common/bscommon.h>

namespace bsm
{
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
    bs::TextureID textureID;
    s32 width;
    s32 height;
  };


  //TODO 
  class GlyphTable
  {
  public:
    ~GlyphTable();

    Glyph* glyphs;
    s32 glyphCount;
    bs::TextureID textureID;
    s32 width;
    s32 height;
  };

  class Font
  {
  public:

    Font( char const* ttfData ) { init( ttfData ); }
    Font( Font&& );
    ~Font();

    bool init( char const* ttfData );

    bool is_valid() { return internal; }
    void set_scale_for_glyph_creation( float scale );



  private:
    Font( Font const& ) = delete;
    struct FontInternal;
    FontInternal* internal = nullptr;
  };


};



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////cpp//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#define STB_TRUETYPE_IMPLEMENTATION
#include "internal/stb_truetype.h"

namespace bsm
{

  GlyphTable::~GlyphTable()
  {
    if ( textureID )
    {
      bsp::platform->free_texture( textureID );
    }
    if ( glyphs )
    {
      bsp::platform->free( glyphs );
    }
  }

  struct Font::FontInternal
  {
    float scale;
    float scaleForPixelHeight;
    stbtt_fontinfo fontInfo;
  };

  bool Font::init( char const* ttfData )
  {
    bool result = false;
    if ( stbtt_GetFontOffsetForIndex( (u8 const*) ttfData, 0 ) == 0 )
    {
      u32 allocSize = sizeof( FontInternal );
      u8* allocation = (u8*) bsp::platform->allocate( allocSize );
      memset( allocation, 0, allocSize );
      internal = (FontInternal*) allocation;

      if ( stbtt_InitFont( &internal->fontInfo, (u8 const*) ttfData, 0 ) )
      {
        set_scale_for_glyph_creation( 64.0f );
        result = true;
      }
      else
      {
        bsp::platform->free( allocation );
        internal = nullptr;
        BREAK;
      }
    }
    else
    {
      BREAK;
    }

    return result;
  }

  Font::~Font()
  {
    if ( internal )
    {
      bsp::platform->free( internal );
    }
  }

  Font::Font( Font&& other )
  {
    internal = other.internal;
    other.internal = nullptr;
  }

  void Font::set_scale_for_glyph_creation( float scale )
  {
    if ( internal )
    {
      internal->scale = scale;
      internal->scaleForPixelHeight = stbtt_ScaleForPixelHeight( &internal->fontInfo, scale );
    }
    else
    {
      BREAK;
    }
  }

};