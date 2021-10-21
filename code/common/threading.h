#pragma once

#include "preprocessor.h"
#include "basic_types.h"
#include "memory.h"

#define LOCK_SCOPE(lockAtomic) threading::LockingObject prevent_locks_in_locks_tmp_object { &lockAtomic }

namespace threading
{
  struct ThreadInfo;

  INLINE s16   interlocked_increment16( s16 volatile* value );
  INLINE s16   interlocked_decrement16( s16 volatile* value );
  INLINE s32   interlocked_increment( s32 volatile* value );
  INLINE s32   interlocked_decrement( s32 volatile* value );
  INLINE s32   interlocked_compare_exchange( s32 volatile* value, s32 new_value, s32 comparand );
  INLINE void* interlocked_compare_exchange_ptr( void* volatile* value, void* new_value, void* comparand );

  INLINE void sleep( s32 milliseconds );

  INLINE void request_thread_pause( ThreadInfo* threadInfo );
  INLINE void request_thread_unpause( ThreadInfo* threadInfo );
  INLINE void wait_if_requested( ThreadInfo* threadInfo, s32 millisecondsSleepPerPoll = 0 );

  struct alignas(4) atomic32
  {
    atomic32() : value( 0 ) {}
    atomic32( s32 value ) : value( value ) {}
    INLINE operator s32() const { return value; }
    INLINE s32 increment() { return interlocked_increment( &value ); }
    INLINE s32 increment_unsafe() { return ++value; }
    INLINE s32 decrement() { return interlocked_decrement( &value ); }
    INLINE s32 decrement_unsafe() { return --value; }
  private:
    s32 value;
  };

  struct ThreadInfo
  {
    ThreadInfo* parent;
    char const* name;
    u32 id;
    atomic32 requestPause;
    atomic32 isPaused;
    atomic32 hasReturned;
  };


  struct LockingObject
  {
    LockingObject( atomic32* lock ) : lock( lock )
    {
      if ( *lock )
      {
        while ( *lock )
        {
          sleep( 0 );
        }
      }
      lock->increment();
    }
    ~LockingObject()
    {
      assert( *lock );
      lock->decrement();
    }

  private:
    atomic32* lock;
  };
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////inl//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include <intrin.h>
#include <xthreads.h>
#endif

namespace threading
{
  #ifdef _WIN32
  INLINE s16   interlocked_increment16( s16 volatile* value ) { return _InterlockedIncrement16( (s16*) value ); }
  INLINE s16   interlocked_decrement16( s16 volatile* value ) { return _InterlockedDecrement16( (s16*) value ); }
  INLINE s32   interlocked_increment( s32 volatile* value ) { return _InterlockedIncrement( (long*) value ); }
  INLINE s32   interlocked_decrement( s32 volatile* value ) { return _InterlockedDecrement( (long*) value ); }
  INLINE s32   interlocked_compare_exchange( s32 volatile* value, s32 new_value, s32 comparand ) { return _InterlockedCompareExchange( (long*) value, new_value, comparand ); }
  INLINE void* interlocked_compare_exchange_ptr( void* volatile* value, void* new_value, void* comparand ) { return _InterlockedCompareExchangePointer( value, new_value, comparand ); }

  INLINE void sleep( s32 milliseconds )
  {
    xtime timer {};
    timer.nsec = 1000 * (s64) milliseconds;
    _Thrd_sleep( &timer );
  }
  #endif

  INLINE void request_thread_pause( ThreadInfo* threadInfo )
  {
    threadInfo->requestPause.increment();
  }

  INLINE void request_thread_unpause( ThreadInfo* threadInfo )
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





///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////cpp//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

namespace threading
{




};