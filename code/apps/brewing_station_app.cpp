//#define BUILD_TESTAPP
//#define BUILD_FIRST_APP

//#define BUILD_BSANIM_APP
#define BUILD_ASSETCOMPILER_APP


#if defined(BUILD_BSANIM_APP)
#include "bsanim_app/bsanim_app_main.cpp"
#elif defined(BUILD_ASSETCOMPILER_APP)
#include "assetcompiler/assetcompiler_main.cpp"
#endif


//#include <core/internal/bsfile.cpp>
//#include <core/internal/bstask.cpp>
//#include <core/internal/bsnet.cpp>
//#include <core/internal/bsgraphics.cpp>
#include <platform/bs_platform.h>
#include <module/bs_font.h>

namespace bsp
{
  void app_on_load_internal( APP_ON_LOAD_PARAMETERS )
  {
    platform = platformCallbacks;

    if ( !platform->defaultFileSystem )
    {
      platform->defaultFileSystem = bsm::create_filesystem();
      if ( executablePath )
      {
        bsm::mount_path_to_filesystem( platform->defaultFileSystem, executablePath );
        bsm::mount_path_to_filesystem( platform->defaultFileSystem, "/../../data" );
      }
    }

    if ( platform->defaultFileSystem && !platform->defaultFont )
    {
      platform->defaultFont = bsm::create_font_from_ttf_file( "bs.ttf", platform->defaultFileSystem );
    }

    if ( platform->defaultFont && !platform->defaultGlyphTable )
    {
      char const chars[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
      platform->defaultGlyphTable = bsm::create_glyph_table_for_utf8_characters( platform->defaultFont, chars );
    }

    bs::app_on_load( appData );
  }

  void app_tick_internal( APP_TICK_PARAMETERS )
  {
    bs::app_tick( appData );
  }

  PlatformCallbacks* platform;
};

