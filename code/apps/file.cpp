#include "file.h"



FILE_TYPE file_get_extension( char const* filename )
{
  s32 length = string_length( filename );
  s32 last = string_last( filename, length, '.' );

  if ( string_match( filename + last, ".bmp" ) )
  {
    return BMP;
  }
  else if ( string_match( filename + last, ".jpg" ) )
  {
    return JPG;
  }
  else if ( string_match( filename + last, ".jpeg" ) )
  {
    return JPG;
  }
  else if ( string_match( filename + last, ".ttf" ) )
  {
    return TTF;
  }
  else
  {
    return ERROR;
  }
}

Bitmap file_load_bmp( platform::read_file* read_file, char const* filename )
{
  Bitmap result = {};
  platform::ReadFileResult file = read_file( filename, 0, nullptr );

  BitmapHeader& header = *(BitmapHeader*) file.data;

  result.pixel = (u32*) ((u8*) file.data + header.bitmapOffset);
  result.width = header.width;
  result.height = header.height;

  return result;
}

void file_save_bmp( platform::write_file* write_file, char const* filename, Bitmap const& bitmap )
{
  BitmapHeader header {};
  header.fileType = 19778;
  header.reserved1 = 0;
  header.reserver2 = 0;
  header.bitmapOffset = sizeof( BitmapHeader );
  header.dibHeaderSize = sizeof( BitmapHeader ) - 14;
  header.width = bitmap.width;
  header.height = bitmap.height;
  header.planes = 1;
  header.bitsPerPixel = 32;
  header.compression = 0;
  header.sizeofBitmap = (u32) (bitmap.width * bitmap.height * sizeof( *bitmap.pixel ));
  header.horzResolution = 0;
  header.vertResolution = 0;
  header.colorsUsed = 0;
  header.colorsImportant = 0;

  header.fileSize = header.bitmapOffset + header.sizeofBitmap;

  void* data[]
  {
   (void*) &header,
   (void*) bitmap.pixel
  };

  s32 size[]
  {
   (s32) sizeof( BitmapHeader ),
   (s32) header.sizeofBitmap
  };

  s32 count = array_size( data );
  write_file( filename, data, size, count );
}


#define STB_IMAGE_IMPLEMENTATION
#include "extern/stb_image.h"

Bitmap load_image( u8 const* buffer, s32 size )
{
  //sizeof( stbi__jpeg )
  constexpr s32 bytesPerChannel = 4;

  Bitmap result = {};
  s32 channelsPerSample;
  result.pixel = (u32*) stbi_load_from_memory( buffer, size, &result.width, &result.height, &channelsPerSample, bytesPerChannel );

  if ( result.pixel )
  {
    u32* writer = result.pixel;
    for ( s32 x = 0; x < result.width; ++x )
    {
      for ( s32 y = 0; y < result.height;++y )
      {
        *writer = color_RGBAtoBGRA( *writer );
        ++writer;
      }
    }
  }
  return result;
}

#define STB_TRUETYPE_IMPLEMENTATION
#include "extern/stb_truetype.h"


Bitmap8 load_font( memory::Arena arena, u8 const* ttf_data )
{
  stb_truetype::init_memory_arena( &arena );

  u64 profile_fontData;
  u64 profile_codepoint;
  stbtt_fontinfo font;
  {
    PROFILE_SCOPE( profile_fontData );
    stbtt_InitFont( &font, ttf_data, stbtt_GetFontOffsetForIndex( ttf_data, 0 ) );
  }
  float s = 128.0f;
  s32 codepoint = 'h';
  Bitmap8 bmp {};
  {
    PROFILE_SCOPE( profile_codepoint );
    bmp.pixel = stbtt_GetCodepointBitmap( &font, 0, stbtt_ScaleForPixelHeight( &font, s ), codepoint, &bmp.width, &bmp.height, 0, 0 );
  }
  stb_truetype::deinit_memory_arena();

  return bmp;
}