#pragma once

#include <core/bsthread.h>
#include <common/bscommon.h>

namespace bs
{
  using taskfn = void( void* );

  enum class alignas(4) TaskState : u32
  {
    INVALID     = 0x0,
    IN_QUEUE    = 0x1,
    IN_PROGRESS = 0x2,
    COMPLETED   = 0x4,
  };

  struct Task
  {
    bs::taskfn* function;
    void* parameter;
  };

  namespace platform
  {
    using push_async_task = void( Task const&, TaskState volatile* out_taskState );
    using push_synced_task = void( Task const&, TaskState volatile* out_taskState );
    using complete_synced_tasks = void();
  };
};