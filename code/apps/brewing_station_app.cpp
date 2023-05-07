#define BREWING_STATION_APP

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
#include <core/bs_allocator.h>
#include <core/bs_font.h>

namespace bsp
{
  void app_on_load_internal( APP_ON_LOAD_PARAMETERS )
  {
    platform = platformCallbacks;

    if ( !platform->default.allocator )
    {
      platform->default.allocator = bs::create_thread_safe_linear_allocator( GigaBytes( 1 ) );
    }

    if ( !platform->default.mainThreadAllocator )
    {
      platform->default.mainThreadAllocator = bs::create_buddy_allocator( GigaBytes( 2 ) );
    }

    if ( !platform->default.fileSystem )
    {
      platform->default.fileSystem = bs::create_filesystem();
      if ( executablePath )
      {
        bs::mount_path_to_filesystem( platform->default.fileSystem, executablePath );
        //TODO this only while debugging
        bs::mount_path_to_filesystem( platform->default.fileSystem, "/../../data" );
      }
    }

    if ( !platform->default.font )
    {
      if ( !platform->default.fileSystem )
      {
        BREAK;
      }
      platform->default.font = bs::create_font_from_ttf_file( "default_font.ttf", platform->default.fileSystem );
    }

    if ( !platform->default.glyphTable )
    {
      if ( !platform->default.font )
      {
        BREAK;
      }
      char const chars[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
      platform->default.glyphTable = bs::create_glyph_table_for_utf8_characters( platform->default.font, chars );
    }

    bs::app_on_load( appData );
  }

  void app_tick_internal( APP_TICK_PARAMETERS )
  {
    bs::app_tick( appData );
  }

  PlatformCallbacks* platform;
};

