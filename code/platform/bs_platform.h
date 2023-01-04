#pragma once

#include <common/bscommon.h>

#include<module/bs_debuglog.h>
#include <core/bsinput.h>


namespace bsp
{

  using debug_log_fn = void( bsm::DebugLogFlags, char const*, s32 );

  using allocate_fn = void* (s64 size);
  using free_fn = void( void* );

  using get_file_info_fn = bool( char const* filePath, u64* out_fileSize );
  using load_file_part_fn = bool( char const* filePath, u64 readOffset, void* targetBuffer, u32 bufferSize );
  using write_file_fn = bool( char const* filePath, void const* data, u32 size );

  struct PlatformCallbacks
  {
    //logging
    debug_log_fn* debug_log;

    //allocations for larger chunks
    allocate_fn* allocate;
    free_fn* free;

    //file IO
    get_file_info_fn* get_file_info;
    load_file_part_fn* load_file_part;
    write_file_fn* write_file;




  };
  extern PlatformCallbacks* platform;

  struct AppData
  {
    u64 currentFrameIndex;
    float deltaTime;
    bs::input::State input;
    // net::NetworkData network;
    void* userData;
  };

  #define APP_ON_LOAD_PARAMETERS bsp::AppData* appData, bsp::PlatformCallbacks* platformData
  using app_on_load_fn = void( APP_ON_LOAD_PARAMETERS );
  extern "C" app_on_load_fn app_on_load_internal;

  #define APP_TICK_PARAMETERS bsp::AppData* appData
  using app_tick_fn = void( APP_TICK_PARAMETERS );
  extern "C" app_tick_fn app_tick_internal;


  //  using send_tcp = void( bs::net::TCPSendParameter const& );
  //  using send_udp = void( bs::net::UDPSendParameter const& );


  //  using push_async_task = u32( bs::Task const&, bs::TaskState volatile* out_taskState );
  //  using push_synced_task = u32( bs::Task const&, bs::TaskState volatile* out_taskState );
  //  using complete_synced_tasks = void();

  //  using allocate_texture = bs::graphics::TextureID( bs::graphics::TextureData const* );
  //  using free_texture = void( bs::graphics::TextureID );
  //  using allocate_mesh = bs::graphics::Mesh( bs::graphics::MeshData const* );
  //  using free_mesh = void( bs::graphics::Mesh );

  //  using render = void( bs::graphics::RenderTarget*, bs::graphics::RenderGroup*, bs::graphics::Camera* );

  //  using bind_shader = void();
  //  using unbind_shader = void();
    /*

    //debug



    //asset IO
      mount path
      get asset info
      load asset
      save asset

    //tasks & threading

    //memory


    //graphics

      resize_viewport
      allocate_texture
      free_texture
      allocate_mesh
      free_mesh
      create_shader_program
      destroy_shader_program


     */



};