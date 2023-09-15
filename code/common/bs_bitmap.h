#pragma once

#include "bsmath.h"

namespace bs
{
  struct Bitmap
  {
    Bitmap():pixel( 0 ), width( 0 ), height( 0 ) {}
    u32* pixel;
    union
    {
      struct
      {
        s32 width;
        s32 height;
      };
      int2 dimensions;
    };
  };
};