//#define BUILD_TESTAPP
//#define BUILD_FIRST_APP
#define BUILD_BSANIM_APP


#if defined(BUILD_BSANIM_APP)
#include "bsanim_app/bsanim_app_main.cpp"
#endif


//#include <core/internal/bsfile.cpp>
//#include <core/internal/bstask.cpp>
//#include <core/internal/bsnet.cpp>
//#include <core/internal/bsgraphics.cpp>

#include <platform/bs_platform.h>

namespace bsp
{
  void app_on_load_internal( APP_ON_LOAD_PARAMETERS )
  {
    platform = platformData;

    bs::app_on_load( appData );
  }

  void app_tick_internal( APP_TICK_PARAMETERS )
  {
    bs::app_tick( appData );
  }

  PlatformCallbacks* platform;
};

