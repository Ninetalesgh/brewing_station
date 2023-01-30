#pragma once


//BA2 is attempting to make this thread safe and block indexing more cache friendly



#include <platform/bs_platform.h>
#include <core/bsthread.h>
#include <common/bs_common.h>


namespace bsm
{

  struct BuddyAllocator2;

  [[nodiscard]]
  BuddyAllocator2* create_buddy_allocator2( s64 size, u32 suggestLeafSize = 0 );
  void destroy_buddy_allocator( BuddyAllocator2* allocator );

  [[nodiscard]]
  void* allocate( BuddyAllocator2* allocator, s64 size );
  [[nodiscard]]
  void* allocate_to_zero( BuddyAllocator2* allocator, s64 size );
  void free( BuddyAllocator2* allocator, void* allocation );
  void free( BuddyAllocator2* allocator, void* allocation, s64 size );

};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////cpp/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include <memory>

namespace bsm
{
  struct BuddyAllocator2
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

  void add_node( BuddyAllocator2::FreeNode* list, BuddyAllocator2::FreeNode* newNode )
  {
    newNode->next = list;
    newNode->prev = list->prev;
    list->prev->next = newNode;
    list->prev = newNode;
  }

  void remove_node( BuddyAllocator2::FreeNode* node )
  {
    node->next->prev = node->prev;
    node->prev->next = node->next;
  }

  INLINE u64 get_size_for_level( BuddyAllocator2* allocator, s32 level )
  {
    return u64( allocator->leafSize ) << level;
  }

  u64 get_block_index_for_address_and_level( BuddyAllocator2* allocator, char* address, s32 level )
  {
    u64 offset = (u64( 1 ) << allocator->maxLevel) + (u64( address - allocator->buffer ) / u64( allocator->leafSize ));
    return offset >> level;
  }

  s32 get_level_for_block_index( BuddyAllocator2* allocator, u64 blockIndex )
  {
    s32 result = allocator->maxLevel;
    while ( blockIndex>>=1 )
    {
      --result;
    }
    return result;
  }

  BuddyAllocator2::FreeNode* get_node_for_block_index_and_level( BuddyAllocator2* allocator, u64 blockIndex, s32 level )
  {
    u64 offset = (blockIndex << u64( level )) - (u64( 1 ) << u64( allocator->maxLevel ));
    char* const address = (allocator->buffer + (offset * u64( allocator->leafSize )));
    //make sure we're actually getting into our memory
    assert( address > (char*) allocator );
    return (BuddyAllocator2::FreeNode*) address;
  }

  s32 get_level_for_size( BuddyAllocator2* allocator, s64 size )
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

  u64 get_meta_count_for_level_count( s32 levelCount )
  {
    u64 result = 0;
    s32 iterations = levelCount / 9;
    while ( iterations-- )
    {
      result = 1 + (result << 9);
    }

    iterations = (levelCount % 9) + 1;
    result = 1 + (result << iterations);    return result;
  }

  u64 get_offset_in_level( BuddyAllocator2* allocator, u64 blockIndex, s32 level )
  {
    return blockIndex - (u64( 1 ) << (allocator->maxLevel - level));
  }

  s32 get_meta_level( BuddyAllocator2* allocator, s32 level )
  {
    return (allocator->maxLevel - level) / 9;
  }

  s32 get_meta_origin_level( BuddyAllocator2* allocator, s32 level )
  {
    return min( level + 8 - (level % 9), allocator->maxLevel );
  }

  u64 get_meta_index( BuddyAllocator2* allocator, u64 blockIndex )
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

  u64 get_local_block_index( BuddyAllocator2* allocator, u64 blockIndex )
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

  bool block_state_new_xor( BuddyAllocator2* allocator, u64 blockIndex )
  {
    u64 metaIndex = get_meta_index( allocator, blockIndex );
    BuddyAllocator2::MetaDataBlock& meta = allocator->meta[metaIndex];
    u64 localIndex = get_local_block_index( allocator, blockIndex );
    u8& data = meta.data[localIndex >> 3];
    assert( &data < (u8*) allocator );
    u8 const bit = 1 << (localIndex % 8);
    data = data ^ bit;
    return data & bit;
  }

  bool block_state_new_get( BuddyAllocator2* allocator, u64 blockIndex )
  {
    u64 metaIndex = get_meta_index( allocator, blockIndex );
    BuddyAllocator2::MetaDataBlock& meta = allocator->meta[metaIndex];
    u64 localIndex = get_local_block_index( allocator, blockIndex );
    u8& data = meta.data[localIndex >> 3];
    assert( &data < (u8*) allocator );
    u8 const bit = 1 << (localIndex % 8);
    return data & bit;
  }

  bool debug_block_index_matches_level( BuddyAllocator2* allocator, u64 blockIndex, s32 level )
  {
    u64 const controlIndex = (u64( 1 ) << (1 + allocator->maxLevel - level));
    return blockIndex < controlIndex&& blockIndex >= (controlIndex >> 1);
  }

