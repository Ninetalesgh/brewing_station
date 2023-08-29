#pragma once

#include "brewing_station_core.h"
#include "core/bs_allocator.h"
#include "core/bs_filesystem.h"
#include "common/bs_color.h"
#include "common/bs_bitmap.h"

namespace bs
{
  [[nodiscard]]
  Bitmap load_image( char const* path );

  void unload_image( Bitmap& );
};


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

#define STB_IMAGE_IMPLEMENTATION

namespace stb_image
{
  void* bs_realloc( void* data, size_t oldSize, size_t newSize )
  {
    void* newData = ALLOCATE( newSize );
    if ( data )
    {
      memcpy( newData, data, oldSize );
      FREE( data );
    }
    return newData;
  }
}

#define STBI_MALLOC(sz)           ALLOCATE(sz)
#define STBI_REALLOC_SIZED(p,oldsz,newsz)     stb_image::bs_realloc(p,oldsz,newsz)
#define STBI_FREE(p)              FREE(p)

#include "core/internal/stb_image.h"

namespace bs
{
  Bitmap load_image( char const* path )
  {
    File* imageFile = load_file( path );

    Bitmap result = {};
    s32 channelsPerSample;
    constexpr s32 bytesPerChannel = 4;
    assert( imageFile->size <= INT_MAX );
    result.pixel = (u32*) stbi_load_from_memory( (u8*) imageFile->data, s32( imageFile->size ), &result.width, &result.height, &channelsPerSample, bytesPerChannel );

    unload_file( imageFile );
    // if ( result.pixel )
    // {
    //   u32* writer = result.pixel;
    //   for ( s32 x = 0; x < result.width; ++x )
    //   {
    //     for ( s32 y = 0; y < result.height;++y )
    //     {
    //       *writer = color::RGBAtoBGRA( *writer );
    //       ++writer;
    //     }
    //   }
    // }

    return result;
  }

  void unload_image( Bitmap& bmp )
  {
    STBI_FREE( bmp.pixel );
  }
};