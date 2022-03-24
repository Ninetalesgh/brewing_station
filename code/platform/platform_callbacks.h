#pragma once

#include <core/bsdebuglog.h>

#include <core/bsnet.h>
#include <core/bsfile.h>
#include <core/bstask.h>

namespace platform
{
  namespace callbackfunctionsignature
  {
    //debug calls
    using debug_log = void( bs::debug::DebugLogFlags, char const*, s32 );

    //networking
    using send_tcp = void( bs::net::TCPSendParameter const& );

    using send_udp = void( bs::net::UDPSendParameter const& );

    //file IO
    using get_file_info = u32( char const* filename, bs::FileInfo* out_FileInfo );

    using read_file = bs::ReadFileResult( char const* filename, u32 maxSize, void* out_data );

    using free_file = void( void* filename );

    using write_file = u32( bs::WriteFileParameter const& );

    //threading
    using push_async_task = void( bs::Task const&, bs::TaskState volatile* out_taskState );

    using push_synced_task = void( bs::Task const&, bs::TaskState volatile* out_taskState );

    using complete_synced_tasks = void();
  };

  extern callbackfunctionsignature::debug_log* debug_log;
  extern callbackfunctionsignature::push_async_task* push_async_task;
  extern callbackfunctionsignature::push_synced_task* push_synced_task;
  extern callbackfunctionsignature::complete_synced_tasks* complete_synced_tasks;
  extern callbackfunctionsignature::get_file_info* get_file_info;
  extern callbackfunctionsignature::read_file* read_file;
  extern callbackfunctionsignature::write_file* write_file;
  extern callbackfunctionsignature::free_file* free_file;
  extern callbackfunctionsignature::send_udp* send_udp;
  extern callbackfunctionsignature::send_tcp* send_tcp;
};
