
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
    bsm::mount_path_to_filesystem( bsp::platform->default.fileSystem, "/../../code/shader" );
    bsm::File* test = bsm::load_file( bsp::platform->default.fileSystem, "test_texture.glsl" );


    bsp::platform->create_shader_program( (char const*) test->data, (s32) test->size );

  }

  void app_tick( bsp::AppData* appData )
  {



    //    auto* fs = bsm::create_filesystem();
        //mount_path_to_filesystem( fs, "../../data/" );

        // BREAK;
    // char const* poop = "#h            \n\r #vs          \n\r             #fs   \n\r\n\r";
    // bsp::platform->create_shader_program( poop, array_count( poop ) - 1 );



  }

  //mesh
  //textures

  //shader program
  //uniform buffers & uniforms ?
  //






};
