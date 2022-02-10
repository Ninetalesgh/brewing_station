#pragma once

#include <common/bscommon.h>

namespace bs
{
  namespace platform
  {
    /*
      returns 0 if no file was found
      returns 1 if file was found and fileinfo was successfully written to
    */
    struct FileInfo
    {
      u64 size;
    };
    using get_file_info = u32( char const* filename, FileInfo* out_FileInfo );

    /*
      TODO NOT HAVE READ FILE ALLOCATE ITS OWN MEMORY
    */
    struct ReadFileResult
    {
      void* data;
      u32 size;
    };
    using read_file = ReadFileResult( char const* filename, u32 maxSize, void* out_data );

    /*
      TODO THIS IS ONLY NECESSARY WITH READFILE ALLOCATING ITS OWN MEMORY
    */
    using free_file = void( void* filename );

    struct WriteFileParameter
    {
      char const* filename;
      void* data;
      s32 size;
      s32 count;
    };
    using write_file = u32( WriteFileParameter const& );
  };
};