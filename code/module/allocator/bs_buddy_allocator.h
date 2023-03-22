#pragma once

#include <platform/bs_platform.h>
#include <core/bsthread.h>
#include <common/bs_common.h>


namespace bsm
{

  struct BuddyAllocator;

  [[nodiscard]]
  BuddyAllocator* create_buddy_allocator( s64 size, u32 suggestLeafSize = 0 );
  void destroy_buddy_allocator( BuddyAllocator* allocator );

  [[nodiscard]]
  void* allocate( BuddyAllocator* allocator, s64 size );
  [[nodiscard]]
  void* allocate_to_zero( BuddyAllocator* allocator, s64 size );
  void free( BuddyAllocator* allocator, void* allocation );
  void free( BuddyAllocator* allocator, void* allocation, s64 size );

  //If you are planning to keep your allocation around for longer,
  //use this after allocating to have the buddy allocator waste less memory.
  void defragment_existing_allocation( BuddyAllocator* allocator, void* allocation, s64 size );

};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////cpp/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include <memory>

namespace bsm
{
  struct BuddyAllocator
  {
    struct FreeNode
    {
      FreeNode* next;
      FreeNode* prev;
    };

    struct MetaDataBlock
    {
      u8 data[64];
    };

    char* buffer;
    FreeNode* freeNodesList;
    MetaDataBlock* meta;
    u32 leafSize;
    s32 maxLevel;
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

  INLINE u64 get_size_for_level( BuddyAllocator* allocator, s32 level )
  {
    return u64( allocator->leafSize ) << level;
  }

  u64 get_block_index_for_address_and_level( BuddyAllocator* allocator, char* address, s32 level )
  {
    u64 offset = (u64( 1 ) << allocator->maxLevel) + (u64( address - allocator->buffer ) / u64( allocator->leafSize ));
    return offset >> level;
  }

  s32 get_level_for_block_index( BuddyAllocator* allocator, u64 blockIndex )
  {
    s32 result = allocator->maxLevel;
    while ( blockIndex>>=1 )
    {
      --result;
    }
    return result;
  }

  BuddyAllocator::FreeNode* get_node_for_block_index_and_level( BuddyAllocator* allocator, u64 blockIndex, s32 level )
  {
    u64 offset = (blockIndex << u64( level )) - (u64( 1 ) << u64( allocator->maxLevel ));
    char* const address = (allocator->buffer + (offset * u64( allocator->leafSize )));
    //make sure we're actually getting into our memory
    assert( address > (char*) allocator );
    return (BuddyAllocator::FreeNode*) address;
  }

  s32 get_level_for_size( BuddyAllocator* allocator, s64 size )
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

  u64 get_meta_block_count_for_max_level( s32 levelCount )
  {
    u64 result = 0;
    s32 iterations = levelCount / 9;
    while ( iterations-- )
    {
      result = 1 + (result << 9);
    }

    iterations = (levelCount % 9) + 1;
    result = 1 + (result << iterations);
    return result;
  }

  u64 get_offset_in_level( BuddyAllocator* allocator, u64 blockIndex, s32 level )
  {
    return blockIndex - (u64( 1 ) << (allocator->maxLevel - level));
  }

  s32 get_meta_origin_level( BuddyAllocator* allocator, s32 level )
  {
    return min( level + 8 - (level % 9), allocator->maxLevel );
  }

  u64 get_meta_index( BuddyAllocator* allocator, u64 blockIndex )
  {
    s32 level = get_level_for_block_index( allocator, blockIndex );
    s32 metaOriginLevel = get_meta_origin_level( allocator, level );

    u64 metaIndex = blockIndex >> (metaOriginLevel - level);

    u64 offsetInLevel = get_offset_in_level( allocator, metaIndex, metaOriginLevel );

    u64 metaIndexOffset = 0;
    s32 iterations = (allocator->maxLevel - metaOriginLevel) / 9;
    while ( iterations-- )
    {
      metaIndexOffset = 1 + (metaIndexOffset << 9);
    }

    return offsetInLevel + metaIndexOffset;
  }

