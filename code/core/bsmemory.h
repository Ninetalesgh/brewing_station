DEPRECATED

#pragma once

#include <core/bsthread.h>
#include <common/bs_common.h>


//TODO arena code
namespace bs
{
  namespace memory
  {
    struct Arena;
    struct ArenaEntry;

    namespace debug
    {
      struct ArenaDebugData
      {
        s64 size;
        s64 capacity;
        s32 totalEntries;
        s32 inactiveEntries;
      };

      struct ArenaObserver
      {
        void fetch_debug_data( ArenaDebugData* out_DebugData );
        memory::Arena const* arena;
      };
    };

    // best used LIFO, some unordered deallocation OK, will result in fragmentation though.
    struct Arena
    {
      void* alloc( s64 size, u32 alignment = 4 );
      void* alloc_set_zero( s64 size, u32 alignment = 4 );
      s64 get_entry_size( char* ptr );
      void free( void* ptr );
      void clear();

      char* bufferBegin;
      char* bufferEnd;
      char* current;
      ArenaEntry* lastEntry;
      s32 entryCount;
      atomic32 guard;

    public:
      Arena();
    private:
      Arena( Arena const& ) {}
    };

    INLINE void copy( char* destination, char* source, s64 size );
    INLINE void set_zero( char* target, s64 size );

    void init_arena( char* memory, s64 capacity, Arena* out_Arena );
    Arena* init_arena_in_place( char* memory, s64 capacity );

  };

};



////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////cpp/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////




namespace bs
{
  namespace memory
  {
    enum EntryFlags: u64
    {
      ALIVE = 0x1,

    };
    DEFINE_ENUM_OPERATORS_U64( EntryFlags );

    struct ArenaEntry
    {
      char* begin;
      s64   size;
      EntryFlags flags;
    };

    namespace debug
    {
      void ArenaObserver::fetch_debug_data( ArenaDebugData* out_DebugData )
      {
        if ( out_DebugData )
        {
          s32 inactiveEntries = 0;
          memory::ArenaEntry const* entry = arena->lastEntry;

          while ( entry != (memory::ArenaEntry*) arena->bufferEnd )
          {
            if ( !(entry->flags & ALIVE) )
            {
              ++inactiveEntries;
            }
            ++entry;
          }

          out_DebugData->size = arena->current - arena->bufferBegin;
          out_DebugData->capacity = arena->bufferEnd - arena->bufferBegin;
          out_DebugData->totalEntries = arena->entryCount;
          out_DebugData->inactiveEntries = inactiveEntries;
        }
        else
        {
          BREAK;
        }
      }
    };

    Arena::Arena()
      : bufferBegin( nullptr )
      , bufferEnd( nullptr )
      , current( nullptr )
      , lastEntry( nullptr )
      , entryCount( 0 )
    {}

    void* Arena::alloc( s64 size, u32 alignment /*= 4*/ )
    {
      LOCK_SCOPE( guard );

      //TODO compensate for fragmentation a bit?
      char* result = nullptr;

      current += ((u64) current) & (alignment - 1); //TODO this is wrong, right? it should be ^?

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

    void* Arena::alloc_set_zero( s64 size, u32 alignment /*= 4*/ )
    {
      char* result = (char*) alloc( size, alignment );
      set_zero( result, size );
      return result;
    }

    s64 Arena::get_entry_size( char* ptr )
    {
      ArenaEntry* entry = lastEntry;

      while ( entry != (ArenaEntry*) bufferEnd )
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

    void Arena::free( void* ptr )
    {
      LOCK_SCOPE( guard ); //TODO this is a bit more overhead than necessary

      if ( ptr != nullptr && ptr < bufferEnd && ptr >= bufferBegin )
      {
        ArenaEntry* entry = lastEntry;

        //tag entry
        u32 entryFound = 0;
        while ( entry != (ArenaEntry*) bufferEnd )
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
        while ( lastEntry != (ArenaEntry*) bufferEnd )
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

    void Arena::clear()
    {
      current = bufferBegin;
      lastEntry = (ArenaEntry*) bufferEnd;
    }

    void init_arena( char* memory, s64 capacity, Arena* out_Arena )
    {
      //out of bounds check:
      #if !BS_BUILD_RELEASE
      * (memory + capacity - 1) = 0;
      #endif
      if ( out_Arena )
      {
        out_Arena->bufferBegin = memory;
        out_Arena->bufferEnd = memory + capacity;
        out_Arena->current = out_Arena->bufferBegin;
        out_Arena->lastEntry = (ArenaEntry*) out_Arena->bufferEnd;
        out_Arena->entryCount = 0;
      }
      else
      {
        BREAK;
      }
    }

    Arena* init_arena_in_place( char* memory, s64 capacity )
    {
      char* writer = memory;
      Arena* result = (Arena*) writer;
      writer += sizeof( Arena );
      init_arena( writer, capacity - sizeof( Arena ), result );

      return result;
    }

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

  };

};