#pragma once

#include "allocator/bs_buddy_allocator.h"

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
  [[nodiscard]]
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


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////buddy allocator, simple version//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

namespace bsm
{

  struct SimpleBuddyAllocator;

  [[nodiscard]]
  SimpleBuddyAllocator* create_simple_buddy_allocator( s64 size, u32 suggestLeafSize = 0 );
  void destroy_buddy_allocator( SimpleBuddyAllocator* allocator );

  [[nodiscard]]
  void* allocate( SimpleBuddyAllocator* allocator, s64 size );
  [[nodiscard]]
  void* allocate_to_zero( SimpleBuddyAllocator* allocator, s64 size );
  void free( SimpleBuddyAllocator* allocator, void* allocation );
  void free( SimpleBuddyAllocator* allocator, void* allocation, s64 size );

};

////////////////////////////////////////////////////////////////////////////


namespace bsm
{
  struct SimpleBuddyAllocator
  {
    struct FreeNode
    {
      FreeNode* next;
      FreeNode* prev;
    };

    char* buffer;
    FreeNode* freeNodesList;
    u8* metaData;
    u8* metaDataDebugEnd;
    u32 leafSize;
    s32 maxLevel;

  };

  void add_node( SimpleBuddyAllocator::FreeNode* list, SimpleBuddyAllocator::FreeNode* newNode )
  {
    newNode->next = list;
    newNode->prev = list->prev;
    list->prev->next = newNode;
    list->prev = newNode;
  }

  void remove_node( SimpleBuddyAllocator::FreeNode* node )
  {
    node->next->prev = node->prev;
    node->prev->next = node->next;
  }

  INLINE u64 get_size_for_level( SimpleBuddyAllocator* allocator, s32 level )
  {
    return u64( allocator->leafSize ) << level;
  }

  u64 get_block_index_for_address_and_level( SimpleBuddyAllocator* allocator, char* address, s32 level )
  {
    u64 offset = (u64( 1 ) << allocator->maxLevel) + (u64( address - allocator->buffer ) / u64( allocator->leafSize ));
    return offset >> level;
  }

  SimpleBuddyAllocator::FreeNode* get_node_for_block_index_and_level( SimpleBuddyAllocator* allocator, u64 blockIndex, s32 level )
  {
    u64 offset = (blockIndex << u64( level )) - (u64( 1 ) << u64( allocator->maxLevel ));
    char* const address = (allocator->buffer + (offset * u64( allocator->leafSize )));
    //make sure we're actually getting into our memory
    assert( address > (char*) allocator );
    return (SimpleBuddyAllocator::FreeNode*) address;
  }

  s32 get_level_for_size( SimpleBuddyAllocator* allocator, s64 size )
  {
    s32 resultLevel = 0;
    s64 blockSize = s64( allocator->leafSize );
    while ( blockSize < size )
    {
      blockSize *= 2;
      ++resultLevel;
    }
    return resultLevel;
  }

  bool block_state_xor( SimpleBuddyAllocator* allocator, u64 blockIndex )
  {
    blockIndex >>= 1;
    u8& data = allocator->metaData[blockIndex >> 3];
    assert( &data < allocator->metaDataDebugEnd );
    u8 const bit = 1 << (blockIndex % 8);
    data = data ^ bit;
    return data & bit;
  }

  bool block_state_get( SimpleBuddyAllocator* allocator, u64 blockIndex )
  {
    blockIndex >>= 1;
    u8& data = allocator->metaData[blockIndex >> 3];
    assert( &data < allocator->metaDataDebugEnd );
    u8 const bit = 1 << (blockIndex % 8);
    return data & bit;
  }

  bool block_state_get_lower( SimpleBuddyAllocator* allocator, u64 blockIndex )
  {
    u64 const startIndex = (u64( 1 ) << allocator->maxLevel);
    if ( blockIndex < startIndex )
    {
      return block_state_get( allocator, blockIndex << 1 );
    }
    else
    {
      blockIndex = startIndex + ((blockIndex - startIndex) >> 1);
      u8& data = allocator->metaData[blockIndex >> 3];
      assert( &data < allocator->metaDataDebugEnd );
      u8 const bit = 1 << (blockIndex % 8);
      return data & bit;
    }
  }

