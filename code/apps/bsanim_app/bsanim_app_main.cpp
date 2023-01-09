
#include <platform/bs_platform.h>

#include "klein.hpp"

#include <module/bs_font.h>

namespace bs
{

  void app_on_load( bsp::AppData* appData )
  {
    // bsm::Font* test = bsm::create_font_from_ttf_file( "../../data/bs.ttf" );
    // test = test;
   // BREAK;
  }

  void app_tick( bsp::AppData* appData )
  {

    // bsm::File* test = bsm::load_file( bsp::defaultModules.defaultFileSystem, "bs.ttf" );

    bsm::Font* testFont = bsm::create_font_from_ttf_file( "bs.ttf" );
    testFont = testFont;
    //    auto* fs = bsm::create_filesystem();
        //mount_path_to_filesystem( fs, "../../data/" );

        // BREAK;


  }

};
