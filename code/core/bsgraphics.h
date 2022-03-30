#pragma once

#include <common/bsmath.h>

namespace bs
{
  namespace ui { struct TextArea; }

  namespace graphics
  {
    using TextureID = u32;
    using ShaderProgram = u32;
    struct Bitmap;

    struct RenderGroup
    {
      enum : u32
      {
        TEXT_AREA,
        CUSTOM_BITMAP
      } type;
      void* renderObject;
    };

    RenderGroup render_group_from_text_area( ui::TextArea* ta );
    RenderGroup render_group_from_custom_bitmap( Bitmap* bmp );

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

    RenderGroup render_group_from_text_area( ui::TextArea* ta )
    {
      RenderGroup result {};
      result.type = RenderGroup::TEXT_AREA;
      result.renderObject = ta;
      return result;
    }

    RenderGroup render_group_from_custom_bitmap( Bitmap* bmp )
    {
      RenderGroup result {};
      result.type = RenderGroup::CUSTOM_BITMAP;
      result.renderObject = bmp;
      return result;
    }

  };
};