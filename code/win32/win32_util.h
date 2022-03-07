#pragma once

#include "win32_global.h"


LRESULT CALLBACK brewing_station_main_window_callback( HWND window, UINT message, WPARAM wParam, LPARAM lParam )
{
  LRESULT result = 0;
  switch ( message )
  {
    case WM_DESTROY:
    {
      global::running = false;
      break;
    }
    default:
    {
      result = DefWindowProc( window, message, wParam, lParam );
      break;
    }
  }

  return result;
}

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

  INLINE LARGE_INTEGER GetTimer()
  {
    LARGE_INTEGER result;
    QueryPerformanceCounter( &result );
    return result;
  }

  INLINE float GetSecondsElapsed( LARGE_INTEGER beginCounter, LARGE_INTEGER endCounter )
  {
    return float( endCounter.QuadPart - beginCounter.QuadPart ) / float( global::performanceCounterFrequency );
  }

  void pause_all_app_threads()
  {
    for ( s32 i = 0; i < THREAD_COUNT_MAX; ++i )
    {
      if ( global::appThreads[i].id )
      {
        thread::request_pause( &global::appThreads[i] );
      }
    }
  }

  void wait_for_all_app_threads_to_pause()
  {
    for ( s32 i = 0; i < THREAD_COUNT_MAX; ++i )
    {
      if ( global::appThreads[i].id )
      {
        while ( !global::appThreads[i].isPaused )
        {
          thread::sleep( 1 );
        }
      }
    }
  }

  void resume_all_app_threads()
  {
    for ( s32 i = 0; i < THREAD_COUNT_MAX; ++i )
    {
      if ( global::appThreads[i].id )
      {
        thread::request_unpause( &global::appThreads[i] );
      }
    }
  }

  void debug_log( platform::debug::DebugLogFlags flags, char const* string, s32 size )
  {
    wchar_t wideChars[platform::debug::MAX_DEBUG_MESSAGE_LENGTH / 2];

    int wideCharCount = MultiByteToWideChar( CP_UTF8, 0, string, -1, NULL, 0 );

    if ( wideCharCount < platform::debug::MAX_DEBUG_MESSAGE_LENGTH )
    {
      MultiByteToWideChar( CP_UTF8, 0, string, -1, wideChars, wideCharCount );
    }

    OutputDebugStringW( wideChars );
    if ( flags & platform::debug::DebugLogFlags::WRITE_TO_DEBUG_LOG_FILE )
    {
      static HANDLE debug_log_file = CreateFileW( L"debug.log", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );

      s32 bytesWritten {};
      WriteFile( debug_log_file, string, size, (LPDWORD) &bytesWritten, 0 );
    }
    if ( flags & platform::debug::DebugLogFlags::SEND_TO_SERVER )
    {

    }
  };

  namespace stub
  {
    DWORD WINAPI xInputGetState( DWORD, XINPUT_STATE* ) { return ERROR_DEVICE_NOT_CONNECTED; }
    DWORD WINAPI xInputSetState( DWORD, XINPUT_VIBRATION* ) { return ERROR_DEVICE_NOT_CONNECTED; }
  };

  void load_xInput()
  {
    HMODULE module = LoadLibraryA( "xinput1_4.dll" );

    if ( !module )
    {
      module = LoadLibraryA( "xinput1_3.dll" );
    }

    if ( module )
    {
      global::xInputGetState = (xInputGetState*) GetProcAddress( module, "XInputGetState" );
      global::xInputSetState = (xInputSetState*) GetProcAddress( module, "XInputSetState" );

      #if DEBUG_LOG_INPUT
      OutputDebugStringA( "[WIN32_INPUT] Controller input initialized.\n" );
      #endif

    }
    else
    {
      OutputDebugStringA( "[WIN32_INPUT] ERROR - neither xinput1_4.dll or xinput1_3.dll found.\n" );
      global::xInputGetState = stub::xInputGetState;
      global::xInputSetState = stub::xInputSetState;
    }
  }

  INLINE void ProcessXInputDigitalButton( WORD wButtons, DWORD buttonBit, bs::ButtonState& state, bs::ButtonState const& oldState )
  {
    state.halfTransitionCount = (state.endedDown != oldState.endedDown) ? 1 : 0;
    state.endedDown = wButtons & buttonBit;
  }

  void ProcessControllerInput( bs::Input& input )
  {
    constexpr s32 supportedControllers = array_count( input.controller );
    constexpr s32 maxControllerCount = min( supportedControllers, XUSER_MAX_COUNT );

    for ( DWORD iController = 0; iController < (DWORD) maxControllerCount; ++iController )
    {
      XINPUT_STATE controllerState;
      if ( global::xInputGetState( iController, &controllerState ) == ERROR_SUCCESS )
      {
        //controller plugged in
        XINPUT_GAMEPAD& pad = controllerState.Gamepad;
        bs::ControllerInput& newController = input.controller[iController];

        ProcessXInputDigitalButton( pad.wButtons, XINPUT_GAMEPAD_DPAD_UP, newController.up, newController.up );
        ProcessXInputDigitalButton( pad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN, newController.down, newController.down );
        ProcessXInputDigitalButton( pad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT, newController.left, newController.left );
        ProcessXInputDigitalButton( pad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT, newController.right, newController.right );
        ProcessXInputDigitalButton( pad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER, newController.leftShoulder, newController.leftShoulder );
        ProcessXInputDigitalButton( pad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER, newController.rightShoulder, newController.rightShoulder );

        float stickX         = float( pad.sThumbLX ) / (pad.sThumbLX < 0 ? 32768.f : 32767.f);
        float stickY         = float( pad.sThumbLY ) / (pad.sThumbLY < 0 ? 32768.f : 32767.f);

        newController.start = newController.end;
        newController.min = newController.max = newController.end = { stickX, stickY };
        //bool start         = pad.wButtons & XINPUT_GAMEPAD_START;
        //bool back          = pad.wButtons & XINPUT_GAMEPAD_BACK;
        //bool buttonA       = pad.wButtons & XINPUT_GAMEPAD_A;
        //bool buttonB       = pad.wButtons & XINPUT_GAMEPAD_B;
        //bool buttonX       = pad.wButtons & XINPUT_GAMEPAD_X;
        //bool buttonY       = pad.wButtons & XINPUT_GAMEPAD_Y;
      }
      else
      {
        //controller not available
      }
    }

    // {
    //   XINPUT_VIBRATION vibrationState { 0, 0 };
    //   global::xInputSetState( 0, &vibrationState );
    // }
  }

};