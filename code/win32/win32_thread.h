#pragma once

#include <core/bsthread.h>

namespace thread
{
  void request_pause( ThreadInfo* );
  void request_unpause( ThreadInfo* );
  void wait_for_thread_to_pause( ThreadInfo* );
  void pause_thread_if_requested( ThreadInfo* );
  void pause_thread_if_requested( ThreadInfo*, s32 millisecondsSleepPerPoll );

};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////inl//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


namespace thread
{


  INLINE void request_pause( ThreadInfo* threadInfo )
  {
    threadInfo->requestPause.compare_exchange( 1, 0 );
  }

  INLINE void request_unpause( ThreadInfo* threadInfo )
  {
    threadInfo->requestPause.compare_exchange( 0, 1 );
  }

  INLINE void wait_for_thread_to_pause( ThreadInfo* threadInfo )
  {
    while ( !threadInfo->isPaused )
    {
      thread::sleep( 0 );
    }
  }


  void pause_thread_if_requested( ThreadInfo* threadInfo, s32 millisecondsSleepPerPoll )
  {
    if ( !is_current_thread( threadInfo ) )
    {
      BREAK;
    }

    if ( threadInfo->requestPause )
    {
      threadInfo->isPaused.compare_exchange( 1, 0 );

      while ( threadInfo->requestPause )
      {
        thread::sleep( millisecondsSleepPerPoll );
      }
      threadInfo->isPaused.compare_exchange( 0, 1 );
    }
  }

  INLINE void pause_thread_if_requested( ThreadInfo* threadInfo )
  {
    pause_thread_if_requested( threadInfo, 0 );
  }
};