#pragma once

#include <common/bsmath.h>

namespace bs
{
  namespace ui { struct TextArea; }

  namespace graphics
  {
    using TextureID = u32;
    using ShaderProgram = u32;

    struct RenderGroup
    {
      enum : u32
      {
        TEXT_AREA,
      } type;
      void* renderObject;
    };

    RenderGroup get_text_area_render_group( ui::TextArea* ta )
    {
      RenderGroup result {};
      result.type = RenderGroup::TEXT_AREA;
      result.renderObject = ta;
      return result;
    }

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
    struct RenderTarget
    {
      void* placeholder;
    };
  };
};