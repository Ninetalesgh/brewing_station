#pragma once


#if !BS_BUILD_RELEASE 

#if defined(_WIN32)

#include <intrin.h>
#include "common/basic_types.h"

namespace debug
{
  class ProfilingObject
  {
  public:
    ProfilingObject( u64* output ) : output( output ) { *output =  __rdtsc(); }
    ~ProfilingObject() { *output = __rdtsc() - *output; }
  private:
    u64* output;
  };
};

#define PROFILE_SCOPE(outCycles_u64) debug::ProfilingObject dbg_tmp_profiler_object##outCycles_u64 { &outCycles_u64 }

#endif

#else
#define PROFILE_SCOPE(outCycles_u64)
#endif
