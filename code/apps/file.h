#pragma once

#include <platform.h>
#include <common/basic_types.h>
#include <common/string.h>
#include <common/memory.h>

#pragma pack(push, 1)
struct BitmapHeader
{
  u16 fileType;
  u32 fileSize;
  u16 reserved1;
  u16 reserver2;
  u32 bitmapOffset;
  u32 dibHeaderSize;
  s32 width;
  s32 height;
  u16 planes;
  u16 bitsPerPixel;
  u32 compression;
  u32 sizeofBitmap;
  s32 horzResolution;
  s32 vertResolution;
  u32 colorsUsed;
  u32 colorsImportant;
};
#pragma pack(pop)


struct Font
{
  u8* bitmap;
};

enum FILE_TYPE
{
  ERROR = 0x0,
  BMP,
  JPG,
  TTF,
};

FILE_TYPE file_get_extension( char const* filename );
Bitmap file_load_bmp( platform::read_file* read_file, char const* filename );

void file_save_bmp( platform::write_file* write_file, char const* filename, Bitmap const& bitmap );

Bitmap load_image( u8 const* buffer, s32 size );

