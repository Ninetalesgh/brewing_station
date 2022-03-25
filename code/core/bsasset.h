#pragma once

#include "bsfile.h"

#include <common/bscommon.h>
//things:

//asset id 

//asset in file IO

//asset loaded in game

using TextureID = s32;
namespace bs
{
  struct Texture
  {
    TextureID vramID;
    u32* pixel;
    s32 width;
    s32 height;
  };
};


namespace bs
{
  namespace font
  {
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
  }

  namespace asset
  {
    struct RawTTF;

    struct Font
    {
      RawTTF* raw;
      font::GlyphTable* glyphTable;
    };

  };
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////cpp//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include <platform/platform_callbacks.h>

  // u32 get_file_info( char const* filename, bs::FileInfo* out_FileInfo );
  // bs::ReadFileResult read_file( char const* filename, u32 maxSize, void* out_data );
  // void free_file( void* filename );
  // u32 write_file( bs::WriteFileParameter const& );

//#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "internal/stb_truetype.h"

namespace bs
{
  namespace asset
  {
    struct RawTTF
    {

    };

    RawTTF* load_from_file( char const* path )
    {
      return nullptr;
    }

    font::GlyphTable* create_glyph_table_from_ttf( RawTTF const* ttf )
    {
      return nullptr;
    }

  };
};