  void block_state_xor_lower( SimpleBuddyAllocator* allocator, u64 blockIndex )
  {
    u64 const startIndex = (u64( 1 ) << allocator->maxLevel);
    if ( blockIndex < startIndex )
    {
      block_state_xor( allocator, blockIndex << 1 );
    }
    else
    {
      blockIndex = startIndex + ((blockIndex - startIndex) >> 1);
      u8& data = allocator->metaData[blockIndex >> 3];
      assert( &data < allocator->metaDataDebugEnd );
      u8 const bit = 1 << (blockIndex % 8);
      data = data ^ bit;
    }
  }

  bool debug_block_index_matches_level( SimpleBuddyAllocator* allocator, u64 blockIndex, s32 level )
  {
    u64 const controlIndex = (u64( 1 ) << (1 + allocator->maxLevel - level));
    return blockIndex < controlIndex&& blockIndex >= (controlIndex >> 1);
  }

  void* allocate( SimpleBuddyAllocator* allocator, s64 size )
  {
    s32 desiredLevel = get_level_for_size( allocator, size );

    s32 listedLevel = desiredLevel;
    while ( (listedLevel < allocator->maxLevel) && (allocator->freeNodesList[listedLevel].next == &allocator->freeNodesList[listedLevel]) ) { ++listedLevel; }

    if ( (listedLevel >= allocator->maxLevel) )
    {
      BREAK;
      return nullptr; //no space for desired allocation? 
    }

    SimpleBuddyAllocator::FreeNode* desiredNode = allocator->freeNodesList[listedLevel].next;
    remove_node( desiredNode );

    u64 blockIndex = get_block_index_for_address_and_level( allocator, (char*) desiredNode, desiredLevel );

    //set control extra bit at lower level for finder
    if ( desiredLevel || !(blockIndex & 1) )
    {
      assert( !block_state_get_lower( allocator, blockIndex ) );
      block_state_xor_lower( allocator, blockIndex );
    }

    for ( s32 i = desiredLevel; i < listedLevel; ++i )
    {
      SimpleBuddyAllocator::FreeNode* node = get_node_for_block_index_and_level( allocator, blockIndex ^ 1, i );
      assert( debug_block_index_matches_level( allocator, blockIndex ^ 1, i ) );
      add_node( &allocator->freeNodesList[i], node );

      assert( !block_state_get( allocator, blockIndex ) );
      block_state_xor( allocator, blockIndex );
      blockIndex >>= 1;
    }

    block_state_xor( allocator, blockIndex );

    return desiredNode;
  }

  s32 find_level_of_address_for_deallocation( SimpleBuddyAllocator* allocator, char* address )
  {
    s32 level = 0;
    u64 blockIndex = get_block_index_for_address_and_level( allocator, address, 0 );

    if ( !(blockIndex & 1) && !block_state_get_lower( allocator, blockIndex ) )
    {
      ++level;
      while ( !(blockIndex & 1) )
      {
        if ( block_state_get( allocator, blockIndex ) )
        {
          break;
        }
        ++level;

        blockIndex >>= 1;
      }
    }

    return level;
  }

  void free_at_level( SimpleBuddyAllocator* allocator, char* address, s32 level )
  {
    u64 blockIndex = get_block_index_for_address_and_level( allocator, address, level );
    assert( debug_block_index_matches_level( allocator, blockIndex, level ) );

    // control extra bit at lowest level
    if ( !(blockIndex & 1) || level )
    {
      assert( block_state_get_lower( allocator, blockIndex ) );
      block_state_xor_lower( allocator, blockIndex );
    }

    for ( ; level < allocator->maxLevel; ++level )
    {
      if ( !block_state_xor( allocator, blockIndex ) )
      {
        SimpleBuddyAllocator::FreeNode* buddy = get_node_for_block_index_and_level( allocator, blockIndex ^ 1, level );
        remove_node( buddy );
        blockIndex>>=1;
      }
      else
      {
        break;
      }
    }

    SimpleBuddyAllocator::FreeNode* newNode = get_node_for_block_index_and_level( allocator, blockIndex, level );
    assert( debug_block_index_matches_level( allocator, blockIndex, level ) );
    add_node( &allocator->freeNodesList[level], newNode );
  }

