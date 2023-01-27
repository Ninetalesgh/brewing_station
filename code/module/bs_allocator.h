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
///////////////buddy allocator//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

namespace bsm
{

  struct BuddyAllocator;

  [[nodiscard]]
  BuddyAllocator* create_buddy_allocator( s64 size, s32 suggestLeafSize = 0 );
  void destroy_buddy_allocator( BuddyAllocator* allocator );

  [[nodiscard]]
  void* allocate( BuddyAllocator* allocator, s64 size );
  [[nodiscard]]
  void* allocate_to_zero( BuddyAllocator* allocator, s64 size );
  void free( BuddyAllocator* allocator, void* allocation );
  void free( BuddyAllocator* allocator, void* allocation, s64 size );

};

////////////////////////////////////////////////////////////////////////////

#include <intrin.h>
u32 INLINE count_trailing_zero( u32 value )
{
  unsigned long trailing_zero = 0;
  return u32( _BitScanForward( &trailing_zero, value ) ? trailing_zero : 32 );
}

u32 INLINE count_leading_zero( u32 value )
{
  unsigned long leading_zero = 0;
  return u32( _BitScanReverse( &leading_zero, value ) ? 31 - leading_zero : 32 );
}

namespace bsm
{
  struct BuddyAllocator
  {
    struct FreeNode
    {
      FreeNode* next;
      FreeNode* prev;
    };

    char* buffer;
    FreeNode* freeNodesList;
    u8* metaData;
    s64 leafSize;
    s32 maxLevel;

    u8* metaDataDebugEnd;
  };

  void add_node( BuddyAllocator::FreeNode* list, BuddyAllocator::FreeNode* newNode )
  {
    newNode->next = list;
    newNode->prev = list->prev;
    list->prev->next = newNode;
    list->prev = newNode;
  }

  void remove_node( BuddyAllocator::FreeNode* node )
  {
    node->next->prev = node->prev;
    node->prev->next = node->next;
  }

  INLINE s64 get_size_for_level( BuddyAllocator* allocator, s32 level )
  {
    return allocator->leafSize << level;
  }

  u64 get_block_index_for_address_and_level( BuddyAllocator* allocator, char* address, s32 level )
  {
    u64 offset = (u64( 1 ) << allocator->maxLevel) + (u64( address - allocator->buffer ) / allocator->leafSize);
    return offset >> level;
  }

  BuddyAllocator::FreeNode* get_node_for_block_index_and_level( BuddyAllocator* allocator, u64 blockIndex, s32 level )
  {
    u64 offset = (blockIndex << u64( level )) - (u64( 1 ) << u64( allocator->maxLevel ));
    return (BuddyAllocator::FreeNode*) (char*) (allocator->buffer + (offset * allocator->leafSize));
  }

  s32 get_level_for_size( BuddyAllocator* allocator, s64 size )
  {
    s32 resultLevel = 0;
    s64 blockSize = allocator->leafSize;
    while ( blockSize < size )
    {
      blockSize *= 2;
      ++resultLevel;
    }
    return resultLevel;
  }

  void block_state_unset( BuddyAllocator* allocator, u64 blockIndex )
  {
    blockIndex >>= 1;
    u8& data = allocator->metaData[blockIndex >> 3];
    assert( &data < allocator->metaDataDebugEnd );
    u8 const bit = 1 << (blockIndex % 8);
    data = data & ~bit;
  }

  void block_state_set( BuddyAllocator* allocator, u64 blockIndex )
  {
    blockIndex >>= 1;
    u8& data = allocator->metaData[blockIndex >> 3];
    assert( &data < allocator->metaDataDebugEnd );
    u8 const bit = 1 << (blockIndex % 8);
    data = data | bit;
  }

  bool block_state_xor( BuddyAllocator* allocator, u64 blockIndex )
  {
    blockIndex >>= 1;
    u8& data = allocator->metaData[blockIndex >> 3];
    assert( &data < allocator->metaDataDebugEnd );
    u8 const bit = 1 << (blockIndex % 8);
    data = data ^ bit;
    return data & bit;
  }

  bool block_state_get( BuddyAllocator* allocator, u64 blockIndex )
  {
    blockIndex >>= 1;
    u8& data = allocator->metaData[blockIndex >> 3];
    assert( &data < allocator->metaDataDebugEnd );
    u8 const bit = 1 << (blockIndex % 8);
    return data & bit;
  }

  bool block_state_get_lower( BuddyAllocator* allocator, u64 blockIndex )
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

  void block_state_xor_lower( BuddyAllocator* allocator, u64 blockIndex )
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

  bool debug_block_index_matches_level( BuddyAllocator* allocator, u64 blockIndex, s32 level )
  {
    u64 const controlIndex = (u64( 1 ) << (1 + allocator->maxLevel - level));
    return blockIndex < controlIndex&& blockIndex >= (controlIndex >> 1);
  }

  bool debug_node_matches_level( BuddyAllocator* allocator, BuddyAllocator::FreeNode* node, s32 level )
  {
    BuddyAllocator::FreeNode* expectedNode = &allocator->freeNodesList[level];
    BuddyAllocator::FreeNode* startNode = node;

    for ( ;; )
    {
      if ( node->next == expectedNode )
      {
        return true;
      }
      else if ( node->next == startNode || node->next == nullptr )
      {
        return false;
      }
      else
      {
        node = node->next;
      }
    }
  }

