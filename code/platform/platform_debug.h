#pragma once 


#ifndef BS_DEBUG

# define BS_PROFILE_SCOPE( outCycles_u64 )

# define log_info_con_only( contextName, ... )
# define log_warning_con_only( contextName, ... )
# define log_error_con_only( contextName, ... )

# define log_info( contextName, ... )
# define log_warning( contextName, ... )
# define log_error( contextName, ... )

#else

# define log_info_con_only( contextName, ... )     _debug_log( platform::debug::DebugLogFlags::INFO, contextName, __VA_ARGS__ )
# define log_warning_con_only( contextName, ... )  _debug_log( platform::debug::DebugLogFlags::WARNING, contextName, __VA_ARGS__ )
# define log_error_con_only( contextName, ... )    _debug_log( platform::debug::DebugLogFlags::ERROR, contextName, __VA_ARGS__ )
# define log_info( contextName, ... )     _debug_log( platform::debug::DebugLogFlags::WRITE_TO_DEBUG_LOG_FILE | platform::debug::DebugLogFlags::INFO, contextName, __VA_ARGS__ )
# define log_warning( contextName, ... )  _debug_log( platform::debug::DebugLogFlags::WRITE_TO_DEBUG_LOG_FILE | platform::debug::DebugLogFlags::WARNING, contextName, __VA_ARGS__ )
# define log_error( contextName, ... )    _debug_log( platform::debug::DebugLogFlags::WRITE_TO_DEBUG_LOG_FILE | platform::debug::DebugLogFlags::ERROR, contextName, __VA_ARGS__ )


#ifdef _WIN32
#include <common/bscommon.h>
#include <intrin.h>
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
# define PROFILE_SCOPE( outCycles_u64 ) debug::ProfilingObject dbg_tmp_profiler_object##outCycles_u64 { &outCycles_u64 }
#else
# define PROFILE_SCOPE( outCycles_u64 )
#endif

#include <common/bsstring.h>

namespace platform
{
  namespace debug
  {
    constexpr u32 MAX_DEBUG_MESSAGE_LENGTH = 512;

    enum DebugLogFlags : u32
    {
      NONE = 0x0,
      INFO = 0x1,
      WARNING = 0x2,
      ERROR = 0x4,
      WRITE_TO_DEBUG_LOG_FILE = 0x8,
      SEND_TO_SERVER = 0x10,
    };
    DEFINE_ENUM_OPERATORS_U32( DebugLogFlags );
    using debug_log = void( DebugLogFlags, char const*, s32 );

    namespace global
    {
      static debug_log* ptr_debug_log;
    };

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

        global::ptr_debug_log( flags, debugBuffer, bytesToWrite );
      }
    }

    struct PrmRegisterDebugCallbacks
    {
      debug_log* debug_log;
    };
    extern "C" void register_debug_callbacks( PrmRegisterDebugCallbacks );
  };
};

#endif