#include "win32_util.h"
#include "win32_global.h"
#include "win32_app_dll_loader.h"
#include "win32_platformcallbacks.h"

#include "win32_thread.h"
#include <platform/win32/win32_opengl.h>
#include <platform/bs_platform.h>
#include <module/bs_performanceprofile.h>
//#include <core/bsfont.h>
#include <common/bsstring.h>

//winsock
#pragma comment(lib,"ws2_32.lib")
//WinMain
#pragma comment(lib,"user32.lib")
//time
#pragma comment(lib,"winmm.lib")

void brewing_station_loop();
void brewing_station_main();
u32 init_compiled_assets();
LRESULT CALLBACK brewing_station_main_window_callback( HWND window, UINT message, WPARAM wParam, LPARAM lParam );


void brewing_station_loop()
{
  u64 debug_CyclesForFrame         = 0;
  u64 debug_CyclesForAppTick       = 0;
  u64 debug_CyclesSleep            = 0;

  LARGE_INTEGER beginCounter = win32::GetTimer();

  {
    PROFILE_SCOPE( debug_CyclesForFrame );

    {
      bs::input::State& input = global::appData.input;
      memset( input.down, 0, bs::input::STATE_COUNT );
      MSG message;
      while ( PeekMessage( &message, 0, 0, 0, PM_REMOVE ) )
      {
        input.mousePos[0].start = input.mousePos[0].end;

        switch ( message.message )
        {
          case WM_XBUTTONDOWN:
          case WM_XBUTTONUP:
          {
            assert( message.wParam == 65568 || message.wParam == 131136 );
            u32 code = message.wParam == 65568 ? bs::input::MOUSE_4 : bs::input::MOUSE_5;
            u8 isDown = (u8) !(message.lParam & (1 << 31));
            u8 wasDown = input.held[code];
            input.down[code] = ((!wasDown) && isDown);
            input.held[code] = isDown;
            break;
          }
          case WM_MOUSEWHEEL:
          {
            input.mouseWheelDelta = GET_WHEEL_DELTA_WPARAM( message.wParam ) / WHEEL_DELTA;
            DEBUG::scale -= float( input.mouseWheelDelta ) * 0.03f;
            break;
          }
          case WM_MOUSEMOVE:
          {
            input.mousePos[0].min
              = input.mousePos[0].max
              = input.mousePos[0].end
              = { s32( s16( LOWORD( message.lParam ) ) ), s32( s16( HIWORD( message.lParam ) ) ) };
            break;
          }
          case WM_LBUTTONDOWN:
          case WM_LBUTTONUP:
          case WM_RBUTTONDOWN:
          case WM_RBUTTONUP:
          case WM_MBUTTONDOWN:
          case WM_MBUTTONUP:
          case WM_SYSKEYDOWN:
          case WM_SYSKEYUP:
          case WM_KEYDOWN:
          case WM_KEYUP:
          {
            assert( message.wParam < bs::input::STATE_COUNT );
            u32 code = (u32) message.wParam;

            u8 isDown = (u8) !(message.lParam & (1 << 31));
            u8 wasDown = input.held[code];
            input.held[code] = isDown;

            if ( !wasDown && isDown )
            {
              switch ( code )
              {
                case bs::input::KEY_F9:
                  break;
                case bs::input::KEY_F10:
                  break;
                case bs::input::KEY_F11:
                  break;
                case bs::input::KEY_F12:
                  // win32::ServerHandshake( global::netData.udpSocket, global::netData.server, global_debugUsername );
                  break;
                case bs::input::KEY_ESCAPE:
                  global::running = false;
                  break;
                default:
                  input.down[code] = 1;
                  break;
              }
            }

            if ( input.held[bs::input::KEY_ALT] && input.down[bs::input::KEY_F4] ) global::running = false;

            break;
          }
          default:
          {
            TranslateMessage( &message );
            DispatchMessage( &message );
          }
          break;
        }

        win32::ProcessControllerInput( global::appData.input );
      }
    }

    {
      PROFILE_SCOPE( debug_CyclesForAppTick );

      global::appDll.tick( &global::appData );
    }

    //TODO sound here ? 
    //bs::TaskState volatile state;
    //win32::push_synced_task( { &testfunc, nullptr } );
    //win32::push_async_task( { &testfunc, nullptr }, &state );


    //TODO remove this here and leave it up to the app, if we do dll reload has to also pause these threads manually
    win32::complete_synced_tasks();

    constexpr float APP_TARGET_FPS = 60.0f;
    constexpr float APP_TARGET_SPF = 1.0f / float( APP_TARGET_FPS );

    float secondsElapsed = win32::GetSecondsElapsed( beginCounter, win32::GetTimer() );
    {
      PROFILE_SCOPE( debug_CyclesSleep );
      if ( secondsElapsed < APP_TARGET_SPF )
      {
        float const msSleep = ((APP_TARGET_SPF - secondsElapsed) * 1000.f) + global::sleepMsSubtraction;
        thread::sleep( s32( max( msSleep, 0.0f ) ) );
        float secondsElapsedIncludingSleep =  win32::GetSecondsElapsed( beginCounter, win32::GetTimer() );
        float const delta = 1000.0f * (APP_TARGET_SPF - secondsElapsedIncludingSleep);
        global::sleepMsSubtraction += min( 0.f, delta ) - (delta > 2.0f) * 1.0f;

        //   log_info( "[WIN32_CLOCK] frame ", global::appData.currentFrameIndex, " had ", delta, " ms left after sleeping for ", max( msSleep, 0.f ),
        //                                        " ms\n - - - next sleep reduced by ", -global::sleepMsSubtraction, " ms\n" );
        do
        {
          secondsElapsedIncludingSleep = win32::GetSecondsElapsed( beginCounter, win32::GetTimer() );
        } while ( secondsElapsedIncludingSleep < APP_TARGET_SPF );
      }
      else
      {
        //  log_info( "[WIN32_CLOCK] Missed fps target for frame: ", global::appData.currentFrameIndex,
        //                                        "\n- - - - - - - Actual ms: ", 1000.f * secondsElapsed,
        //                                       "   fps: ", float( 1.f / secondsElapsed ), "\n" );
      }
    } // PROFILE_SCOPE( debug_CyclesSleep );
  } // PROFILE_SCOPE( debug_CyclesForFrame );

  LARGE_INTEGER endCounter = win32::GetTimer();


  //log_info( "[WIN32] frame: ", global::appData.currentFrameIndex );
  {
    //  float ms = 1000.f * win32::GetSecondsElapsed( beginCounter, endCounter );
    //  float fps = 1000.f / ms;
    //  log_info( "[WIN32_CLOCK] ms: ", ms,
    //            "  fps: ", s32( fps + 0.5f ),
    //           "  Mcpf: ", float( debug_CyclesForFrame ) / 1000000.f, "\n" );
  }
}

