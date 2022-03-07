

#include "win32_util.h"
#include "win32_app_dll_loader.h"
#include <win32/win32_opengl.h>

#include <platform/platform.h>
#include <common/bsstring.h>

#include <windows.h>
#ifdef min
# undef min
#endif
#ifdef max
# undef max
#endif

//winsock
#pragma comment(lib,"ws2_32.lib")
//WinMain
#pragma comment(lib,"user32.lib")
//time?
#pragma comment(lib,"winmm.lib")

//CoInitialize
//#pragma comment(lib,"Ole32.lib")


constexpr u32 THREAD_COUNT_MAX = 32;
constexpr u64 APP_MEMORY_SIZE = GigaBytes( 1 );
namespace global
{
  static s64 performanceCounterFrequency;
  static win32::xInputGetState* xInputGetState;
  static win32::xInputSetState* xInputSetState;

  static win32::AppDll         appDll;
  static bs::AppData           appData;

  static u32                   running;

  static thread::ThreadInfo appThreads[THREAD_COUNT_MAX];
  static u32                   threadCount;
};

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

void brewing_station_loop()
{


  // while ( global::running )
  // {

  // }
}


void brewing_station_main()
{
  #ifdef BS_DEBUG
  platform::debug::global::ptr_debug_log = &debug_log;
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

  //init debug logging here TODO

  load_xInput( &global::xInputGetState, &global::xInputSetState );

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
    dllLoaderPrm.pause_app = &pause_all_app_threads;
    dllLoaderPrm.wait_for_pause = &wait_for_all_app_threads_to_pause;
    dllLoaderPrm.resume_app = &resume_all_app_threads;
    CloseHandle( CreateThread( 0, 0, win32::thread_DllLoader, &dllLoaderPrm, 0, (LPDWORD) &dllLoaderPrm.threadInfo->id ) );
  }
  #endif


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