
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

  void app_on_load( PrmAppOnLoad prm )
  {
    char const* text = "dobiieboo\nhello Klara, my email is:\ncamillolukesch@gmail.com\nPlease mail me. <3.";
    testArea = ui::create_text_area_from_text( text, nullptr, { 0,0 } );
  }

  void app_tick( PrmAppTick prm )
  {
    graphics::RenderGroup rg = graphics::render_group_from_text_area( testArea );

    platform::render( nullptr, &rg, nullptr );
  }

  void app_receive_udp_packet( PrmAppReceiveUDPPacket prm )
  {
    BREAK;
  }
};

