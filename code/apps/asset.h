#pragma once

#include <platform.h>




#pragma pack(push, 1)

struct AssetFileHeader
{
  u32 magicValue; //define this
  u32 version; //define this
};

#pragma pack(pop)




struct Asset
{
  // ??
};



enum AssetState : u32
{
  LOCKED,
  UNLOADED,
  LOADED,
  QUEUED
};



//atomiccompareexchange for checking whether asset is unloaded


/*

task
{
  completed?
  void* result.

}


work queue

entry completion count
next entry to do
entry count
semaphore handle

_mm_sfence() ?


*/
