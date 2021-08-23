#pragma once

#include <platform.h>





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