#pragma once 


#ifndef BS_DEBUG

# define log_info_con_only( ... )
# define log_warning_con_only( ... )
# define log_error_con_only( ... )

# define log_info( ... )
# define log_warning( ... )
# define log_error( ... )

#else

#ifdef ERROR
#undef ERROR
#endif

# define log_info_con_only( ... )     _debug_log( bs::debug::DebugLogFlags::INFO, __VA_ARGS__ )
# define log_warning_con_only( ... )  _debug_log( bs::debug::DebugLogFlags::WARNING, __VA_ARGS__ )
# define log_error_con_only( ... )    _debug_log( bs::debug::DebugLogFlags::ERROR, __VA_ARGS__ )
# define log_info( ... )     _debug_log( bs::debug::DebugLogFlags::WRITE_TO_DEBUG_LOG_FILE | bs::debug::DebugLogFlags::INFO, __VA_ARGS__ )
# define log_warning( ... )  _debug_log( bs::debug::DebugLogFlags::WRITE_TO_DEBUG_LOG_FILE | bs::debug::DebugLogFlags::WARNING, __VA_ARGS__ )
# define log_error( ... )    _debug_log( bs::debug::DebugLogFlags::WRITE_TO_DEBUG_LOG_FILE | bs::debug::DebugLogFlags::ERROR, __VA_ARGS__ )

#endif

#ifdef _WIN32
#include <common/bscommon.h>
#include <intrin.h>
namespace bs
{
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
};
# define PROFILE_SCOPE( outCycles_u64 ) bs::debug::ProfilingObject dbg_tmp_profiler_object##outCycles_u64 { &outCycles_u64 }
#else
# define PROFILE_SCOPE( outCycles_u64 )
#endif

namespace bs
{
  namespace debug
  {
    constexpr u32 MAX_DEBUG_MESSAGE_LENGTH = 512;

    enum DebugLogFlags : u32
    {
      NONE                    = 0x0,
      INFO                    = 0x1,
      WARNING                 = 0x2,
      ERROR                   = 0x4,
      WRITE_TO_DEBUG_LOG_FILE = 0x8,
      SEND_TO_SERVER          = 0x10,
    };
    DEFINE_ENUM_OPERATORS_U32( DebugLogFlags );

    template<typename... Args> void _debug_log( DebugLogFlags flags, Args... args );
  };
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////inl//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


#include <platform/platform_callbacks.h>
#include <common/bsstring.h>
namespace bs
{
  namespace debug
  {
    template<typename... Args> void _debug_log( DebugLogFlags flags, Args... args )
    {
      char debugBuffer[MAX_DEBUG_MESSAGE_LENGTH];
      s32 bytesToWrite = bs::string_format( { debugBuffer, MAX_DEBUG_MESSAGE_LENGTH }, args... ) - 1 /* ommit null */;
      if ( bytesToWrite > 0 )
      {
        if ( debugBuffer[bytesToWrite - 1] != '\n' )
        {
          debugBuffer[bytesToWrite++] = '\n';
          debugBuffer[bytesToWrite] = '\0';
        }

        platform::debug_log( flags, debugBuffer, bytesToWrite );
      }
    }
  };
};