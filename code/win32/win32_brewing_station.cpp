

#include "win32_util.h"
#include "win32_global.h"
#include "win32_app_dll_loader.h"
#include "win32_opengl.h"

#include <platform/platform.h>
#include <common/bsstring.h>

//winsock
#pragma comment(lib,"ws2_32.lib")
//WinMain
#pragma comment(lib,"user32.lib")
//time
#pragma comment(lib,"winmm.lib")

void brewing_station_loop()
{
  u64 debug_CyclesForFrame         = 0;
  u64 debug_CyclesForAppTick       = 0;
  u64 debug_CyclesSleep            = 0;

  LARGE_INTEGER beginCounter = win32::GetTimer();

  {
    PROFILE_SCOPE( debug_CyclesForFrame );

    {
      bs::Input& input = global::appData.input;
      memset( input.down, 0, bs::Input::STATE_COUNT );
      MSG message;
      while ( PeekMessage( &message, 0, 0, 0, PM_REMOVE ) )
      {
        switch ( message.message )
        {
          case WM_XBUTTONDOWN:
          case WM_XBUTTONUP:
          {
            assert( message.wParam == 65568 || message.wParam == 131136 );
            u8 code = message.wParam == 65568 ? bs::Input::MOUSE_4 : bs::Input::MOUSE_5;
            u8 isDown = (u8) !(message.lParam & (1 << 31));
            u8 wasDown = input.held[code];
            input.down[code] = ((!wasDown) && isDown);
            input.held[code] = isDown;
            break;
          }
          case WM_MOUSEWHEEL:
          {
            //TODO
            break;
          }
          case WM_MOUSEMOVE:
          {
            input.mousePos[0].start = input.mousePos[0].end;
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
            assert( message.wParam < bs::Input::STATE_COUNT );
            u64 code = message.wParam;

            u8 isDown = (u8) !(message.lParam & (1 << 31));
            u8 wasDown = input.held[code];
            input.held[code] = isDown;

            if ( !wasDown && isDown )
            {
              switch ( code )
              {
                case bs::Input::KEY_F9:
                  break;
                case bs::Input::KEY_F10:
                  break;
                case bs::Input::KEY_F11:
                  break;
                case bs::Input::KEY_F12:
                  // win32::ServerHandshake( global::netData.udpSocket, global::netData.server, global_debugUsername );
                  break;
                case bs::Input::KEY_ESCAPE:
                  global::running = false;
                  break;
                default:
                  input.down[code] = 1;
                  break;
              }
            }

            if ( input.held[bs::Input::KEY_ALT] && input.down[bs::Input::KEY_F4] ) global::running = false;

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

      bs::PrmAppTick appTickParameter {};
      appTickParameter.appData = &global::appData;
      global::appDll.tick( appTickParameter );
    }

    //TODO sound here ? 

    thread::sleep( 7 );

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

        log_info( "[WIN32_CLOCK] frame ", global::appData.currentFrameIndex, " had ", delta, " ms left after sleeping for ", max( msSleep, 0.f ),
                                             " ms\n - - - next sleep reduced by ", -global::sleepMsSubtraction, " ms\n" );
        do
        {
          secondsElapsedIncludingSleep = win32::GetSecondsElapsed( beginCounter, win32::GetTimer() );
        } while ( secondsElapsedIncludingSleep < APP_TARGET_SPF );
      }
      else
      {
        log_info( "[WIN32_CLOCK] Missed fps target for frame: ", global::appData.currentFrameIndex,
                                              "\n- - - - - - - Actual ms: ", 1000.f * secondsElapsed,
                                              "   fps: ", float( 1.f / secondsElapsed ), "\n" );
      }
    } // PROFILE_SCOPE( debug_CyclesSleep );
  } // PROFILE_SCOPE( debug_CyclesForFrame );
  LARGE_INTEGER endCounter = win32::GetTimer();

  {
    float ms = 1000.f * win32::GetSecondsElapsed( beginCounter, endCounter );
    float fps = 1000.f / ms;
    log_info( "[WIN32_CLOCK] ms: ", ms,
              "  fps: ", s32( fps + 0.5f ),
              "  Mcpf: ", float( debug_CyclesForFrame ) / 1000000.f, "\n" );
  }

}



void brewing_station_main()
{
  #ifdef BS_DEBUG
  platform::debug::global::ptr_debug_log = &win32::debug_log;
  #endif

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

  HWND window = 0;
  {
    win32::WindowInitParameter parameter {};
    parameter.windowName = L"tmp_window_name";
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

    window = win32::window_init( parameter );
    assert( window != 0 );
    bs::opengl::init( window );
  }

  s64 bufferSize = (s64) APP_MEMORY_SIZE;
  void* buffer = VirtualAlloc( 0, bufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
  memory::init_arena( (char*) buffer, bufferSize, &global::appData.mainArena );

  thread::ThreadInfo& mainThread = global::appThreads[global::threadCount++];
  mainThread.id = GetCurrentThreadId();
  mainThread.name = "thread_main";

  #ifdef BS_RELEASE_BUILD
  {
    global::appDll.sample_sound = &bs::app_sample_sound;
    global::appDll.on_load = &bs::app_on_load;
    global::appDll.tick = &bs::app_tick;
    global::appDll.render = &bs::app_render;
    global::appDll.receive_udp_packet = &bs::app_receive_udp_packet;
    //global::appDll.register_debug_callbacks = &platform::debug::app_register_debug_callbacks;
  }
  #else
  {
    win32::PrmThreadDllLoader dllLoaderPrm = {};
    thread::ThreadInfo standaloneDllLoadThread {};
    dllLoaderPrm.threadInfo = &standaloneDllLoadThread;
    dllLoaderPrm.appDll =  &global::appDll;
    dllLoaderPrm.pause_app = &win32::pause_all_app_threads;
    dllLoaderPrm.wait_for_pause = &win32::wait_for_all_app_threads_to_pause;
    dllLoaderPrm.resume_app = &win32::resume_all_app_threads;
    CloseHandle( CreateThread( 0, 0, win32::thread_DllLoader, &dllLoaderPrm, 0, (LPDWORD) &dllLoaderPrm.threadInfo->id ) );
  }
  #endif

  result = (s32) timeBeginPeriod( 1 );
  assert( result == TIMERR_NOERROR );

  global::running = true;
  while ( global::running )
  {
    thread::wait_if_requested( &mainThread );
    brewing_station_loop();
  }

}


#ifdef BS_RELEASE_BUILD
#include <apps/brewing_Station_app.cpp>
#endif