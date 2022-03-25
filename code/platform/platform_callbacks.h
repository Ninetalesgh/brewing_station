#pragma once

#include <core/bsdebuglog.h>

#include <core/bsnet.h>
#include <core/bsfile.h>
#include <core/bstask.h>

namespace platform
{
  //debug log callback. use defines in bsdebuglog for ease of use
  void debug_log( bs::debug::DebugLogFlags, char const* string, s32 size );

  //TODO//////
    //networking
  void send_tcp( bs::net::TCPSendParameter const& );
  void send_udp( bs::net::UDPSendParameter const& );

  //file io
  // u32 get_file_info( char const* filename, bs::FileInfo* out_FileInfo );
  // bs::ReadFileResult read_file( char const* filename, u32 maxSize, void* out_data );
  // void free_file( void* filename );
  // u32 write_file( bs::WriteFileParameter const& );
  ////////////

    //threading
  u32 push_async_task( bs::Task const&, bs::TaskState volatile* out_taskState = nullptr );
  u32 push_synced_task( bs::Task const&, bs::TaskState volatile* out_taskState = nullptr );
  void complete_synced_tasks();
};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////inl/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
namespace bs { namespace memory { struct Arena; } }

namespace platform
{
  namespace callbackfunctionsignature
  {
    using debug_log = void( bs::debug::DebugLogFlags, char const*, s32 );
    using send_tcp = void( bs::net::TCPSendParameter const& );
    using send_udp = void( bs::net::UDPSendParameter const& );

    using get_file_info = u32( char const* filename );
    using read_file = void( char const* filename, u32 maxSize, void* out_data );
    using free_file = void( void* filename );
    using write_file = u32( bs::WriteFileParameter const& );

    using push_async_task = u32( bs::Task const&, bs::TaskState volatile* out_taskState );
    using push_synced_task = u32( bs::Task const&, bs::TaskState volatile* out_taskState );
    using complete_synced_tasks = void();
  };

  extern callbackfunctionsignature::debug_log* ptr_debug_log;
  extern callbackfunctionsignature::push_async_task* ptr_push_async_task;
  extern callbackfunctionsignature::push_synced_task* ptr_push_synced_task;
  extern callbackfunctionsignature::complete_synced_tasks* ptr_complete_synced_tasks;

  extern callbackfunctionsignature::get_file_info* ptr_get_file_info;
  extern callbackfunctionsignature::read_file* ptr_read_file;
  extern callbackfunctionsignature::write_file* ptr_write_file;
  extern callbackfunctionsignature::free_file* ptr_free_file;

  extern callbackfunctionsignature::send_udp* ptr_send_udp;
  extern callbackfunctionsignature::send_tcp* ptr_send_tcp;
  extern bs::memory::Arena* mainArena;

  INLINE void debug_log( bs::debug::DebugLogFlags flags, char const* string, s32 size )
  {
    return ptr_debug_log( flags, string, size );
  }
  INLINE void send_tcp( bs::net::TCPSendParameter const& prm )
  {
    return ptr_send_tcp( prm );
  }
  INLINE void send_udp( bs::net::UDPSendParameter const& prm )
  {
    return ptr_send_udp( prm );
  }

  // INLINE u32 get_file_info( char const* filename, bs::FileInfo* out_FileInfo )
  // {
  //   return ptr_get_file_info( filename, out_FileInfo );
  // }
  // INLINE bs::ReadFileResult read_file( char const* filename, u32 maxSize, void* out_data )
  // {
  //   return ptr_read_file( filename, maxSize, out_data );
  // }
  // INLINE void free_file( void* filename )
  // {
  //   return ptr_free_file( filename );
  // }
  // INLINE u32 write_file( bs::WriteFileParameter const& prm )
  // {
  //   return ptr_write_file( prm );
  // }

  INLINE u32 push_async_task( bs::Task const& task, bs::TaskState volatile* out_taskState )
  {
    return ptr_push_async_task( task, out_taskState );
  }
  INLINE u32 push_synced_task( bs::Task const& task, bs::TaskState volatile* out_taskState )
  {
    return ptr_push_synced_task( task, out_taskState );
  }
  INLINE void complete_synced_tasks()
  {
    return ptr_complete_synced_tasks();
  }
};
