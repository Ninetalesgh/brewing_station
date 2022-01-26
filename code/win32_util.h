#pragma once

#include <app_common/platform.h>
#include <common/basic_types.h>
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
  using win32_xInputGetState = DWORD WINAPI( DWORD, XINPUT_STATE* );
  DWORD WINAPI win32_stub_xInputGetState( DWORD, XINPUT_STATE* ) { return ERROR_DEVICE_NOT_CONNECTED; }

  using win32_xInputSetState = DWORD WINAPI( DWORD, XINPUT_VIBRATION* );
  DWORD WINAPI win32_stub_xInputSetState( DWORD, XINPUT_VIBRATION* ) { return ERROR_DEVICE_NOT_CONNECTED; }

  void load_xInput( win32_xInputGetState** xInputGetState, win32_xInputSetState** xInputSetState )
  {
    HMODULE module = LoadLibraryA( "xinput1_4.dll" );

    if ( !module )
    {
      module = LoadLibraryA( "xinput1_3.dll" );
    }

    if ( module )
    {
      *xInputGetState = (win32_xInputGetState*) GetProcAddress( module, "XInputGetState" );
      *xInputSetState = (win32_xInputSetState*) GetProcAddress( module, "XInputSetState" );

      #if DEBUG_LOG_INPUT
      OutputDebugStringA( "[WIN32_INPUT] Controller input initialized.\n" );
      #endif

    }
    else
    {
      OutputDebugStringA( "[WIN32_INPUT] ERROR - neither xinput1_4.dll or xinput1_3.dll found.\n" );
      *xInputGetState = win32_stub_xInputGetState;
      *xInputSetState = win32_stub_xInputSetState;
    }
  }

  namespace stub
  {
    void app_sample_sound( bs::AppSampleSoundParameter& ) {}
    void app_on_load( bs::AppOnLoadParameter& ) {}
    void app_tick( bs::AppTickParameter& ) {}
    void app_render( bs::AppRenderParameter& ) {}
    void app_receive_udp_packet( bs::AppReceiveUDPPacketParameter& ) {}
  };

  using win32_app_sample_sound = void( bs::AppSampleSoundParameter& );
  using win32_app_on_load = void( bs::AppOnLoadParameter& );
  using win32_app_tick = void( bs::AppTickParameter& );
  using win32_app_render = void( bs::AppRenderParameter& );
  using win32_app_receive_udp_packet = void( bs::AppReceiveUDPPacketParameter& );

  struct AppDll
  {
    HMODULE dll;
    win32_app_sample_sound* sample_sound = stub::app_sample_sound;
    win32_app_on_load* on_load = stub::app_on_load;
    win32_app_tick* tick = stub::app_tick;
    win32_app_render* render = stub::app_render;
    win32_app_receive_udp_packet* receive_udp_packet = stub::app_receive_udp_packet;
  };


}