//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FILE: main
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define UNICODE

#include "common/basic_types.h"

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
//StretchDIBits
//#pragma comment(lib,"Gdi32.lib")
//time?
#pragma comment(lib,"winmm.lib")
//CoInitialize
//#pragma comment(lib,"Ole32.lib")
//openGL
//#pragma comment(lib,"opengl32.lib")

namespace win32
{
  s32 brewing_station_init();
  void brewing_station_run();
}

int main()
{
  if ( win32::brewing_station_init() )
  {
    win32::brewing_station_run();
  }
  // win32::brewing_station_end();

  return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FILE: main brewing station entry .cpp
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "app_common/platform.h"

#include "win32_util.h"

#include <common/string.h>

namespace win32
{
  struct BrewingStationGlobalData
  {
    s64 performanceCounterFrequency;
    win32_xInputGetState* xInputGetState;
    win32_xInputSetState* xInputSetState;

    AppDll                appDll[2];
    bs::AppData           appData;

    s32                   running;
  } global;

  LRESULT CALLBACK brewing_station_main_window_callback( HWND window, UINT message, WPARAM wParam, LPARAM lParam )
  {
    LRESULT result = 0;
    switch ( message )
    {
      case WM_DESTROY:
      {
        global.running = false;
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

  s32 brewing_station_init()
  {
    s32 result = 1;

    HINSTANCE hInstance = GetModuleHandle( NULL );

    result &= QueryPerformanceFrequency( (LARGE_INTEGER*) &global.performanceCounterFrequency );
    assert( result );

    {
      WSADATA wsaData;
      result ^= WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
    }
    assert( result );

    //init debug logging here TODO

    load_xInput( &global.xInputGetState, &global.xInputSetState );

    // FETCH DLL HERE FIRST TODO

    HWND window = 0;
    {
      WindowInitParameter parameter {};
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

      window = window_init( parameter );
      assert( window != 0 );
    }

    return result;
  }

  void brewing_station_run()
  {

  }
}
