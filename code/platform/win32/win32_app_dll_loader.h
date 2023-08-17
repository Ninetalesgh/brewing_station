#pragma once

#include "win32_util.h"
#include "win32_thread.h"
#include "win32_opengl.h"
#include "win32_platformcallbacks.h"

namespace win32
{
  s32 get_exe_path( char* out_exePath, s32 exePathLengthMax )
  {
    wchar_t wideChars[MAX_BS_PATH];
    s32 filePathLength = GetModuleFileNameW( 0, wideChars, MAX_BS_PATH );
    assert( filePathLength <= exePathLengthMax );
    return wchar_to_utf8( wideChars, out_exePath, exePathLengthMax );
  }

  struct PrmThreadDllLoader
  {
    thread::ThreadInfo* threadInfo;
    HGLRC renderContext;
    //char const* appFilename; //lul?
    AppDll* appDll;
  };
  DWORD WINAPI thread_DllLoader( void* void_parameter )
  {
    PrmThreadDllLoader* parameter = (PrmThreadDllLoader*) void_parameter;
    thread::ThreadInfo* threadInfo = parameter->threadInfo;
    AppDll* appDll = parameter->appDll;

    opengl::set_worker_thread_render_context( parameter->renderContext );

    thread::write_barrier();
    parameter->threadInfo = nullptr;

    threadInfo->name = "thread_dll_loader";
    constexpr u32 THREAD_SLEEP_DURATION = 500;
    char const* TMP_APP_CODE_FILENAME[2] = { "bs_app0.tmp.dll", "bs_app1.tmp.dll" };
    char const* APP_FILENAME = "brewing_station_app.dll";
    u32 currentDllIndex = 0;

    AppDll newApp {};
    AppDll& currentApp = *appDll;

    FILETIME lastWriteTime = {};
    for ( ;;)
    {
      thread::pause_thread_if_requested( threadInfo );

      _WIN32_FIND_DATAA findData;
      HANDLE findHandle = FindFirstFileA( APP_FILENAME, &findData );
      if ( findHandle != INVALID_HANDLE_VALUE )
      {
        FindClose( findHandle );

        if ( CompareFileTime( &lastWriteTime, &findData.ftLastWriteTime ) )
        {
          if ( CopyFileA( APP_FILENAME, TMP_APP_CODE_FILENAME[currentDllIndex], false ) )//~60M cycles
          {
            //dll_log_info( "copied ", APP_FILENAME, " over to ", TMP_APP_CODE_FILENAME[currentDllIndex], ".\n" );
            u32 successfulLoad = false;

            newApp.dll = LoadLibraryA( TMP_APP_CODE_FILENAME[currentDllIndex] );//~120M cycles
            if ( newApp.dll )
            {
              //   newApp.sample_sound = (win32_app_sample_sound*) GetProcAddress( newApp.dll, "app_sample_sound" );
              newApp.on_load = (bsp::app_on_load_fn*) GetProcAddress( newApp.dll, "app_on_load_internal" );
              newApp.tick = (bsp::app_tick_fn*) GetProcAddress( newApp.dll, "app_tick_internal" );
              //  newApp.receive_udp_packet = (win32_app_receive_udp_packet*) GetProcAddress( newApp.dll, "app_receive_udp_packet" );
              //   newApp.register_callbacks = (win32_app_register_callbacks*) GetProcAddress( newApp.dll, "register_callbacks" );

 //              if ( newApp.sample_sound && newApp.on_load && newApp.tick && newApp.receive_udp_packet && newApp.register_callbacks )
              if ( true )
              {
                successfulLoad = true;
                //dll_log_info( "Loaded ", TMP_APP_CODE_FILENAME[currentDllIndex], ".\n" );
              }
              else
              {
                BREAK;
                //dll_log_error( "Couldn't fetch proc addresses in ", TMP_APP_CODE_FILENAME[currentDllIndex], ".\n" );
              }
            }
            else
            {
              BREAK;
              //dll_log_error( "Couldn't load library ", TMP_APP_CODE_FILENAME[currentDllIndex], ".\n" );
            }

            if ( successfulLoad )
            {
              log_info( "[DLL RELOAD] Requesting application pause." );
              dll_reload_threads_action( &thread::request_pause );

              currentDllIndex = currentDllIndex ? 0 : 1;
              lastWriteTime = findData.ftLastWriteTime;

              dll_reload_threads_action( &thread::wait_for_thread_to_pause );
              log_info( "[DLL RELOAD] Switching application library." );

              FreeLibrary( currentApp.dll );
              currentApp = newApp;

              // platform::Callbacks prm = get_callbacks();
             //  currentApp.register_callbacks( prm );
              char exePath[512] = {};
              get_exe_path( exePath, 512 );

              // TODO: maybe this not here? 
              currentApp.on_load( &global::appData, &global::platformCallbacks, exePath );

              static s32 reloadCount = 0;
              log_info( "[DLL RELOAD] Successfully loaded application instance ", reloadCount++ );

              dll_reload_threads_action( &thread::request_unpause );
              log_info( "[DLL RELOAD] Requesting application unpause." );
            }
            else
            {
              if ( newApp.dll ) FreeLibrary( newApp.dll );
              DeleteFileA( TMP_APP_CODE_FILENAME[currentDllIndex] );
            }
            newApp = {};
          }
        }
      }

      thread::sleep( THREAD_SLEEP_DURATION );
    }
  }

}