  u64 get_local_block_index( BuddyAllocator* allocator, u64 blockIndex )
  {
    s32 level = get_level_for_block_index( allocator, blockIndex );
    s32 metaOriginLevel = get_meta_origin_level( allocator, level );
    u64 result = 1;
    for ( s32 i = level; i < metaOriginLevel; ++i )
    {
      result <<= 1;
      result |= (blockIndex & 1);
      blockIndex >>=1;
    }

    assert( result < 512 );
    return result;
  }

  //returns new state
  bool block_state_xor( BuddyAllocator* allocator, u64 blockIndex )
  {
    u64 metaIndex = get_meta_index( allocator, blockIndex );
    BuddyAllocator::MetaDataBlock& meta = allocator->meta[metaIndex];
    u64 localIndex = get_local_block_index( allocator, blockIndex );
    u8& data = meta.data[localIndex >> 3];
    assert( &data < (u8*) allocator );
    u8 const bit = 1 << (localIndex % 8);
    data = data ^ bit;
    return data & bit;
  }

  bool block_state_get( BuddyAllocator* allocator, u64 blockIndex )
  {
    u64 metaIndex = get_meta_index( allocator, blockIndex );
    BuddyAllocator::MetaDataBlock& meta = allocator->meta[metaIndex];
    u64 localIndex = get_local_block_index( allocator, blockIndex );
    u8& data = meta.data[localIndex >> 3];
    assert( &data < (u8*) allocator );
    u8 const bit = 1 << (localIndex % 8);
    return data & bit;
  }

  INLINE u64 get_defragmentation_index( u64 blockIndex )
  {
    return u64( 1 ) + (blockIndex << 2);
  }
  INLINE u64 get_right_child_block_index( u64 blockIndex )
  {
    return u64( 1 ) + (blockIndex << 1);
  }
  INLINE u64 get_left_child_block_index( u64 blockIndex )
  {
    return blockIndex << 1;
  }
  INLINE u64 get_parent_block_index( u64 blockIndex )
  {
    return blockIndex >> 1;
  }

  bool debug_block_index_matches_level( BuddyAllocator* allocator, u64 blockIndex, s32 level )
  {
    u64 const controlIndex = (u64( 1 ) << (1 + allocator->maxLevel - level));
    return blockIndex < controlIndex && blockIndex >= (controlIndex >> 1);
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

    //set control extra bit at lower level for finder
    assert( !block_state_get( allocator, blockIndex ) );
    block_state_xor( allocator, blockIndex );

    for ( s32 i = desiredLevel; i < listedLevel; ++i )
    {
      BuddyAllocator::FreeNode* node = get_node_for_block_index_and_level( allocator, blockIndex ^ 1, i );
      assert( debug_block_index_matches_level( allocator, blockIndex ^ 1, i ) );
      add_node( &allocator->freeNodesList[i], node );

      blockIndex >>= 1;
      assert( !block_state_get( allocator, blockIndex ) );
      block_state_xor( allocator, blockIndex );
    }

    blockIndex >>= 1;
    block_state_xor( allocator, blockIndex );

    return desiredNode;
  }

  s32 find_level_of_address_for_deallocation( BuddyAllocator* allocator, char* address )
  {
    s32 level = 0;
    u64 blockIndex = get_block_index_for_address_and_level( allocator, address, 0 );

    while ( !(blockIndex & 1) && !block_state_get( allocator, blockIndex ) )
    {
      ++level;
      blockIndex >>= 1;
    }

    return level;
  }

  void* allocate_to_zero( BuddyAllocator* allocator, s64 size )
  {
    void* result = allocate( allocator, size );
    memset( result, 0, size );
    return result;
  }

