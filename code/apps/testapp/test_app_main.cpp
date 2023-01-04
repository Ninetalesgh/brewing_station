
DEPRECATED

#include <ui/bstextarea.h>
#include <scene/bssceneobject.h>

#include <core/bsgraphics.h>
#include <common/bscolor.h>

#include <platform/platform.h>

namespace bs
{
  void app_sample_sound( PrmAppSampleSound prm )
  {
    BREAK;
  }

  static ui::TextArea* testArea;
  static graphics::Camera camera;

  static graphics::RenderObject* renderObject;

  static scene::Object sceneObject;

  void app_on_load( PrmAppOnLoad prm )
  {
    char const* text = "dobiieboo\nhello Klara, my email is:\ncamillolukesch@gmail.com\nPlease mail me. <3.";
    testArea = ui::create_text_area_from_text( text, nullptr, { 0,0 } );

    camera.transform = float4x4::identity();
    camera.transform.pos.xyz = float3 { 0,0,20 };


    static float3 vertices[] = {
          float3{-1.0f,-1.0f, -1.0f},
          float3{ 1.0f,-1.0f, -1.0f},
          float3{ 1.0f, 1.0f, -1.0f},
          float3{-1.0f, 1.0f, -1.0f},
    };

    static  float2 uvs[] = {
    float2{0.0f, 1.0f},
    float2{1.0f, 1.0f},
    float2{1.0f, 0.0f},
    float2{0.0f, 0.0f},
    };

    static  u16 indices[] =
    {
      0,1,2,
      0,2,3
    };

    graphics::MeshData raw {};
    raw.vertices = vertices;
    raw.vertexCount = array_count( vertices );
    raw.indices = indices;
    raw.indexCount = array_count( indices );
    raw.uvs = uvs;
    raw.indexFormat = graphics::IndexFormat::U16;

    //  renderObject = graphics::create_render_object( &raw, nullptr, nullptr );

    sceneObject.transform = float4x4::identity();
    sceneObject.renderObject = renderObject;

  }

  void app_tick( PrmAppTick prm )
  {
    static s32 firstTime = 0;
    if ( !firstTime-- )
    {
      static float3 vertices[] = {
      float3{-1.0f,-1.0f, -1.0f},
      float3{ 1.0f,-1.0f, -1.0f},
      float3{ 1.0f, 1.0f, -1.0f},
      float3{-1.0f, 1.0f, -1.0f},
      };

      static  float2 uvs[] = {
      float2{0.0f, 1.0f},
      float2{1.0f, 1.0f},
      float2{1.0f, 0.0f},
      float2{0.0f, 0.0f},
      };

      static  u16 indices[] =
      {
        0,1,2,
        0,2,3
      };

      graphics::MeshData raw {};
      raw.vertices = vertices;
      raw.vertexCount = array_count( vertices );
      raw.indices = indices;
      raw.indexCount = array_count( indices );
      raw.uvs = uvs;
      raw.indexFormat = graphics::IndexFormat::U16;
      renderObject = graphics::create_render_object( &raw, nullptr, nullptr );

      sceneObject.transform = float4x4::identity();
      sceneObject.renderObject = renderObject;
    }

    graphics::RenderGroup rg = graphics::render_group_from_text_area( testArea );

    graphics::RenderGroup rg2 = graphics::render_group_from_scene_objects( &sceneObject, 1 );

    static float modelposy = 0.0f;
    modelposy += 0.02f;
    sceneObject.transform.pos = float4 { 2.0f * sinf( modelposy ), 0, 0, 1 };


    graphics::camera_move( &camera, &prm.appData->input );
    platform::render( nullptr, &rg, &camera );
    //platform::render( nullptr, &rg2, &camera );
  }

  void app_receive_udp_packet( PrmAppReceiveUDPPacket prm )
  {
    BREAK;
  }
};

