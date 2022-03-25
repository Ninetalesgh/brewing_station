#pragma once

#include <common/bscommon.h>

namespace bs
{
  struct WriteFileParameter
  {
    char const* filename;
    void* data;
    s32 size;
    s32 count;
  };

  namespace file
  {
    struct LoadedFile
    {
      void* data;
      u32   size;
    };

    //LoadedFile load_into_memory( char const* path );

  };
};