  BuddyAllocator* create_buddy_allocator( s64 size, u32 suggestLeafSize )
  {
    u32 leafSize = max( suggestLeafSize, sizeof( BuddyAllocator::FreeNode ) );
    s32 maxLevel = 0;

    u64 sizeAtMaxLevel = s64( leafSize );
    while ( sizeAtMaxLevel < u64( size ) )
    {
      sizeAtMaxLevel *= 2;
      ++maxLevel;
    }

    u64 metaDataSize = get_meta_block_count_for_max_level( maxLevel ) * sizeof( BuddyAllocator::MetaDataBlock );

    u64 const deadSpaceOffset = sizeAtMaxLevel - u64( size );
    s64 overheadSize = deadSpaceOffset + sizeof( BuddyAllocator ) + sizeof( BuddyAllocator::FreeNode ) * maxLevel + metaDataSize;
    if ( u64( overheadSize ) * 2 >= sizeAtMaxLevel )
    {
      BREAK;
      return nullptr;
    }

    char* allocation = (char*) bsp::platform->allocate_new_app_memory( size );

    BuddyAllocator* result = (BuddyAllocator*) (allocation + metaDataSize);

    //offset buffer pointer into unreachable space for non-power of 2 allocator sizes to make sure pointer math works
    result->buffer = allocation - deadSpaceOffset;
    result->freeNodesList = (BuddyAllocator::FreeNode*) (((char*) result) + sizeof( BuddyAllocator ));
    result->leafSize = leafSize;
    result->maxLevel = maxLevel;
    result->meta = (BuddyAllocator::MetaDataBlock*) allocation;
    memset( result->meta, 0, metaDataSize );

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

    defragment_existing_allocation( allocator, result->buffer, overheadSize );
    return result;
  }

  void destroy_buddy_allocator( BuddyAllocator* allocator )
  {
    bsp::platform->free_app_memory( allocator->meta );
  }

  void defragment_existing_allocation( BuddyAllocator* allocator, void* allocation, s64 size )
  {
    s32 level = get_level_for_size( allocator, size );

    if ( level < 3 )
    {
      //we can't split, we leave
      BREAK;
      return;
    }

    //round up to next leaf size
    size += allocator->leafSize - (((size - 1) % allocator->leafSize) + 1);
    s64 fullSizeAtLevel = (s64) get_size_for_level( allocator, level );
    if ( size > fullSizeAtLevel - allocator->leafSize )
    {
      //this has no relevant inner fragmentation
      BREAK;
      return;
    }

    u64 blockIndex = get_block_index_for_address_and_level( allocator, (char*) allocation, level );
    u64 leftIndex = get_left_child_block_index( blockIndex );
    u64 rightIndex = get_right_child_block_index( blockIndex );
    u64 defragmentationIndex = get_defragmentation_index( leftIndex );

    if ( !block_state_get( allocator, blockIndex ) )
    {
      //make sure it was indicated as split.
      assert( !block_state_get( allocator, defragmentationIndex ) );
      //make sure the left child is properly marked, which means this was already tightly fit
      assert( block_state_get( allocator, leftIndex ) );
      return;
    }

    assert( !block_state_get( allocator, defragmentationIndex ) );
    block_state_xor( allocator, defragmentationIndex );

    u64 controlBlockIndex = leftIndex;
    while ( level-- )
    {
      s64 levelSize = get_size_for_level( allocator, level );
      if ( levelSize < size )
      {
        size -= levelSize;

        //mark this level appended to the allocation in the control branch
        assert( !block_state_get( allocator, controlBlockIndex ) );
        block_state_xor( allocator, controlBlockIndex );

        //continue with right branch
        leftIndex = get_left_child_block_index( rightIndex );
        rightIndex = get_right_child_block_index( rightIndex );
      }
      else
      {
        //free right block
        BuddyAllocator::FreeNode* node = get_node_for_block_index_and_level( allocator, rightIndex, level );
        add_node( &allocator->freeNodesList[level], node );

        //mark parent split
        assert( !block_state_get( allocator, leftIndex >> 1 ) );
        block_state_xor( allocator, leftIndex >> 1 );

        if ( size == levelSize )
        {
          block_state_xor( allocator, controlBlockIndex );
          break;
        }

        //continue with left branch
        rightIndex = get_right_child_block_index( leftIndex );
        leftIndex = get_left_child_block_index( leftIndex );
      }

      controlBlockIndex = get_right_child_block_index( controlBlockIndex );
    }
  }

