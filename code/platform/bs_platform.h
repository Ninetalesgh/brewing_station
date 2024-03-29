#pragma once

#include <common/bs_common.h>
#include <common/bs_bitmap.h>

#include <core/bs_debuglog.h>
#include <core/bs_texture.h>
#include <core/bs_mesh.h>
#include <core/bsinput.h>
#include <core/bstask.h>
#define DEFAULT_WINDOW_SIZE int2( 1024, 780 )

namespace bs
{
  struct BuddyAllocator;
  struct ThreadSafeLinearAllocator;
  struct FileSystem;
  struct Font;
  struct GlyphTable;
};

struct AppUserData;

namespace bsp
{

  using debug_log_fn = void( bs::DebugLogFlags, char const* message, s32 messageSize );

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////    file io    //////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  using get_file_info_fn = bool( char const* filePath, u64* out_fileSize );
  using load_file_part_fn = bool( char const* filePath, u64 readOffset, void* targetBuffer, u32 bufferSize );

  enum class WriteFileFlags: u32
  {
    OVERWRITE_OR_CREATE_NEW = 0x0,
    APPEND_OR_FAIL = 0x1,
    OVERWRITE_OR_FAIL = 0x2,
  };
  using write_file_fn = bool( char const* filePath, void const* data, u32 size, WriteFileFlags );

  using create_directory_fn = bool( char const* directoryPath );

  using get_precompiled_asset_fn = bool( char const* name, void** out_data, u64* out_size );

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////    threading    /////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //returns false on fail
  //pushes a task onto the asynchronous task queue
  //out_taskState can be null if keeping track isn't relevant
  using push_low_priority_task_fn = bool( bs::Task const&, bs::TaskState volatile* out_taskState );

  //returns false on fail
  //pushes a task onto the high priority task queue, which can be collectively waited for with complete_synced_tasks();
  //out_taskState can be null if keeping track isn't relevant
  using push_high_priority_task_fn = bool( bs::Task const&, bs::TaskState volatile* out_taskState );

  //wait for all tasks in the high priority task queue to finish. 
  using wait_for_high_priority_tasks_fn = void();

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////    graphics    //////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  using allocate_mesh_fn = bs::Mesh( bs::MeshData const* raw );
  using free_mesh_fn = void( bs::Mesh const& mesh );

  using allocate_texture_fn = bs::TextureID( bs::TextureData const* );
  using free_texture_fn = void( bs::TextureID );

  using render_custom_bitmap_fn = void( bs::Bitmap* bmp );

  //mark sections with #h, #vs and #fs.
  //example file content:
  //#h
  //#version 450 core
  //#vs
  //void main() { ... }
  //#fs
  //void main() { ... }
  //
  using create_shader_program_fn = bs::ShaderProgramID( char const* combinedglslData, s32 size );
  using destroy_shader_program_fn = void( bs::ShaderProgramID );

  //TODO WIP

  using set_shader_program_data_fn = void( bs::ShaderProgramID, char** names );


  //choose a frame buffer
  //initiate draw calls with program

//  void update_uniform_buffer( UniformBufferID buffer, s64 offset, s64 size, void* data )


  enum class DrawCallSelection: u32
  {
    MESHES_UI
  };

  using render_to_target_fn = void( int renderTarget, bs::ShaderProgramID shaderProgram, void* drawCalls );


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////   memory     //////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  using allocate_new_app_memory_fn = void* (s64 size);
  using free_app_memory_fn = void( void* );

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////    system    ////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  using shutdown_fn = void();

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////    content    ///////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  struct PlatformCallbacks
  {
    //logging
    debug_log_fn* debug_log;

    //file IO
    get_file_info_fn* get_file_info;
    load_file_part_fn* load_file_part;
    write_file_fn* write_file;
    create_directory_fn* create_directory;
    get_precompiled_asset_fn* get_precompiled_asset;

    //task scheduling
    push_low_priority_task_fn* push_low_priority_task;
    push_high_priority_task_fn* push_high_priority_task;
    wait_for_high_priority_tasks_fn* wait_for_high_priority_tasks;

    //graphics
    allocate_mesh_fn* allocate_mesh;
    free_mesh_fn* free_mesh;
    allocate_texture_fn* allocate_texture;
    free_texture_fn* free_texture;
    create_shader_program_fn* create_shader_program;
    render_custom_bitmap_fn* render_custom_bitmap;

    //system
    shutdown_fn* shutdown;

    //allocations for larger chunks
    allocate_new_app_memory_fn* allocate_new_app_memory;
    free_app_memory_fn* free_app_memory;

    //default modules
    struct DefaultModules
    {
      bs::ThreadSafeLinearAllocator* allocator = nullptr;
      bs::BuddyAllocator* mainThreadAllocator = nullptr;
      bs::FileSystem* fileSystem = nullptr;
      bs::Font* font = nullptr;
      bs::GlyphTable* glyphTable = nullptr;
    } default;

  } extern* platform;

  struct AppData
  {
    u64 currentFrameIndex;
    float deltaTime;
    bs::input::State input;
    // net::NetworkData network;

    //This is to be user defined
    AppUserData* userData;
  };

  #define APP_ON_LOAD_PARAMETERS bsp::AppData* appData, bsp::PlatformCallbacks* platformCallbacks, char const* executablePath
  using app_on_load_fn = void( APP_ON_LOAD_PARAMETERS );
  extern "C" app_on_load_fn app_on_load_internal;

  #define APP_TICK_PARAMETERS bsp::AppData* appData, bsp::PlatformCallbacks* platformCallbacks
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