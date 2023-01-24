DEPRECATED
#pragma once

#include <common/bs_common.h>

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
    struct Info
    {
      u64 size;
    };

    struct Data
    {
      void* data;
      u64   size;
    };
  };
};