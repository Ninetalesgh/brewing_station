#pragma once

#include <platform/bs_platform.h>
#include <module/bs_debuglog.h>
#include <core/bsthread.h>
#include <common/bs_common.h>

namespace bsm
{
  struct SlowThreadSafeAllocator
  {
    struct Entry
    {
      char* begin;
      char* end;
    };

    char* buffer;
    Entry* entries;
    Entry* lastEntry;
    atomic32 threadGuard;
  };

  [[nodiscard]]
  /// @brief 
  /// @param size will be the total size of the allocation including the allocator and overhead, 
  /// this is not to be precisely sized.  
  /// @return 
  SlowThreadSafeAllocator* create_slow_thread_safe_allocator( s64 size );

  void destroy_slow_thread_safe_allocator( SlowThreadSafeAllocator* allocator );

  [[nodiscard]]
  /// @brief 
  /// @param allocator 
  /// @param size 
  /// @return 64 byte aligned memory block of asked size
  void* allocate( SlowThreadSafeAllocator* allocator, s64 size );
  void* allocate_to_zero( SlowThreadSafeAllocator* allocator, s64 size );
  void free( SlowThreadSafeAllocator* allocator, void* address );

};


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////cpp/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


#include <memory>

namespace bsm
{
  INLINE char* align_pointer_forward( char* ptr, s32 byteAlignment ) { return (char*) (((u64( ptr ) - 1) | (byteAlignment - 1)) + 1); }

  void* allocate_to_zero( SlowThreadSafeAllocator* allocator, s64 size )
  {
    void* result = allocate( allocator, size );
    memset( result, 0, size );
    return result;
  }
  void* allocate( SlowThreadSafeAllocator* allocator, s64 size )
  {
    LOCK_SCOPE( allocator->threadGuard ); //This isn't meant to be called often, not necessary to optimize now

    //make sure there's space for entry
    if ( allocator->entries->end + sizeof( SlowThreadSafeAllocator::Entry ) > (char*) allocator->entries )
    {
      BREAK; //TODO more space
      return nullptr;
    }

    s64 bestSlotSize = s64( ((char*) (allocator->entries - 1)) - allocator->entries->end );
    SlowThreadSafeAllocator::Entry* bestSlot = allocator->entries;
    SlowThreadSafeAllocator::Entry* search = allocator->lastEntry;

    //linear find smallest space that fits O(n)
    while ( search > allocator->entries )
    {
      s64 slotSize = search[-1].begin - search[0].end;
      if ( slotSize > size && slotSize < bestSlotSize )
      {
        bestSlotSize = slotSize;
        bestSlot = search;
      }
      --search;
    }

    if ( bestSlotSize < size )
    {
      BREAK; //TODO more space
      return nullptr;
    }

    size_t bytes = (bestSlot - allocator->entries) * sizeof( SlowThreadSafeAllocator::Entry );
    memmove( allocator->entries - 1, allocator->entries, bytes );
    --allocator->entries;
    --bestSlot;
    bestSlot->begin = bestSlot[1].end;
    bestSlot->end = align_pointer_forward( bestSlot->begin + size, 64 );

    return bestSlot->begin;
  }

  void free( SlowThreadSafeAllocator* allocator, void* address )
  {
    LOCK_SCOPE( allocator->threadGuard );

    SlowThreadSafeAllocator::Entry* search = allocator->entries;

    while ( search < allocator->lastEntry )
    {
      if ( search->begin == (char*) address )
      {
        size_t bytes = (search - allocator->entries) * sizeof( SlowThreadSafeAllocator::Entry );
        memmove( allocator->entries + 1, allocator->entries, bytes );
        ++allocator->entries;
        return;
      }

      ++search;
    }
    BREAK; //TODO address wasn't in this allocator? 
  }

  SlowThreadSafeAllocator* create_slow_thread_safe_allocator( s64 size )
  {
    char* allocation = (char*) bsp::platform->allocate_new_app_memory( size );
    SlowThreadSafeAllocator* result = (SlowThreadSafeAllocator*) allocation;

    if ( allocation )
    {
      result->buffer = align_pointer_forward( allocation + sizeof( SlowThreadSafeAllocator ), 64 );

      result->entries = (SlowThreadSafeAllocator::Entry*) (allocation + size - sizeof( SlowThreadSafeAllocator::Entry ));
      result->entries[0].end = (char*) result->buffer;
      result->entries[0].begin = nullptr;
      result->lastEntry = result->entries;
      result->threadGuard = atomic32();
    }
    else
    {
      BREAK;
    }

    return result;
  }

  void destroy_slow_thread_safe_allocator( SlowThreadSafeAllocator* allocator )
  {
    bsp::platform->free_app_memory( allocator );
  }
};