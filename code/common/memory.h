#pragma once

#include "preprocessor.h"
#include "basic_types.h"

struct GeneralArena
{

  u32 capacity;
};


struct FixedArena
{
  s32 capacity;
};

//#include <malloc.h>

namespace memory
{
  void copy( char* source, char* destination, s64 size )
  {
    char* reader = source;
    char* writer = destination;
    while ( size-- > 0 )
    {
      *writer++ = *reader++;
    }
  }


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
      //  return (char*) malloc( (size_t) size );
        //TODO compensate for fragmentation a bit?
        //TODO alignment
        //TODO thread safety?
      char* result = nullptr;

      if ( current + size < (char*) (lastEntry - 1) )
      {
        result = current;
        lastEntry--;
        lastEntry->begin = result;
        lastEntry->size = size;
        lastEntry->flags = ALIVE;
        ++entryCount;
        current += size;
      }
      else
      {
        BREAK;
      }

      return result;
    }

    //TODO: if realloc is ever needed, finish this

    // char* realloc( char* ptr, s64 newSize )
    // {
    //   Entry* entry = lastEntry;
    //   //tag entry
    //   u32 entryFound = 0;
    //   while ( entry != (Entry*) bufferEnd )
    //   {
    //     if ( entry->begin == ptr )
    //     {
    //       entry->flags &= ~ALIVE;
    //       entryFound = 1;
    //       break;
    //     }
    //     ++entry;
    //   }
    //   assert( entryFound );
    //   if ( entry == lastEntry )
    //   {
    //     if ( current + (newSize - entry.size) < (char*) (lastEntry - 1) )
    //     {
    //     }
    //   }
    // }

    s64 get_size( char* ptr )
    {
      Entry* entry = lastEntry;

      while ( entry != (Entry*) bufferEnd )
      {
        if ( entry->begin == ptr )
        {
          return entry->size;
        }
        ++entry;
      }

      //TODO: ptr is not in arena
      assert( 0 );
      return 0;
    }

    void free( void* ptr )
    {
      if ( ptr != nullptr )
      {
        Entry* entry = lastEntry;

        //tag entry
        u32 entryFound = 0;
        while ( entry != (Entry*) bufferEnd )
        {
          if ( entry->begin == (char*) ptr )
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
          --entryCount;
        }
      }
      else
      {
        BREAK;
      }
    }

    void clear()
    {
      current = bufferBegin;
      lastEntry = (Entry*) bufferEnd;
    }

    char* bufferBegin;
    char* bufferEnd;
    char* current;
    Entry* lastEntry;
    s32 entryCount;
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
    outArena.entryCount = 0;
  }

  Arena* init_arena_in_place( char* memory, s64 capacity )
  {
    char* writer = memory;
    Arena* result = (Arena*) writer;
    writer += sizeof( Arena );
    init_arena( writer, capacity - sizeof( Arena ), *result );

    return result;
  }


}

