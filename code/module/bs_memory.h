DEPRECATED
#pragma once


#include <platform/bs_platform.h>
#include <common/bs_common.h>


namespace bs
{
  //for memory blocks larger than 4GB
  struct GeneralAllocator64
  {
    void* allocate( s64 size, u32 alignment = 16 ) { return nullptr; }
    void deallocate( void* ) {}
    char* bufferBegin;
    char* bufferEnd;
    struct Entry
    {
      char* begin;
      char* end;
    };
    Entry* lastEntry;
    Entry* nextEntry;
  };

  [[nodiscard]]
  GeneralAllocator64* create_general_allocator64( void* memoryBlock, s64 sizeOfBlockInBytes );


  //for memory blocks smaller than 4GB
  struct GeneralAllocator32
  {
    void* allocate( u32 size, u32 alignment = 16 );
    void deallocate( void* );
    char* bufferBegin;
    char* bufferEnd;
    struct Entry
    {
      u32 begin;
      u32 end;
    };
    Entry* lastEntry;
    Entry* nextEntry;
  };

  [[nodiscard]]
  GeneralAllocator32* create_general_allocator32( void* memoryBlock, u32 sizeOfBlockInBytes );


};


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////cpp/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


namespace bs
{
  void* GeneralAllocator32::allocate( u32 size, u32 alignment = 16 )
  {

    char* result = nullptr;

    u32 expectedOffset = lastEntry->end;



    current += ((u64) current) & (alignment - 1);
  }

  void GeneralAllocator32::deallocate( void* )
  {

  }

  GeneralAllocator32* create_general_allocator32( void* memoryBlock, u32 sizeOfBlockInBytes )
  {
    if ( sizeOfBlockInBytes < 1024 )
    {
      BREAK; //that's a bit of a small block to be allocating into, isn't it?
    }
    char* allocation = (char*) memoryBlock;

    GeneralAllocator64* result = (GeneralAllocator64*) allocation;
    result->bufferBegin = allocation;
    result->bufferEnd = allocation + sizeOfBlockInBytes;

    result->lastEntry = (GeneralAllocator64::Entry*) result->bufferEnd;
    result->nextEntry = result->lastEntry - 1;
  }









  GeneralAllocator64* create_general_allocator64( void* memoryBlock, s64 sizeOfBlockInBytes )
  {
    if ( sizeOfBlockInBytes < 1024 )
    {
      BREAK; //that's a bit of a small block to be allocating into, isn't it?
    }
    char* allocation = (char*) memoryBlock;

    GeneralAllocator64* result = (GeneralAllocator64*) allocation;
    result->bufferBegin = allocation;
    result->bufferEnd = allocation + sizeOfBlockInBytes;

    result->lastEntry = (GeneralAllocator64::Entry*) result->bufferEnd;
    result->nextEntry = result->lastEntry - 1;
  }



};


};
