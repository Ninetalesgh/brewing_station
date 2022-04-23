#pragma once

#include <core/bsgraphics.h>
#include <common/bsmath.h>
namespace bs
{
  namespace scene
  {
    struct Object
    {
      float4x4 transform;
      graphics::RenderObject* renderObject;
    };
  };
};
