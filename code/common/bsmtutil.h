#pragma once

#include <common/bscommon.h>

#define LOCK_SCOPE(lockAtomic) thread::LockingObject prevent_locks_in_locks_tmp_object { &lockAtomic }

INLINE s16   interlocked_increment16( s16 volatile* value );
INLINE s16   interlocked_decrement16( s16 volatile* value );
INLINE s32   interlocked_increment( s32 volatile* value );
INLINE s32   interlocked_decrement( s32 volatile* value );
INLINE s32   interlocked_compare_exchange( s32 volatile* value, s32 new_value, s32 comparand );
INLINE void* interlocked_compare_exchange_ptr( void* volatile* value, void* new_value, void* comparand );

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

namespace thread
{
  struct LockingObject
  {
    LockingObject( atomic32* lock );
    ~LockingObject();
  private:
    atomic32* lock;
  };

  INLINE void sleep( s32 milliseconds );
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////inl//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include <xthreads.h>
#endif

#ifdef _WIN32
INLINE s16   interlocked_increment16( s16 volatile* value ) { return _InterlockedIncrement16( (s16*) value ); }
INLINE s16   interlocked_decrement16( s16 volatile* value ) { return _InterlockedDecrement16( (s16*) value ); }
INLINE s32   interlocked_increment( s32 volatile* value ) { return _InterlockedIncrement( (long*) value ); }
INLINE s32   interlocked_decrement( s32 volatile* value ) { return _InterlockedDecrement( (long*) value ); }
INLINE s32   interlocked_compare_exchange( s32 volatile* value, s32 new_value, s32 comparand ) { return _InterlockedCompareExchange( (long*) value, new_value, comparand ); }
INLINE void* interlocked_compare_exchange_ptr( void* volatile* value, void* new_value, void* comparand ) { return _InterlockedCompareExchangePointer( value, new_value, comparand ); }
#endif

namespace thread
{
  LockingObject::LockingObject( atomic32* lock ) : lock( lock )
  {
    if ( *lock )
    {
      while ( *lock )
      {
        thread::sleep( 0 );
      }
    }
    lock->increment();
  }


  LockingObject::~LockingObject()
  {
    assert( *lock );
    lock->decrement();
  }

  INLINE void sleep( s32 milliseconds )
  {
    xtime timer {};
    timer.nsec = 1000 * (s64) milliseconds;
    _Thrd_sleep( &timer );
  }
};