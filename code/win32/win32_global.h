#pragma once

//ONLY for brewing_station_platform.cpp build
#include <platform/platform.h>
#include <platform/platform_callbacks_internal.h>
#include <core/bsthread.h>
#include <common/bscommon.h>

#include <windows.h>
#ifdef ERROR
# undef ERROR
#endif

#ifdef min
# undef min
#endif
#ifdef max
# undef max
#endif
#include <Xinput.h>

namespace win32
{
  using xInputGetState = DWORD WINAPI( DWORD, XINPUT_STATE* );
  using xInputSetState = DWORD WINAPI( DWORD, XINPUT_VIBRATION* );

  namespace stub
  {
    void app_sample_sound( bs::PrmAppSampleSound ) {}
    void app_on_load( bs::PrmAppOnLoad ) {}
    void app_tick( bs::PrmAppTick ) {}
    void app_render( bs::PrmAppRender ) {}
    void app_receive_udp_packet( bs::PrmAppReceiveUDPPacket ) {}
    void app_register_callbacks( platform::Callbacks ) {}
  };

  using  win32_app_sample_sound = void( bs::PrmAppSampleSound );
  using  win32_app_on_load = void( bs::PrmAppOnLoad );
  using  win32_app_tick = void( bs::PrmAppTick );
  using  win32_app_render = void( bs::PrmAppRender );
  using  win32_app_receive_udp_packet = void( bs::PrmAppReceiveUDPPacket );
  using  win32_app_register_callbacks = void( platform::Callbacks );

  struct AppDll
  {
    HMODULE dll;
    win32_app_sample_sound* sample_sound = stub::app_sample_sound;
    win32_app_on_load* on_load = stub::app_on_load;
    win32_app_tick* tick = stub::app_tick;
    win32_app_render* render = stub::app_render;
    win32_app_receive_udp_packet* receive_udp_packet = stub::app_receive_udp_packet;
    win32_app_register_callbacks* register_callbacks = stub::app_register_callbacks;
  };

  struct Window
  {
    HWND handle;
    int2 pos;
    int2 size;
  };
};

namespace bs
{
  namespace font { struct GlyphTable; struct GlyphSheet; };
};
namespace global
{
  constexpr u64 APP_MEMORY_SIZE = GigaBytes( 1 );
  constexpr u32 SYNCED_THREAD_COUNT = 8;
  constexpr u32 ASYNC_THREAD_COUNT = 2;
  static s64 performanceCounterFrequency;
  float sleepMsSubtraction;

  static win32::xInputGetState* xInputGetState;
  static win32::xInputSetState* xInputSetState;

  static win32::Window        mainWindow;

  static win32::AppDll         appDll;
  static bs::AppData           appData;

  static bs::memory::Arena* defaultArena;
  static bs::font::GlyphTable* defaultGlyphTable;
  static bs::font::GlyphSheet* defaultGlyphSheet;

  static u32                   running;

  static thread::ThreadInfo mainThread;
  static thread::ThreadInfo syncedThreads[SYNCED_THREAD_COUNT];
  static thread::ThreadInfo asyncThreads[ASYNC_THREAD_COUNT];
};

namespace DEBUG
{
  static float scale = 1.0f;
};