LRESULT CALLBACK brewing_station_main_window_callback( HWND window, UINT message, WPARAM wParam, LPARAM lParam )
{
  LRESULT result = 0;
  switch ( message )
  {
    case WM_SIZE:
    {
      global::mainWindow.size.x = s32( LOWORD( lParam ) );
      global::mainWindow.size.y = s32( HIWORD( lParam ) );
      if ( wParam == SIZE_MAXIMIZED || wParam == SIZE_MINIMIZED )
      {
        opengl::resize_viewport( global::mainWindow.size );
      }
      break;
    }
    case WM_MOVE:
    {
      global::mainWindow.pos.x = s32( LOWORD( lParam ) );
      global::mainWindow.pos.y = s32( HIWORD( lParam ) );
      break;
    }
    case WM_DISPLAYCHANGE:
    case WM_EXITSIZEMOVE:
    {
      opengl::resize_viewport( global::mainWindow.size );
      break;
    }
    case WM_DESTROY:
    {
      global::running = false;
      break;
    }
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
    {
      if ( !wParam )
      {
        memset( global::appData.input.held, 0, bs::input::STATE_COUNT );
      }
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

void brewing_station_init_release();

void brewing_station_main()
{
  // win32::fetch_paths();

  s32 result = 1;
  HINSTANCE hInstance = GetModuleHandle( NULL );

  result = QueryPerformanceFrequency( (LARGE_INTEGER*) &global::performanceCounterFrequency );
  assert( result );

  {
    WSADATA wsaData;
    result = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
    assert( result == 0 );
  }

  win32::load_xInput();

  {
    win32::WindowInitParameter parameter {};
    parameter.windowName = L"tmp_window_name";
    parameter.width = 1024;//DEFAULT_WINDOW_SIZE.x;
    parameter.height = 780;//DEFAULT_WINDOW_SIZE.y;
    parameter.x = -parameter.width - 200;
    parameter.y = 200;
    parameter.wndClass.cbSize        = sizeof( WNDCLASSEX );
    parameter.wndClass.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    parameter.wndClass.lpfnWndProc   = brewing_station_main_window_callback;
    //parameter.wndClass.cbClsExtra    =;
    //parameter.wndClass.cbWndExtra    =;
    parameter.wndClass.hInstance     = hInstance;
    //parameter.wndClass.hIcon         =;
    parameter.wndClass.hCursor       = LoadCursor( (HINSTANCE) NULL, IDC_ARROW );
    //parameter.wndClass.hbrBackground =(HBRUSH) (COLOR_APPWORKSPACE + 1);
    //parameter.wndClass.lpszMenuName  =MAKEINTRESOURCE(IDR_GUIED_MAIN);
    parameter.wndClass.lpszClassName = L"bswnd";
    //parameter.wndClass.hIconSm       =;

    global::mainWindow.handle = win32::init_window( parameter );

    assert( global::mainWindow.handle != 0 );
    HDC deviceContext = GetDC( global::mainWindow.handle );
    assert( deviceContext != 0 );
    opengl::init( deviceContext, global::mainWindow.size );
  }

  void* buffer = VirtualAlloc( 0, (s64) global::APP_MEMORY_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
  // bs::memory::init_arena( (char*) buffer, (s64) global::APP_MEMORY_SIZE, &global::mainArena );

  global::defaultArena = bs::memory::init_arena_in_place( (char*) buffer, (s64) global::APP_MEMORY_SIZE );

  global::mainThread.id = thread::get_current_thread_id();
  global::mainThread.name = "thread_main";

  win32::init_worker_threads();

  result = (s32) timeBeginPeriod( 1 );
  assert( result == TIMERR_NOERROR );

  register_callbacks( &global::platformCallbacks );


  #ifdef BS_RELEASE_BUILD
  {
    brewing_station_init_release();
  }
  #else
  {
    win32::PrmThreadDllLoader dllLoaderPrm = {};
    thread::ThreadInfo standaloneDllLoadThread {};
    dllLoaderPrm.threadInfo = &standaloneDllLoadThread;
    dllLoaderPrm.appDll =  &global::appDll;
    dllLoaderPrm.renderContext = opengl::create_render_context_for_worker_thread();
    //    dllLoaderPrm.for_all_app_threads = &win32::for_all_app_threads;
    CloseHandle( CreateThread( 0, 0, win32::thread_DllLoader, &dllLoaderPrm, 0, (LPDWORD) &dllLoaderPrm.threadInfo->id ) );
  }
  #endif

  global::running = true;
  while ( global::running )
  {
    thread::pause_thread_if_requested( &global::mainThread );
    brewing_station_loop();

    opengl::swap_buffers();


    ++global::appData.currentFrameIndex;
  }
}


#ifdef BS_RELEASE_BUILD
#include <apps/brewing_Station_app.cpp>

void brewing_station_init_release()
{
  global::appDll.on_load = &bsp::app_on_load_internal;
  global::appDll.tick = &bsp::app_tick_internal;
  char exePath[512] = {};
  win32::get_exe_path( exePath, 512 );
  global::appDll.on_load( &global::appData, &global::platformCallbacks, exePath );
}
#else

void brewing_station_init_release() {}

namespace bsp
{
  PlatformCallbacks* platform;
};
#endif