  void* allocate( BuddyAllocator2* allocator, s64 size )
  {
    s32 desiredLevel = get_level_for_size( allocator, size );

    s32 listedLevel = desiredLevel;
    while ( (listedLevel < allocator->maxLevel) && (allocator->freeNodesList[listedLevel].next == &allocator->freeNodesList[listedLevel]) ) { ++listedLevel; }

    if ( (listedLevel >= allocator->maxLevel) )
    {
      BREAK;
      return nullptr; //no space for desired allocation? 
    }

    BuddyAllocator2::FreeNode* desiredNode = allocator->freeNodesList[listedLevel].next;
    remove_node( desiredNode );

    u64 blockIndex = get_block_index_for_address_and_level( allocator, (char*) desiredNode, desiredLevel );

    //set control extra bit at lower level for finder
    assert( !block_state_new_get( allocator, blockIndex ) );
    block_state_new_xor( allocator, blockIndex );
    // if ( desiredLevel || !(blockIndex & 1) )
    // {
    //   assert( !block_st ate_get_lower( allocator, blockIndex ) );
    //   block_stat e_xor_lower( allocator, blockIndex );
    // }


    for ( s32 i = desiredLevel; i < listedLevel; ++i )
    {
      BuddyAllocator2::FreeNode* node = get_node_for_block_index_and_level( allocator, blockIndex ^ 1, i );
      assert( debug_block_index_matches_level( allocator, blockIndex ^ 1, i ) );
      add_node( &allocator->freeNodesList[i], node );

      blockIndex >>= 1;
      assert( !block_state_new_get( allocator, blockIndex ) );
      block_state_new_xor( allocator, blockIndex );
    }

    blockIndex >>= 1;
    block_state_new_xor( allocator, blockIndex );

    return desiredNode;
  }

  s32 find_level_of_address_for_deallocation( BuddyAllocator2* allocator, char* address )
  {
    s32 level = 0;
    u64 blockIndex = get_block_index_for_address_and_level( allocator, address, 0 );

    while ( !(blockIndex & 1) )
    {
      if ( block_state_new_get( allocator, blockIndex ) )
      {
        break;
      }
      ++level;

      blockIndex >>= 1;
    }

    return level;
  }

  void free_at_level( BuddyAllocator2* allocator, char* address, s32 level )
  {
    u64 blockIndex = get_block_index_for_address_and_level( allocator, address, level );
    assert( debug_block_index_matches_level( allocator, blockIndex, level ) );

    // control extra bit at lowest level
    block_state_new_xor( allocator, blockIndex );
    // if ( !(blockIndex & 1) || level )
    // {
    //   assert( block_s tate_get_lower( allocator, blockIndex ) );
    //   block_state_ xor_lower( allocator, blockIndex );
    // }

    for ( ; level < allocator->maxLevel; ++level )
    {
      if ( !block_state_new_xor( allocator, blockIndex >> 1 ) )
      {
        BuddyAllocator2::FreeNode* buddy = get_node_for_block_index_and_level( allocator, blockIndex ^ 1, level );
        remove_node( buddy );
        blockIndex>>=1;
      }
      else
      {
        break;
      }
    }

    BuddyAllocator2::FreeNode* newNode = get_node_for_block_index_and_level( allocator, blockIndex, level );
    assert( debug_block_index_matches_level( allocator, blockIndex, level ) );
    add_node( &allocator->freeNodesList[level], newNode );
  }

  void free( BuddyAllocator2* allocator, void* allocation )
  {
    s32 level = find_level_of_address_for_deallocation( allocator, (char*) allocation );
    free_at_level( allocator, (char*) allocation, level );
  }

  void free( BuddyAllocator2* allocator, void* allocation, s64 size )
  {
    s32 level = get_level_for_size( allocator, size );

    s32 debugLevel = find_level_of_address_for_deallocation( allocator, (char*) allocation );
    assert( level == debugLevel );

    free_at_level( allocator, (char*) allocation, level );
  }

  void* allocate_to_zero( BuddyAllocator2* allocator, s64 size )
  {
    void* result = allocate( allocator, size );
    memset( result, 0, size );
    return result;
  }

  BuddyAllocator2* create_buddy_allocator2( s64 size, u32 suggestLeafSize )
  {
    u32 leafSize = max( suggestLeafSize, 64UL );
    s32 maxLevel = 0;

    u64 sizeAtMaxLevel = s64( leafSize );
    while ( sizeAtMaxLevel < u64( size ) )
    {
      sizeAtMaxLevel *= 2;
      ++maxLevel;
    }

    u64 metaDataSize = get_meta_count_for_level_count( maxLevel ) * sizeof( BuddyAllocator2::MetaDataBlock );

    u64 const deadSpaceOffset = sizeAtMaxLevel - u64( size );
    s64 overheadSize = deadSpaceOffset + sizeof( BuddyAllocator2 ) + sizeof( BuddyAllocator2::FreeNode ) * maxLevel + metaDataSize;
    if ( u64( overheadSize ) * 2 >= sizeAtMaxLevel )
    {
      BREAK;
      return nullptr;
    }

    char* allocation = (char*) bsp::platform->allocate_new_app_memory( size );

    BuddyAllocator2* result = (BuddyAllocator2*) (allocation + metaDataSize);

    //offset buffer pointer into unreachable space for non-power of 2 allocator sizes to make sure pointer math works
    result->buffer = allocation - deadSpaceOffset;
    result->freeNodesList = (BuddyAllocator2::FreeNode*) (((char*) result) + sizeof( BuddyAllocator2 ));
    result->leafSize = leafSize;
    result->maxLevel = maxLevel;
    result->meta = (BuddyAllocator2::MetaDataBlock*) allocation;
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
      block_state_new_xor( allocator, blockIndex );

      BuddyAllocator2::FreeNode* node = get_node_for_block_index_and_level( allocator, blockIndex ^ 1, i );
      add_node( &allocator->freeNodesList[i], node );
      blockIndex >>= 1;
    }

    return result;
  }

  void destroy_buddy_allocator( BuddyAllocator2* allocator )
  {
    bsp::platform->free_app_memory( allocator->meta );
  }


};