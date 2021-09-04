#pragma once

#include "preprocessor.h"

struct GeneralArena
{

  u32 capacity;
};


struct FixedArena
{
  s32 capacity;
};


namespace memory
{
  struct GeneralArena
  {

  };

  struct FixedArena
  {

  };

  enum EntryFlags : u64
  {
    ALIVE = 0x1,

  };
  DEFINE_ENUM_OPERATORS_U64( EntryFlags );

  // best used LIFO, some unordered deallocation OK, will result in fragmentation though.
  struct Arena
  {


    struct Entry
    {
      char* begin;
      s64   size;
      EntryFlags flags;
    };

    char* alloc( s64 size )
    {
      //TODO compensate for fragmentation a bit?
      //TODO alignment
      //TODO thread safety?

      if ( current + size < (char*) (lastEntry - 1) )
      {
        current += size;
        lastEntry--;
        lastEntry->begin = current;
        lastEntry->size = size;
        lastEntry->flags = ALIVE;
        return current;
      }
      else
      {
        return nullptr;
      }
    }

    void free( char* ptr )
    {
      Entry* entry = lastEntry;

      //tag entry
      u32 entryFound = 0;
      while ( entry != (Entry*) bufferEnd )
      {
        if ( entry->begin == ptr )
        {
          entry->flags &= ~ALIVE;
          entryFound = 1;
          break;
        }
        entry++;
      }

      assert( entryFound );

      //remove dead entries
      while ( lastEntry != (Entry*) bufferEnd )
      {
        if ( lastEntry->flags & ALIVE )
        {
          break;
        }

        current = lastEntry->begin;
        ++lastEntry;
      }
    }

    char* bufferBegin;
    char* bufferEnd;
    char* current;
    Entry* lastEntry;
  };


  void init_arena( char* memory, s64 capacity, Arena& outArena )
  {
    //out of bounds check:
    #if !BS_BUILD_RELEASE
    * (memory + capacity - 1) = 0;
    #endif

    outArena.bufferBegin = memory;
    outArena.bufferEnd = memory + capacity;
    outArena.current = outArena.bufferBegin;
    outArena.lastEntry = (Arena::Entry*) outArena.bufferEnd;
  }

  Arena* init_arena( char* memory, s64 capacity )
  {
    char* writer = memory;
    Arena* result = (Arena*) writer;
    writer += sizeof( Arena );
    init_arena( writer, capacity - sizeof( Arena ), *result );

    return result;
  }


}