  void* allocate( BuddyAllocator* allocator, s64 size )
  {
    s32 desiredLevel = get_level_for_size( allocator, size );

    s32 listedLevel = desiredLevel;
    while ( (listedLevel < allocator->maxLevel) && (allocator->freeNodesList[listedLevel].next == &allocator->freeNodesList[listedLevel]) ) { ++listedLevel; }

    if ( (listedLevel >= allocator->maxLevel) )
    {
      BREAK;
      return nullptr; //no space for desired allocation? 
    }

    BuddyAllocator::FreeNode* desiredNode = allocator->freeNodesList[listedLevel].next;
    remove_node( desiredNode );

    u64 blockIndex = get_block_index_for_address_and_level( allocator, (char*) desiredNode, desiredLevel );

    //set block below for finder
    if ( desiredLevel || !(blockIndex & 1) )
    {
      assert( !block_state_get_lower( allocator, blockIndex ) );
      block_state_xor_lower( allocator, blockIndex );
    }

    for ( s32 i = desiredLevel; i < listedLevel; ++i )
    {
      BuddyAllocator::FreeNode* node = get_node_for_block_index_and_level( allocator, blockIndex ^ 1, i );
      assert( debug_block_index_matches_level( allocator, blockIndex ^ 1, i ) );
      add_node( &allocator->freeNodesList[i], node );

      assert( !block_state_get( allocator, blockIndex ) );
      block_state_xor( allocator, blockIndex );
      blockIndex >>= 1;
    }

    block_state_xor( allocator, blockIndex );

    return desiredNode;
  }

  s32 find_level_of_address_for_deallocation( BuddyAllocator* allocator, char* address )
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

  void free_at_level( BuddyAllocator* allocator, char* address, s32 level )
  {
    u64 blockIndex = get_block_index_for_address_and_level( allocator, address, level );
    assert( debug_block_index_matches_level( allocator, blockIndex, level ) );

    if ( !(blockIndex & 1) || level )
    {
      assert( block_state_get_lower( allocator, blockIndex ) );
      block_state_xor_lower( allocator, blockIndex );
    }

    for ( ; level < allocator->maxLevel; ++level )
    {
      if ( !block_state_xor( allocator, blockIndex ) )
      {
        BuddyAllocator::FreeNode* buddy = get_node_for_block_index_and_level( allocator, blockIndex ^ 1, level );
        assert( debug_node_matches_level( allocator, buddy, level ) );
        remove_node( buddy );
        blockIndex>>=1;
      }
      else
      {
        break;
      }
    }

    BuddyAllocator::FreeNode* newNode = get_node_for_block_index_and_level( allocator, blockIndex, level );
    assert( debug_block_index_matches_level( allocator, blockIndex, level ) );
    add_node( &allocator->freeNodesList[level], newNode );
  }

  void free( BuddyAllocator* allocator, void* allocation )
  {
    s32 level = find_level_of_address_for_deallocation( allocator, (char*) allocation );
    free_at_level( allocator, (char*) allocation, level );
  }

  void free( BuddyAllocator* allocator, void* allocation, s64 size )
  {
    s32 level = get_level_for_size( allocator, size );

    s32 debugLevel = find_level_of_address_for_deallocation( allocator, (char*) allocation );
    assert( level == debugLevel );

    free_at_level( allocator, (char*) allocation, level );
  }

  void* allocate_to_zero( BuddyAllocator* allocator, s64 size )
  {
    void* result = allocate( allocator, size );
    memset( result, 0, size );
    return result;
  }

  BuddyAllocator* create_buddy_allocator( s64 size, s32 suggestLeafSize )
  {
    char* allocation = (char*) bsp::platform->allocate_new_app_memory( size );
    BuddyAllocator* result = (BuddyAllocator*) allocation;

    //TODO account for non-power of 2 size, should be fine
    result->buffer = allocation;
    result->freeNodesList = (BuddyAllocator::FreeNode*) (allocation + sizeof( BuddyAllocator ));
    result->leafSize = max( suggestLeafSize, sizeof( BuddyAllocator::FreeNode ) );
    result->maxLevel = get_level_for_size( result, size );
    result->metaData = ((u8*) result->freeNodesList) + sizeof( BuddyAllocator::FreeNode ) * result->maxLevel;

    s64 metaDataSize = 1 + ((get_size_for_level( result, result->maxLevel ) - 1) / (result->leafSize * 8));
    metaDataSize = metaDataSize + ((metaDataSize - 1) / 2) + 1;
    result->metaDataDebugEnd = result->metaData + metaDataSize;

    s64 overheadSize = sizeof( BuddyAllocator ) + sizeof( BuddyAllocator::FreeNode ) * result->maxLevel + metaDataSize;
    if ( overheadSize * 2 >= size )
    {
      BREAK;
      bsp::platform->free_app_memory( allocation );
      return nullptr;
    }

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

      BuddyAllocator::FreeNode* node = get_node_for_block_index_and_level( allocator, blockIndex ^ 1, i );
      add_node( &allocator->freeNodesList[i], node );
      blockIndex >>= 1;
    }

    return result;
  }

  void destroy_buddy_allocator( BuddyAllocator* allocator )
  {
    bsp::platform->free_app_memory( allocator );
  }



  bool validate_buddy_allocator( BuddyAllocator* allocator )
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