  void free_defragmented_block_at_level( BuddyAllocator* allocator, u64 blockIndex, s32 level )
  {
    u64 defragmentationIndex = get_defragmentation_index( blockIndex );
    assert( block_state_get( allocator, defragmentationIndex ) );
    assert( block_state_get( allocator, blockIndex ) );
    assert( block_state_get( allocator, get_parent_block_index( blockIndex ) ) );

    block_state_xor( allocator, defragmentationIndex );

    u64 controlIndex = blockIndex;
    s32 controlLevel = level;

    blockIndex = blockIndex ^ 1;

    while ( controlLevel-- )
    {
      controlIndex = get_right_child_block_index( controlIndex );
      if ( block_state_get( allocator, controlIndex ) )
      {
        blockIndex = get_right_child_block_index( blockIndex );
        block_state_xor( allocator, controlIndex );
      }
      else
      {
        blockIndex = get_left_child_block_index( blockIndex );
      }
    }

    bool doMerge = false;
    while ( ++controlLevel < level )
    {
      bool isPart = blockIndex & 1;
      blockIndex = (blockIndex | 1) ^ 1;
      if ( isPart )
      {
        if ( !doMerge )
        {
          if ( block_state_xor( allocator, blockIndex >> 1 ) )
          {
            //buddy is not free
            BuddyAllocator::FreeNode* newNode = get_node_for_block_index_and_level( allocator, blockIndex ^ 1, controlLevel );
            add_node( &allocator->freeNodesList[controlLevel], newNode );

            doMerge = false;
          }
          else
          {
            //buddy is free, this should only ever be possible at the beginning
            BuddyAllocator::FreeNode* buddy = get_node_for_block_index_and_level( allocator, blockIndex ^ 1, controlLevel );
            remove_node( buddy );
            doMerge = true;
          }
        }
      }
      else
      {
        if ( doMerge )
        {
          if ( block_state_xor( allocator, blockIndex >> 1 ) )
          {
            //buddy is not free
            doMerge = false;
          }
          else
          {
            //buddy is free, remove it and continue
            BuddyAllocator::FreeNode* buddy = get_node_for_block_index_and_level( allocator, blockIndex ^ 1, controlLevel );
            remove_node( buddy );
          }
        }
      }

      blockIndex = get_parent_block_index( blockIndex );
    }

    if ( doMerge )
    {
      //add the largest block too, this will just get merged right away, but this way we can use the existing free function
      BuddyAllocator::FreeNode* newNode = get_node_for_block_index_and_level( allocator, blockIndex | 1, controlLevel );
      add_node( &allocator->freeNodesList[controlLevel], newNode );
    }
    else
    {
      //we weren't able to free all the fragments in the block, unset the parent and we're good to go.
      block_state_xor( allocator, get_parent_block_index( blockIndex ) );
    }
  }

  void free_block_at_level( BuddyAllocator* allocator, u64 blockIndex, s32 level )
  {
    assert( debug_block_index_matches_level( allocator, blockIndex, level ) );
    assert( block_state_get( allocator, blockIndex ) );

    // control bit
    block_state_xor( allocator, blockIndex );

    //merge
    for ( ; level < allocator->maxLevel; ++level )
    {
      //see if buddy is mergable
      if ( !block_state_xor( allocator, get_parent_block_index( blockIndex ) ) )
      {
        BuddyAllocator::FreeNode* buddy = get_node_for_block_index_and_level( allocator, blockIndex ^ 1, level );
        remove_node( buddy );
        blockIndex = get_parent_block_index( blockIndex );
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

  void free( BuddyAllocator* allocator, void* allocation, s64 size )
  {
    s32 level = get_level_for_size( allocator, size );
    u64 blockIndex = get_block_index_for_address_and_level( allocator, (char*) allocation, level );

    if ( level >= 3 )
    {
      u64 leftIndex = get_left_child_block_index( blockIndex );
      u64 defragmentationIndex = get_defragmentation_index( leftIndex );

      if ( block_state_get( allocator, defragmentationIndex ) )
      {
        //this allocation has been defragmented, we go down a level
        --level;
        blockIndex = leftIndex;

        //clean up defragmentation before attempting to free & merge up to higher levels
        free_defragmented_block_at_level( allocator, blockIndex, level );
      }
    }

    free_block_at_level( allocator, blockIndex, level );
  }

  void free( BuddyAllocator* allocator, void* allocation )
  {
    s32 level = find_level_of_address_for_deallocation( allocator, (char*) allocation );
    u64 blockIndex = get_block_index_for_address_and_level( allocator, (char*) allocation, level );

    if ( level >= 2 )
    {
      u64 defragmentationIndex = get_defragmentation_index( blockIndex );

      if ( block_state_get( allocator, defragmentationIndex ) )
      {
        //clean up defragmentation before attempting to free & merge up to higher levels
        free_defragmented_block_at_level( allocator, blockIndex, level );
      }
    }

    free_block_at_level( allocator, blockIndex, level );
  }
};