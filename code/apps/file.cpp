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

  //u64 profile_fontData;
  //u64 profile_codepoint;
  stbtt_fontinfo fontInfo;
  {
    //  PROFILE_SCOPE( profile_fontData );
    stbtt_InitFont( &fontInfo, ttf_data, stbtt_GetFontOffsetForIndex( ttf_data, 0 ) );
  }

  float scale = 128.0f;
  s32 codepoint = 'A';
  Bitmap8 bmp {};

  {
    // PROFILE_SCOPE( profile_codepoint );
    bmp.pixel = stbtt_GetCodepointBitmap( &fontInfo, 0, stbtt_ScaleForPixelHeight( &fontInfo, scale ), codepoint, &bmp.width, &bmp.height, 0, 0 );
  }

  // float xpos = 2;
  // int advance, lsb, x0, y0, x1, y1;
  // float x_shift = xpos - (float) floor( xpos );
  // stbtt_GetCodepointHMetrics( &fontInfo, codepoint, &advance, &lsb );
  // stbtt_GetCodepointBitmapBoxSubpixel( &fontInfo, codepoint, scale, scale, x_shift, 0, &x0, &y0, &x1, &y1 );
   //stbtt_MakeCodepointBitmapSubpixel( &fontInfo, &screen[baseline + y0][(int) xpos + x0], x1 - x0, y1 - y0, 79, scale, scale, x_shift, 0, codepoint );


 //  int glyphIndex = stbtt_FindGlyphIndex( fontInfo, codepoint );
  // bmp.pixel = stbtt_GetGlyphBitmapSubpixel( fontInfo, scale_x, scale_y, shift_x, shift_y, glyphIndex, width, height, xoff, yoff );



  stb_truetype::deinit_memory_arena();
  return bmp;
}