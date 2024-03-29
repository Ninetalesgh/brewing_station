#pragma once

//#include "win32_file.h"
#include "win32_thread.h"
#include "win32_global.h"


#include <stdio.h>

namespace win32
{
  struct WindowInitParameter
  {
    LPCWCHAR windowName;
    WNDCLASSEX wndClass;
    s32 x;
    s32 y;
    s32 width;
    s32 height;
  };
  HWND init_window( WindowInitParameter const& parameter )
  {
    HWND resultWindow = 0;

    if ( RegisterClassEx( &parameter.wndClass ) )
    {
      resultWindow = CreateWindowEx( WS_EX_ACCEPTFILES,                // DWORD dwExStyle,                                  
                                     parameter.wndClass.lpszClassName, // LPCWSTR lpClassName,                                  
                                     parameter.windowName,             // LPCWSTR lpWindowName,                     
                                     WS_OVERLAPPEDWINDOW | WS_VISIBLE, // DWORD dwStyle,                                 
                                     parameter.x,                      // int X,               
                                     parameter.y,                      // int Y,              
                                     parameter.width,                  // int nWidth,               
                                     parameter.height,                 // int nHeight,              
                                     /*parent window*/ 0,              // HWND hWndParent,                    
                                     /*menu*/ 0,                       // HMENU hMenu,           
                                     parameter.wndClass.hInstance,     // HINSTANCE hInstance,                             
                                     0 );                              // LPVOID lpParam    
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

  void dll_reload_threads_action( threadfn* fn )
  {
    for_all_async_worker_threads( fn );
    //synced threads are paused together with mainThread
    fn( &global::mainThread );
  }

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

  INLINE void ProcessXInputDigitalButton( WORD wButtons, DWORD buttonBit, bs::input::ControllerButton& state, bs::input::ControllerButton const& oldState )
  {
    state.halfTransitionCount = (state.endedDown != oldState.endedDown) ? 1 : 0;
    state.endedDown = wButtons & buttonBit;
  }

  void ProcessControllerInput( bs::input::State& input )
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
        bs::input::Controller& newController = input.controller[iController];

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