#pragma once
#include "platform.h"

#include "common/basic_types.h"

#define WINDOW_RESOLUTION int2{ 1620, 720 }

struct win32_BackBuffer
{
  BITMAPINFO info;
  void* bmpBuffer;
  s32 bmpWidth;
  s32 bmpHeight;
  s32 pitch;
  s32 bytesPerPixel;
};

struct win32_WindowDimensions
{
  s32 width;
  s32 height;
};

struct win32_SoundOutput
{
  s32 samplesPerSecond;
  s32 bytesPerSample;
  s32 soundBufferSize;
  u32 runningSampleIndex;
  s32 writeSafetyBytes;
};

struct win32_DebugTimeMarker
{
  DWORD outputPlayCursor;
  DWORD outputWriteCursor;
  DWORD outputLocation;
  DWORD outputByteCount;
  DWORD eofPlayCursor;
  DWORD eofWriteCursor;
};

namespace stub
{
  void app_sample_sound( AppSampleSoundParameter& ) {}
  void app_on_load( AppOnLoadParameter& ) {}
  void app_tick( AppTickParameter& ) {}
  void app_render( AppRenderParameter& ) {}
  void app_receive_udp_packet( AppReceiveUDPPacketParameter& ) {}
};

using win32_app_sample_sound = void( AppSampleSoundParameter& );
using win32_app_on_load = void( AppOnLoadParameter& );
using win32_app_tick = void( AppTickParameter& );
using win32_app_render = void( AppRenderParameter& );
using win32_app_receive_udp_packet = void( AppReceiveUDPPacketParameter& );

struct App
{
  HMODULE dll;
  win32_app_sample_sound* sample_sound = stub::app_sample_sound;
  win32_app_on_load* on_load = stub::app_on_load;
  win32_app_tick* tick = stub::app_tick;
  win32_app_render* render = stub::app_render;
  win32_app_receive_udp_packet* receive_udp_packet = stub::app_receive_udp_packet;
};


enum APP_FLAG : u32
{
  NONE            = 0x0,
  APP_ACTIVATED   = 0x1,
  APP_DEACTIVATED = 0x2,
  APP_ACTIVATED_DEACTIVATED = APP_ACTIVATED | APP_DEACTIVATED,
};
DEFINE_ENUM_OPERATORS_U32( APP_FLAG )

struct IDirectSoundBuffer;
namespace win32
{
  struct GlobalData
  {
    s64                 performanceCounterFrequency;
    win32_BackBuffer    backBuffer;
    IDirectSoundBuffer* soundBuffer;

    u32                 running;
    APP_FLAG            frame_flags;

    App                 app_instances[2];
    AppData             appData;
    PlatformData        platformData;

    #if !BS_BUILD_RELEASE
    HANDLE              debugLogHandle;
    threading::atomic32 guard_currentDllIndex;
    threading::atomic32 guard_oldDllCanBeDiscarded;
    #endif
  };

  struct GlobalNetworkData
  {
    SOCKET              udpSocket;
    net::Connection     self;
    net::Connection     server;

    net::Connection     connections[APP_CLIENT_COUNT_MAX];
    threading::atomic32 connectionCount;
    threading::atomic32 guard_connections;
  };
};

namespace global
{
  global_variable win32::GlobalData win32Data;
  global_variable win32::GlobalNetworkData netData;
};

#if !BS_BUILD_RELEASE

#include "common/string.h"

constexpr u32 WIN32_MAX_DEBUG_MESSAGE_LENGTH = 512;

template<typename... Args> void win32_DebugLog( Args... args )
{
  char debugBuffer[WIN32_MAX_DEBUG_MESSAGE_LENGTH];
  s32 bytesToWrite = string_format( debugBuffer, args... ) - 1 /* ommit null */;
  if ( bytesToWrite > 0 )
  {
    if ( debugBuffer[bytesToWrite - 1] != '\n' )
    {
      debugBuffer[bytesToWrite++] = '\n';
      debugBuffer[bytesToWrite] = '\0';
    }

    #if BS_BUILD_DEBUG
    OutputDebugStringA( debugBuffer );
    #endif

    WriteFile( global::win32Data.debugLogHandle, debugBuffer, bytesToWrite, (LPDWORD) &bytesToWrite, 0 );
  }
}

#define _LOG_PLACEHOLDER( ... ) win32_DebugLog( __VA_ARGS__ )
#define log_if( condition, ... ) if ( condition ) _LOG_PLACEHOLDER( __VA_ARGS__ )

#else

#define _LOG_PLACEHOLDER( ... ) (0)
#define log_if( condition, ... )

#endif

#define log_info( ... )     _LOG_PLACEHOLDER( __VA_ARGS__ )
#define log_warning( ... )  _LOG_PLACEHOLDER( __VA_ARGS__ )
#define log_error( ... )    _LOG_PLACEHOLDER( __VA_ARGS__ )

#define log_todo( ... ) _LOG_PLACEHOLDER( "[TODO] ", __FILE__, " #", __LINE__, "\n - - - NOTE: ", __VA_ARGS__ )
