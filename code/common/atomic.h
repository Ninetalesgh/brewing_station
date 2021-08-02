#pragma once 

#include "preprocessor.h"
#include "basic_types.h"

#ifdef _WIN32
#include <intrin.h>
INLINE s16   interlocked_increment16( s16 volatile* value ) { return _InterlockedIncrement16( (s16*) value ); }
INLINE s16   interlocked_decrement16( s16 volatile* value ) { return _InterlockedDecrement16( (s16*) value ); }
INLINE s32   interlocked_increment( s32 volatile* value ) { return _InterlockedIncrement( (long*) value ); }
INLINE s32   interlocked_decrement( s32 volatile* value ) { return _InterlockedDecrement( (long*) value ); }
INLINE s32   interlocked_compare_exchange( s32 volatile* value, s32 new_value, s32 comparand ) { return _InterlockedCompareExchange( (long*) value, new_value, comparand ); }
INLINE void* interlocked_compare_exchange_ptr( void* volatile* value, void* new_value, void* comparand ) { return _InterlockedCompareExchangePointer( value, new_value, comparand ); }
#endif

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

struct LockingObject
{
  LockingObject( atomic32* lock ) : lock( lock )
  {
    if ( *lock )
    {
      while ( *lock )
      {
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

#define LOCK_SCOPE(lockAtomic) LockingObject prevent_locks_in_locks_tmp_object { &lockAtomic }
