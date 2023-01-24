#pragma once

#ifdef _WIN32
#include <common/bs_common.h>
#include <intrin.h>
namespace bs
{
  namespace debug
  {
    class ProfilingObject
    {
    public:
      ProfilingObject( u64* output ): output( output ) { *output =  __rdtsc(); }
      ~ProfilingObject() { *output = __rdtsc() - *output; }
    private:
      u64* output;
    };
  };
};
# define PROFILE_SCOPE( outCycles_u64 ) bs::debug::ProfilingObject dbg_tmp_profiler_object##outCycles_u64 { &outCycles_u64 }
#else
# define PROFILE_SCOPE( outCycles_u64 )
#endif