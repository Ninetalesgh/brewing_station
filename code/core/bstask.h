#pragma once

#include <core/bsthread.h>
#include <common/bscommon.h>

namespace bs
{
  using taskfn = u32( void* );

  struct Task
  {

  };

  // constexpr s32 QUEUE_LENGTH = 64;
  // class TaskQueue
  // {
  // public:
  //   TaskQueue()
  //     : queueLength( QUEUE_LENGTH )
  //     , pushIndex( 0 )
  //     , popIndex( 0 )
  //     , popBarrierIndex( 0 )
  //   {}

  //   taskfn* pop_task()
  //   {

  //   }

  //   void push_task( taskfn* task )
  //   {
  //     s32 index = interlocked_increment( &pushIndex ) - 1;

  //   }

  // private:
  //   taskfn* volatile queue[QUEUE_LENGTH];
  //   s32 queueLength;
  //   s32 volatile pushIndex;
  //   s32 volatile popBarrierIndex;
  //   s32 volatile popIndex;
  // };
};