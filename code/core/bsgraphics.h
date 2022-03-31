#pragma once

#include <common/bsmath.h>
#include <common/bsmatrix.h>

namespace bs
{
  namespace input { struct State; }
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
      float4x4 transform;
      float yaw;
      float pitch;
    };

    void camera_init( Camera* camera, float viewportWidth, float viewportHeight, float fovRadians, float zNear, float zFar );
    void camera_move( Camera* camera, input::State* inputState );

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

#include <core/bsinput.h>
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

    void camera_init( Camera* camera, float fovRadians )
    {
      camera->transform = float4x4::identity();
      camera->yaw = 0.0f;
      camera->pitch = 0.0f;
    }

    float4x4 get_camera_view_projection_matrix( Camera* camera, float viewportWidth, float viewportHeight, float fovRadians, float zNear, float zFar )
    {
      float tanHalfFovx = 1.0f / tanf( fovRadians * 0.5f );
      float aspectRatio = viewportWidth / viewportHeight;
      float z = 1.0f / (zFar - zNear);
      float4x4 m = float4x4::identity();
      m.m00 = tanHalfFovx;
      m.m11 = aspectRatio * tanHalfFovx;
      m.m22 = -zFar * z;
      m.m23 = -1.0f;
      m.m32 = -(zFar * zNear) * z;
      m.m33 = 0.0f;

      return matrix_inverse_orthonormal( camera->transform ) * m;
    }

    void camera_move( Camera* camera, input::State* inputState )
    {
      input::State& input = *inputState;
      float2 mouseDelta = float2 { input.mousePos[0].end - input.mousePos[0].start };
      if ( input.held[input::KEY_CTRL] )
      {
        float factor = 0.008f;
        camera->yaw -= factor * mouseDelta.x;
        camera->pitch -= factor * mouseDelta.y;
      }

      float4x4& transform = camera->transform;
      float4 pos = transform.pos;
      transform = matrix_from_euler( camera->yaw, camera->pitch );

      float4 const& xAxis = transform.xAxis;
      float4 const& yAxis = transform.yAxis;
      float4 const& zAxis = transform.zAxis;

      float4 move = float4 { 0.0f };
      float speed = .5f;

      if ( input.held[input::KEY_A] )
      {
        move -= xAxis * speed;
      }
      if ( input.held[input::KEY_D] )
      {
        move += xAxis * speed;
      }

      if ( input.held[input::KEY_W] )
      {
        move -= zAxis * speed;
      }
      if ( input.held[input::KEY_S] )
      {
        move += zAxis * speed;
      }

      if ( input.held[input::KEY_Q] )
      {
        move -= yAxis * speed;
      }
      if ( input.held[input::KEY_E] )
      {
        move += yAxis * speed;
      }
      transform.pos = pos + move;
    }
  };
};