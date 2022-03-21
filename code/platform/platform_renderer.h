#pragma once

#include <common/bscommon.h>

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