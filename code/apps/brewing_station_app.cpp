//#define BUILD_TESTAPP
//#define BUILD_FIRST_APP

#define BUILD_BSANIM_APP
//#define BUILD_ASSETPRECOMPILER_APP


#if defined(BUILD_BSANIM_APP)
#include "bsanim_app/bsanim_app_main.cpp"
#elif defined(BUILD_ASSETPRECOMPILER_APP)
#include "assetprecompiler/assetprecompiler_main.cpp"
#endif


//#include <core/internal/bsfile.cpp>
//#include <core/internal/bstask.cpp>
//#include <core/internal/bsnet.cpp>
//#include <core/internal/bsgraphics.cpp>
#include <platform/bs_platform.h>
#include <module/bs_allocator.h>
#include <module/bs_font.h>

namespace bsp
{
  void app_on_load_internal( APP_ON_LOAD_PARAMETERS )
  {
    platform = platformCallbacks;

    if ( !platform->default.allocator )
    {
      platform->default.allocator = bsm::create_slow_thread_safe_allocator( GigaBytes( 4 ) );
    }

    if ( !platform->default.fileSystem )
    {
      platform->default.fileSystem = bsm::create_filesystem();
      if ( executablePath )
      {
        bsm::mount_path_to_filesystem( platform->default.fileSystem, executablePath );
        //TODO this only while debugging
        bsm::mount_path_to_filesystem( platform->default.fileSystem, "/../../data" );
      }
    }

    if ( !platform->default.font )
    {
      if ( !platform->default.fileSystem )
      {
        BREAK;
      }
      platform->default.font = bsm::create_font_from_ttf_file( "default_font.ttf", platform->default.fileSystem );
    }

    if ( !platform->default.glyphTable )
    {
      if ( !platform->default.font )
      {
        BREAK;
      }
      char const chars[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
      platform->default.glyphTable = bsm::create_glyph_table_for_utf8_characters( platform->default.font, chars );
    }

    bs::app_on_load( appData );
  }

  void app_tick_internal( APP_TICK_PARAMETERS )
  {
    bs::app_tick( appData );
  }

  PlatformCallbacks* platform;
};

