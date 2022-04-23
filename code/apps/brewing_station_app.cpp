#define BUILD_TESTAPP
//#define BUILD_FIRST_APP

#if defined(BUILD_TESTAPP)
#include "testapp/test_app_main.cpp"
#elif defined(BUILD_FIRST_APP)
#include "first_app/first_app_main.cpp"

#endif


#include <core/internal/bsfile.cpp>
#include <core/internal/bstask.cpp>
#include <core/internal/bsnet.cpp>
#include <core/internal/bsgraphics.cpp>


#include <platform/platform_callbacks_internal.h>
namespace bs { namespace memory { struct Arena; } }

namespace platform
{
  Callbacks callbacks;
  void register_callbacks( platform::Callbacks const& prm )
  {
    callbacks = prm;
  }
};

namespace platform
{
  INLINE void debug_log( bs::debug::DebugLogFlags flags, char const* string, s32 size )
  {
    return callbacks.ptr_debug_log( flags, string, size );
  }

  INLINE void send_tcp( bs::net::TCPSendParameter const& prm )
  {
    return callbacks.ptr_send_tcp( prm );
  }
  INLINE void send_udp( bs::net::UDPSendParameter const& prm )
  {
    return callbacks.ptr_send_udp( prm );
  }

  INLINE u32 get_file_info( char const* filePath, bs::file::Info* out_fileInfo )
  {
    return callbacks.ptr_get_file_info( filePath, out_fileInfo );
  }

  INLINE u32 load_file_into_memory( char const* filePath, bs::file::Data* out_loadedFileData )
  {
    return callbacks.ptr_load_file_into_memory( filePath, out_loadedFileData );
  }

  INLINE u32 write_file( char const* filePath, void const* data, u32 size )
  {
    return callbacks.ptr_write_file( filePath, data, size );
  }

  INLINE u32 push_async_task( bs::Task const& task, bs::TaskState volatile* out_taskState )
  {
    return callbacks.ptr_push_async_task( task, out_taskState );
  }
  INLINE u32 push_synced_task( bs::Task const& task, bs::TaskState volatile* out_taskState )
  {
    return callbacks.ptr_push_synced_task( task, out_taskState );
  }
  INLINE void complete_synced_tasks()
  {
    return callbacks.ptr_complete_synced_tasks();
  }
  INLINE void* allocate( s64 size )
  {
    return platform::callbacks.mainArena->alloc( size );
  }
  INLINE void* allocate_to_zero( s64 size )
  {
    return platform::callbacks.mainArena->alloc_set_zero( size );
  }
  INLINE void free( void* ptr )
  {
    platform::callbacks.mainArena->free( ptr );
  }

  INLINE bs::graphics::TextureID allocate_texture( bs::graphics::TextureData const* textureData )
  {
    return platform::callbacks.ptr_allocate_texture( textureData );
  }

  INLINE void free_texture( bs::graphics::TextureID id )
  {
    return platform::callbacks.ptr_free_texture( id );
  }

  INLINE bs::graphics::Mesh allocate_mesh( bs::graphics::MeshData const* raw )
  {
    return platform::callbacks.ptr_allocate_mesh( raw );
  }

  INLINE void free_mesh( bs::graphics::Mesh mesh )
  {
    return platform::callbacks.ptr_free_mesh( mesh );
  }

  INLINE void render( bs::graphics::RenderTarget* rt, bs::graphics::RenderGroup* rg, bs::graphics::Camera* cam )
  {
    return platform::callbacks.ptr_render( rt, rg, cam );
  }
};


