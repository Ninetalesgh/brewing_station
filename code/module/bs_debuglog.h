#pragma once

#include <common/bs_common.h>


# define log_info_con_only( ... )     bsm::_debug_log( bsm::DebugLogFlags::INFO, __VA_ARGS__ )
# define log_warning_con_only( ... )  bsm::_debug_log( bsm::DebugLogFlags::WARNING, __VA_ARGS__ )
# define log_error_con_only( ... )    bsm::_debug_log( bsm::DebugLogFlags::ERROR, __VA_ARGS__ )
# define log_info( ... )     bsm::_debug_log( bsm::DebugLogFlags::WRITE_TO_DEBUG_LOG_FILE | bsm::DebugLogFlags::INFO, __VA_ARGS__ )
# define log_warning( ... )  bsm::_debug_log( bsm::DebugLogFlags::WRITE_TO_DEBUG_LOG_FILE | bsm::DebugLogFlags::WARNING, __VA_ARGS__ )
# define log_error( ... )    bsm::_debug_log( bsm::DebugLogFlags::WRITE_TO_DEBUG_LOG_FILE | bsm::DebugLogFlags::ERROR, __VA_ARGS__ )




namespace bsm
{

  enum DebugLogFlags: u32
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

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////inl//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include <platform/bs_platform.h>
#include <common/bs_string.h>

namespace bsm
{

  template<typename... Args> void _debug_log( DebugLogFlags flags, Args... args )
  {
    s32 const MAX_DEBUG_MESSAGE_LENGTH = 8192;
    char debugBuffer[MAX_DEBUG_MESSAGE_LENGTH];
    s32 bytesToWrite = bs::string_format( debugBuffer, MAX_DEBUG_MESSAGE_LENGTH, args... ) - 1 /* ommit null */;
    if ( bytesToWrite > 0 )
    {
      if ( debugBuffer[bytesToWrite - 1] != '\n' )
      {
        debugBuffer[bytesToWrite++] = '\n';
        debugBuffer[bytesToWrite] = '\0';
      }

      bsp::platform->debug_log( flags, debugBuffer, bytesToWrite );
    }
  }
};