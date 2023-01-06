
#include <platform/bs_platform.h>

#include "klein.hpp"

#include <module/bs_font.h>

namespace bs
{

  void app_on_load( bsp::AppData* appData )
  {
    BREAK;
  }

  void app_tick( bsp::AppData* appData )
  {
    bsm::Font test( "" );

    bsm::Font test2 = bs::move( test );
    // BREAK;
  }

};
