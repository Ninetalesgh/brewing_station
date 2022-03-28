#pragma once

#include <common/bsmath.h>

namespace bs
{
  namespace graphics
  {
    using TextureID = u32;
    struct RenderGroup;
    struct RenderTarget;

    struct Rect
    {
      float2 pos;
      float2 size;
    };

    struct Camera
    {
      float3 pos;
    };

    struct Bitmap
    {
      u32* pixel;
      s32 width;
      s32 height;
    };
  };
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////inl//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

namespace bs
{
  namespace graphics
  {
    struct RenderGroup
    {
      Rect objects[4];
      TextureID textures[4];
    };

    struct RenderTarget
    {
      Rect viewport;
    };
  };
};