#pragma once

#include <platform/platform.h>
#include <common/bscommon.h>
#include <windows.h>

namespace win32
{
  struct WindowInitParameter
  {
    LPCWCHAR windowName;
    WNDCLASSEX wndClass;
  };
  HWND window_init( WindowInitParameter const& parameter )
  {
    HWND resultWindow = 0;

    if ( RegisterClassEx( &parameter.wndClass ) )
    {
      resultWindow = CreateWindowEx( WS_EX_ACCEPTFILES,
                                     parameter.wndClass.lpszClassName, parameter.windowName,
                                     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                     CW_USEDEFAULT, CW_USEDEFAULT,
                                     CW_USEDEFAULT, CW_USEDEFAULT,
                                     /*parent window*/ 0,
                                     /*menu*/ 0,
                                     parameter.wndClass.hInstance,
                                     0 );
    }

    return resultWindow;
  }

  #include <Xinput.h>
  using xInputGetState = DWORD WINAPI( DWORD, XINPUT_STATE* );
  using xInputSetState = DWORD WINAPI( DWORD, XINPUT_VIBRATION* );

  namespace stub
  {
    DWORD WINAPI xInputGetState( DWORD, XINPUT_STATE* ) { return ERROR_DEVICE_NOT_CONNECTED; }
    DWORD WINAPI xInputSetState( DWORD, XINPUT_VIBRATION* ) { return ERROR_DEVICE_NOT_CONNECTED; }
  };

  void load_xInput( xInputGetState** _xInputGetState, xInputSetState** _xInputSetState )
  {
    HMODULE module = LoadLibraryA( "xinput1_4.dll" );

    if ( !module )
    {
      module = LoadLibraryA( "xinput1_3.dll" );
    }

    if ( module )
    {
      *_xInputGetState = (xInputGetState*) GetProcAddress( module, "XInputGetState" );
      *_xInputSetState = (xInputSetState*) GetProcAddress( module, "XInputSetState" );

      #if DEBUG_LOG_INPUT
      OutputDebugStringA( "[WIN32_INPUT] Controller input initialized.\n" );
      #endif

    }
    else
    {
      OutputDebugStringA( "[WIN32_INPUT] ERROR - neither xinput1_4.dll or xinput1_3.dll found.\n" );
      *_xInputGetState = stub::xInputGetState;
      *_xInputSetState = stub::xInputSetState;
    }
  }

  namespace stub
  {
    void app_sample_sound( bs::PrmAppSampleSound ) {}
    void app_on_load( bs::PrmAppOnLoad ) {}
    void app_tick( bs::PrmAppTick ) {}
    void app_render( bs::PrmAppRender ) {}
    void app_receive_udp_packet( bs::PrmAppReceiveUDPPacket ) {}
    void app_register_debug_callbacks( platform::debug::PrmRegisterDebugCallbacks ) {}
  };

  using  win32_app_sample_sound = void( bs::PrmAppSampleSound );
  using  win32_app_on_load = void( bs::PrmAppOnLoad );
  using  win32_app_tick = void( bs::PrmAppTick );
  using  win32_app_render = void( bs::PrmAppRender );
  using  win32_app_receive_udp_packet = void( bs::PrmAppReceiveUDPPacket );
  using  win32_app_register_debug_callbacks = void( platform::debug::PrmRegisterDebugCallbacks );

  struct AppDll
  {
    HMODULE dll;
    win32_app_sample_sound* sample_sound = stub::app_sample_sound;
    win32_app_on_load* on_load = stub::app_on_load;
    win32_app_tick* tick = stub::app_tick;
    win32_app_render* render = stub::app_render;
    win32_app_receive_udp_packet* receive_udp_packet = stub::app_receive_udp_packet;
    win32_app_register_debug_callbacks* register_debug_callbacks = stub::app_register_debug_callbacks;
  };


}