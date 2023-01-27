
#include <platform/bs_platform.h>

#include "klein.hpp"

#include <core/bs_map.h>

#include <module/bs_font.h>

#include <module/bs_allocator.h>

namespace bs
{





  void app_on_load( bsp::AppData* appData )
  {
    // bsm::Font* test = bsm::create_font_from_ttf_file( "../../data/bs.ttf" );
    // test = test;
   // BREAK;
    bsm::mount_path_to_filesystem( bsp::platform->default.fileSystem, "/../../code/shader" );
    bsm::File* testTexture = bsm::load_file( bsp::platform->default.fileSystem, "test_texture.glsl" );


    bsp::platform->create_shader_program( (char const*) testTexture->data, (s32) testTexture->size );

    // bs::Map<char const*, int> testMap;

    // testMap.find( "test" );
    // testMap[""];


    auto* allocator = bsm::create_buddy_allocator( GigaBytes( 1 ) );
    //  auto* allocator = bsm::create_buddy_allocator( 256 );

      //auto* al1 = bsm::allocate( allocator, 2000 );
     // auto* al = bsm::allocate( allocator, KiloBytes( 4 ) );
    auto* al1 = bsm::allocate( allocator, 20 );
    auto* al = bsm::allocate( allocator, 16 );
    auto* al2 = bsm::allocate( allocator, 16 );

    bsm::free( allocator, al2 );
    bsm::free( allocator, al );

    al1 = bsm::allocate( allocator, KiloBytes( 3 ) );
    al = bsm::allocate( allocator, 12345678 );




    bsm::destroy_buddy_allocator( allocator );
    al = nullptr;
    // auto* all = allocate( allocator, 5000 );
    // auto* all2 = allocate( allocator, 5000 );
    // auto* all3 = allocate( allocator, 5000 );
    // auto* all4 = allocate( allocator, 5000 );

    // free( allocator, all );
    // all = allocate( allocator, 5000 );

    u32 poop[] = { 12,15,33,41,42,47,51,52,53,54,61 };

    s32 test = binary_search( poop, array_count( poop ), 50 );

    test = test;

    HashMap32<s32> mappie;
    mappie.allocate_containers();

    //hash_tester32( hash_name32, 10 );
    //hash_tester64( hash_name64, 1500 );
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