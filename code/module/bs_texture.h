#pragma once

#include <common/bsmath.h>
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

  using VertexBufferID  = u32;
  using UVBufferID      = u32;
  using IndexBufferID   = u32;
  using ShaderProgramID = u32;
  using MeshID          = u32;

  enum class IndexFormat: u32
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
    VertexBufferID vertexBuffer;
    UVBufferID uvBuffer;
    IndexBufferID indexBuffer;
    u32 indexCount;
    IndexFormat indexFormat;
    MeshID id;
  };

};