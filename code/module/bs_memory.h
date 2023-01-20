DEPRECATED
#pragma once


#include <platform/bs_platform.h>
#include <common/bscommon.h>


namespace bs
{

  struct StackAllocator
  {

    StackAllocator()
      : writer( nullptr )
      , next( nullptr )
    {}

    char* push( s32 size );

    char* pop();

    char dataBlock[MaxAllocationSize];
    char* writer;
    StackAllocator<MaxAllocationSize>* next;
  };


  [[nodiscard]]
  StackAllocator* create_stack_allocator( s32 maxAllocationSize );





};