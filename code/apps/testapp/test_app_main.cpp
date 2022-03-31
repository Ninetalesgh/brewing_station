
#include <ui/bstextarea.h>

#include <core/bsgraphics.h>
#include <core/bsmemory.h>
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
  void app_on_load( PrmAppOnLoad prm )
  {
    char const* text = "dobiieboo\nhello Klara, my email is:\ncamillolukesch@gmail.com\nPlease mail me. <3.";
    testArea = ui::create_text_area_from_text( text, nullptr, { 0,0 } );
    //camera.transform.j3 = float4 { 4,3,3,1 };

    //camera.transform = look_at_matrix( float3 { 0,0, 10 }, float3 { 0,0,0 }, float3 { 0,1,0 } );


    camera.transform = float4x4::identity();
    camera.transform.pos.xyz = float3 { 0,0,20 };
    //camera.transform = rotation_matrix_y( PI );
   // camera.transform.pos = float4 { 0,0, 10,1 };

    //    float4x4 test = look_at_matrix( float3 { 0,0, -10 }, float3 { 0,0,0 }, float3 { 0,1,0 } );
   // camera.view = look_at_matrix( float3 { 0,0, -10 }, float3 { 0,0,0 }, float3 { 0,1,0 } );
    //  view = float4x4::identity();
  }

  void app_tick( PrmAppTick prm )
  {
    graphics::RenderGroup rg = graphics::render_group_from_text_area( testArea );

    graphics::camera_move( &camera, &prm.appData->input );
    platform::render( nullptr, &rg, &camera );
  }

  void app_receive_udp_packet( PrmAppReceiveUDPPacket prm )
  {
    BREAK;
  }
};

