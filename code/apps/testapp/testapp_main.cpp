
#include <ui/bstextarea.h>

#include <core/bsgraphics.h>
#include <core/bsmemory.h>

#include <platform/platform.h>
namespace bs
{
  void app_sample_sound( PrmAppSampleSound prm )
  {
    BREAK;
  }

  static ui::TextArea* testArea;

  void app_on_load( PrmAppOnLoad prm )
  {

    //char const* text = "hello\ntest";
    char const* text = "hello klara";
    testArea = ui::create_text_area_from_text( text, nullptr, { 0,0 } );


  }

  void app_tick( PrmAppTick prm )
  {
    graphics::RenderGroup rg = graphics::get_text_area_render_group( testArea );

    platform::render( nullptr, &rg, nullptr );

    // BREAK;

  }
  void app_render( PrmAppRender prm )
  {
    BREAK;
  }
  void app_receive_udp_packet( PrmAppReceiveUDPPacket prm )
  {
    BREAK;
  }
};

