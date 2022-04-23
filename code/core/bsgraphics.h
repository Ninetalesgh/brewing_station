#pragma once

#include <common/bsmath.h>
#include <common/bsmatrix.h>

namespace bs
{
  namespace input { struct State; }
  namespace ui { struct TextArea; }
  namespace scene { struct Object; }

  namespace graphics
  {
    using TextureID = u32;
    using VertexBufferID = u32;
    using UVBufferID = u32;
    using IndexBufferID = u32;
    using ShaderProgram = u32;
    using RenderObjectID = u32;

    struct Bitmap;

    enum class IndexFormat : u32
    {
      INVALID = 0,
      U16 = 1,
      U32 = 2,
    };

    struct MeshData
    {
      float3* vertices;
      float2* uvs;
      void* indices;
      u32 vertexCount;
      u32 indexCount;
      IndexFormat indexFormat;
    };

    struct Mesh
    {
      VertexBufferID vb;
      UVBufferID uvb;
      IndexBufferID ib;
      u32 indexCount;
      IndexFormat indexFormat;
    };

    enum class TextureFormat : u32
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

    struct RenderObject
    {
      // enum : u16
      // {
      //   SCENE_OBJECT = 1,
      // } type;
      // enum : u8
      // {
      //   NONE = 0,
      // } subtype;

      // u32 flags;
      RenderObjectID id;
      TextureID diffuseTexture;
      TextureID normalMap;
      Mesh mesh;
    };

    struct RenderGroup
    {
      enum : u32
      {
        SCENE_OBJECTS,
        TEXT_AREA,
        CUSTOM_BITMAP,
      } type;
      void* renderObject;
      u32 count;
    };

    RenderObject* create_render_object( MeshData const* meshData, TextureData const* diffuseTextureData, TextureData const* normalMapData );

    RenderGroup render_group_from_text_area( ui::TextArea* ta );
    RenderGroup render_group_from_custom_bitmap( Bitmap* bmp );

    RenderGroup render_group_from_scene_objects( scene::Object* objects, u32 objectCount );


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

    RenderGroup render_group_from_scene_objects( scene::Object* objects, u32 objectCount )
    {
      RenderGroup result {};
      result.type = RenderGroup::SCENE_OBJECTS;
      result.renderObject = objects;
      result.count = objectCount;
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
        float factor = 0.005f;
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