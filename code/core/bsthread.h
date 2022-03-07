#pragma once

#include <common/bsmtutil.h>
#include <common/bscommon.h>


namespace thread
{
  struct ThreadInfo;

  INLINE void request_pause( ThreadInfo* threadInfo );
  INLINE void request_unpause( ThreadInfo* threadInfo );
  INLINE void wait_if_requested( ThreadInfo* threadInfo, s32 millisecondsSleepPerPoll = 0 );

  struct ThreadInfo
  {
    char const* name;
    u32 id;
    atomic32 requestPause;
    atomic32 isPaused;
  };
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
    threadInfo->requestPause.increment();
  }

  INLINE void request_unpause( ThreadInfo* threadInfo )
  {
    assert( threadInfo->requestPause > 0 );
    threadInfo->requestPause.decrement();
  }

  INLINE void wait_if_requested( ThreadInfo* threadInfo, s32 millisecondsSleepPerPoll /*= 0*/ )
  {
    if ( threadInfo->requestPause )
    {
      threadInfo->isPaused.increment();
      while ( threadInfo->requestPause )
      {
        sleep( millisecondsSleepPerPoll );
      }
      threadInfo->isPaused.decrement();
    }
  }
};
