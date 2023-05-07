
#include <platform/bs_platform.h>

#include "klein.hpp"

#include <core/bs_map.h>
#include <core/bs_uicontext.h>


#include <core/bs_font.h>

#include <core/bs_allocator.h>

#include <memory>

#include <core/bs_performanceprofile.h>

struct AppUserData
{
  int test;
  bs::UIContext context;
};

namespace bs
{
  void app_on_load( bsp::AppData* appData )
  {
    if ( !appData->userData )
    {
      appData->userData = (AppUserData*) bs::allocate( bsp::platform->default.allocator, sizeof( AppUserData ) );
    }

    bs::mount_path_to_filesystem( bsp::platform->default.fileSystem, "/../../code/shader" );
    bs::File* testTexture = bs::load_file( bsp::platform->default.fileSystem, "test_texture.glsl" );

    bsp::platform->create_shader_program( (char const*) testTexture->data, (s32) testTexture->size );

    // bs::Map<char const*, int> testMap;

    // testMap.find( "test" );
    // testMap[""];

//    int textArea;

    //screen virtualisation
    //textmesh

    //canvas
    // -> canvas transformation to screen
    //textarea location on that canvas
    //textarea text content


    auto* allocator = bs::create_buddy_allocator( MegaBytes( 1 ) );
    //auto* allocator = bs::create_slow_thread_safe_allocator( GigaBytes( 2 ) );


    auto* swg4 = bs::allocate( allocator, KiloBytes( 9 ) );

    u64 counter1;
    u64 counter2;
    u64 counter3;

    s32 runs = 10000;
    {
      PROFILE_SCOPE( counter1 );

      for ( int i = 0; i < runs; ++i )
      {
        auto* swag = bs::allocate( allocator, 464 );
        auto* swag2 = bs::allocate( allocator, KiloBytes( 5 ) );
        auto* swag3 = bs::allocate( allocator, KiloBytes( 3 ) );
        //  bs::defragment_existing_allocation( allocator, swag, 464 );
        //  bs::defragment_existing_allocation( allocator, swag2, KiloBytes( 5 ) );
        //  bs::defragment_existing_allocation( allocator, swag3, KiloBytes( 3 ) );

        free( allocator, swag );
        free( allocator, swag2 );
        free( allocator, swag3 );

      }
    }

    {
      PROFILE_SCOPE( counter2 );

      for ( int i = 0; i < runs; ++i )
      {
        auto* swag = malloc( 464 );
        auto* swag2 = malloc( KiloBytes( 5 ) );
        auto* swag3 = malloc( KiloBytes( 3 ) );

        ::free( swag );
        ::free( swag2 );
        ::free( swag3 );
      }
    }

    {
      PROFILE_SCOPE( counter3 );

      for ( int i = 0; i < runs; ++i )
      {
        auto* swag = bsp::platform->allocate_new_app_memory( 464 );
        auto* swag2 = bsp::platform->allocate_new_app_memory( KiloBytes( 5 ) );
        auto* swag3 = bsp::platform->allocate_new_app_memory( KiloBytes( 3 ) );
        //  bs::defragment_existing_allocation( allocator, swag, 464 );
        //  bs::defragment_existing_allocation( allocator, swag2, KiloBytes( 5 ) );
        //  bs::defragment_existing_allocation( allocator, swag3, KiloBytes( 3 ) );

        bsp::platform->free_app_memory( swag );
        bsp::platform->free_app_memory( swag2 );
        bsp::platform->free_app_memory( swag3 );

      }
    }




    log_info( counter1 );
    log_info( counter2 );
    log_info( counter3 );
    // bs::destroy_buddy_allocator( allocator );

     // auto* allocator = bs::create_buddy_allocator( 256 );

     //auto* al1 = bs::allocate( allocator, 2000 );
    // auto* al = bs::allocate( allocator, KiloBytes( 4 ) );
   //  auto* al1 = bs::allocate( allocator, 512 + 128 );
     // auto* al = bs::allocate( allocator, 16 );
     // auto* al2 = bs::allocate( allocator, 16 );

    // bs::tight_fit_existing_allocation( allocator, (char*) al1, 512 + 128 );

    // bs::free( allocator, al, KiloBytes( 4 ) );
    // bs::free( allocator, al1, 20 );

    // al1 = bs::allocate( allocator, KiloBytes( 3 ) );
    // al = bs::allocate( allocator, 12345678 );

    // bs::destroy_buddy_allocator( allocator );
    // al = nullptr;
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








    //    auto* fs = bs::create_filesystem();
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