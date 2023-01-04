#pragma once

#include "platform_callbacks.h"

namespace bs
{
  namespace memory { struct Arena; };
};

namespace platform
{
  namespace callbackfunctionsignature
  {
    using debug_log = void( bs::debug::DebugLogFlags, char const*, s32 );

    using send_tcp = void( bs::net::TCPSendParameter const& );
    using send_udp = void( bs::net::UDPSendParameter const& );

    using get_file_info = u32( char const* filePath, bs::file::Info* out_fileInfo );
    using load_file_into_memory = u32( char const* filePath, bs::file::Data* out_loadedFileData );
    using write_file = u32( char const* filePath, void const* data, u32 size );

    using push_async_task = u32( bs::Task const&, bs::TaskState volatile* out_taskState );
    using push_synced_task = u32( bs::Task const&, bs::TaskState volatile* out_taskState );
    using complete_synced_tasks = void();

    using allocate_texture = bs::graphics::TextureID( bs::graphics::TextureData const* );
    using free_texture = void( bs::graphics::TextureID );
    using allocate_mesh = bs::graphics::Mesh( bs::graphics::MeshData const* );
    using free_mesh = void( bs::graphics::Mesh );

    using render = void( bs::graphics::RenderTarget*, bs::graphics::RenderGroup*, bs::graphics::Camera* );

    using bind_shader = void();
    using unbind_shader = void();
  };

  struct Callbacks
  {
    callbackfunctionsignature::debug_log* ptr_debug_log;

    callbackfunctionsignature::send_udp* ptr_send_udp;
    callbackfunctionsignature::send_tcp* ptr_send_tcp;

    callbackfunctionsignature::push_async_task* ptr_push_async_task;
    callbackfunctionsignature::push_synced_task* ptr_push_synced_task;
    callbackfunctionsignature::complete_synced_tasks* ptr_complete_synced_tasks;

    callbackfunctionsignature::get_file_info* ptr_get_file_info;
    callbackfunctionsignature::load_file_into_memory* ptr_load_file_into_memory;
    callbackfunctionsignature::write_file* ptr_write_file;

    callbackfunctionsignature::allocate_texture* ptr_allocate_texture;
    callbackfunctionsignature::free_texture* ptr_free_texture;
    callbackfunctionsignature::allocate_mesh* ptr_allocate_mesh;
    callbackfunctionsignature::free_mesh* ptr_free_mesh;

    callbackfunctionsignature::render* ptr_render;

    bs::memory::Arena* mainArena;
  };
};

//extern "C" void register_callbacks( platform::Callbacks const& );