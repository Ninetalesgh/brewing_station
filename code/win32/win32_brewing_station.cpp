

#include "win32_util.h"
#include <win32/win32_opengl.h>

#include <app_common/platform.h>
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



namespace global
{
  static s64 performanceCounterFrequency;
  static win32::xInputGetState* xInputGetState;
  static win32::xInputSetState* xInputSetState;

  static win32::AppDll         appDll;
  static bs::AppData           appData;

  static u32                   running;
};


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



s32 brewing_station_init()
{
  #ifdef BS_DEBUG
  platform::debug::global::ptr_debug_log = &debug_log;
  #endif

  log_info( "[WIN32]", "ĹĿŌ" );

  s32 result = 1;
  HINSTANCE hInstance = GetModuleHandle( NULL );

  result &= QueryPerformanceFrequency( (LARGE_INTEGER*) &global::performanceCounterFrequency );
  assert( result );

  {
    WSADATA wsaData;
    result ^= WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
  }
  assert( result );

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
    parameter.wndClass.hCursor       =LoadCursor( (HINSTANCE) NULL, IDC_ARROW );
    //parameter.wndClass.hbrBackground =(HBRUSH) (COLOR_APPWORKSPACE + 1);
    //parameter.wndClass.lpszMenuName  =MAKEINTRESOURCE(IDR_GUIED_MAIN);
    parameter.wndClass.lpszClassName = L"bswnd";
    //parameter.wndClass.hIconSm       =;

    window = win32::window_init( parameter );
    assert( window != 0 );
    bs::opengl::init( window );
  }

  // FETCH DLL HERE FIRST TODO to specify window stuff

  global::running = true;
  return result;
}

void brewing_station_run()
{
  using namespace win32;
  // threading::ThreadInfo mainThread {};
  // mainThread.id = GetCurrentThreadId();
  // mainThread.name = "thread_Main";
  // mainThread.parent = nullptr;

  // while ( global::running )
  // {

  // }
}