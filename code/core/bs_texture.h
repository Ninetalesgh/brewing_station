#pragma once

#include <common/bscommon.h>

namespace bs
{
  using TextureID = u32;
  enum class TextureFormat: u32
  {
    INVALID = 0,
    RGBA8 = 1,
  };
  struct TextureData
  {
    void* pixel;
    TextureFormat format;
    s32 width;
    s32 height;
  };
};