  void free( SimpleBuddyAllocator* allocator, void* allocation )
  {
    s32 level = find_level_of_address_for_deallocation( allocator, (char*) allocation );
    free_at_level( allocator, (char*) allocation, level );
  }

  void free( SimpleBuddyAllocator* allocator, void* allocation, s64 size )
  {
    s32 level = get_level_for_size( allocator, size );

    s32 debugLevel = find_level_of_address_for_deallocation( allocator, (char*) allocation );
    assert( level == debugLevel );

    free_at_level( allocator, (char*) allocation, level );
  }

  void* allocate_to_zero( SimpleBuddyAllocator* allocator, s64 size )
  {
    void* result = allocate( allocator, size );
    memset( result, 0, size );
    return result;
  }

  SimpleBuddyAllocator* create_simple_buddy_allocator( s64 size, u32 suggestLeafSize )
  {
    u32 leafSize = max( suggestLeafSize, sizeof( SimpleBuddyAllocator::FreeNode ) );
    s32 maxLevel = 0;
    {
      s64 blockSize = s64( leafSize );
      while ( blockSize < size )
      {
        blockSize *= 2;
        ++maxLevel;
      }
    }

    u64 const sizeAtMaxLevel = u64( leafSize ) << maxLevel;
    u64 metaDataSize = 1 + ((sizeAtMaxLevel + (sizeAtMaxLevel >> 1) - 1) / (u64( leafSize ) * 8));

    u64 const deadSpaceOffset = sizeAtMaxLevel - size;
    s64 overheadSize = deadSpaceOffset + sizeof( SimpleBuddyAllocator ) + sizeof( SimpleBuddyAllocator::FreeNode ) * maxLevel + metaDataSize;
    if ( u64( overheadSize ) * 2 >= sizeAtMaxLevel )
    {
      BREAK;
      return nullptr;
    }

    char* allocation = (char*) bsp::platform->allocate_new_app_memory( size );
    SimpleBuddyAllocator* result = (SimpleBuddyAllocator*) allocation;

    //offset buffer pointer into unreachable space for non-power of 2 allocator sizes to make sure pointer math works
    result->buffer = allocation - deadSpaceOffset;
    result->freeNodesList = (SimpleBuddyAllocator::FreeNode*) (allocation + sizeof( SimpleBuddyAllocator ));
    result->leafSize = leafSize;
    result->maxLevel = maxLevel;
    result->metaData = ((u8*) result->freeNodesList) + sizeof( SimpleBuddyAllocator::FreeNode ) * result->maxLevel;
    result->metaDataDebugEnd = result->metaData + metaDataSize;

    memset( result->metaData, 0, metaDataSize );

    for ( s32 i = 0; i < result->maxLevel; ++i )
    {
      result->freeNodesList[i].next = &result->freeNodesList[i];
      result->freeNodesList[i].prev = &result->freeNodesList[i];
    }

    auto* allocator = result;
    s32 desiredLevel = get_level_for_size( result, overheadSize );
    u64 blockIndex = get_block_index_for_address_and_level( allocator, (char*) allocation, desiredLevel );

    for ( s32 i = desiredLevel; i < result->maxLevel; ++i )
    {
      block_state_xor( allocator, blockIndex );

      SimpleBuddyAllocator::FreeNode* node = get_node_for_block_index_and_level( allocator, blockIndex ^ 1, i );
      add_node( &allocator->freeNodesList[i], node );
      blockIndex >>= 1;
    }

    return result;
  }

  void destroy_buddy_allocator( SimpleBuddyAllocator* allocator )
  {
    bsp::platform->free_app_memory( allocator );
  }

  bool validate_buddy_allocator( SimpleBuddyAllocator* allocator )
  {
    bool result = true;

    // char* validBegin = allocator->buffer;
    // char* validEnd = allocator->buffer + get_size_for_level( allocator, allocator->maxLevel );
    // for ( s32 level = allocator->maxLevel; level; --level )
    // {
    //   auto* node = &allocator->freeNodesList[level];
    //   auto* startNode = node;
    //   while ( node->next != startNode )
    //   {
    //     node = node->next;
    //     char* begin = (char*) node;
    //     char* end = begin + get_size_for_level( allocator, level );

    //     if ( begin < validBegin || end > validEnd )
    //     {

    //     }

    //   }
    // }

    return result;
  }

};