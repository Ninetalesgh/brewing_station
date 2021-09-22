#pragma once

#include "preprocessor.h"
#include "basic_types.h"

namespace memory
{
  INLINE void copy( char* destination, char* source, s64 size )
  {
    while ( size-- > 0 )
    {
      *destination++ = *source++;
    }
  }

  INLINE void set_zero( char* target, s64 size )
  {
    while ( size-- > 0 )
    {
      *target++ = 0;
    }
  }

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

    void* alloc( s64 size, u32 alignment = 4 )
    {
      //  return (char*) malloc( (size_t) size );
        //TODO compensate for fragmentation a bit?
        //TODO alignment
        //TODO thread safety?
      char* result = nullptr;

      // changing the current pointer permanently shouldn't be an issue, 
      //arenas shouldn't be used for a lot of small entries
      //BS_BUILD_64BIT
      current += ((u64) current) & (alignment - 1);

      if ( current + size < (char*) (lastEntry - 1) )
      {
        result = current;
        current += size;
        ++entryCount;
        lastEntry--;
        lastEntry->begin = result;
        lastEntry->size = size;
        lastEntry->flags = ALIVE;
      }
      else
      {
        BREAK;
      }

      return result;
    }

    void* alloc_set_zero( s64 size, u32 alignment = 4 )
    {
      char* result = (char*) alloc( size, alignment );
      set_zero( result, size );
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

  public:
    Arena()
      : bufferBegin( nullptr )
      , bufferEnd( nullptr )
      , current( nullptr )
      , lastEntry( nullptr )
      , entryCount( 0 )
    {}
  private:
    Arena( Arena const& ) {}
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

