#pragma once

#include "win32_util.h"
#include <core/bsthread.h>
#include <platform/platform_debug.h>

#define dll_log_info(...) log_info("[WIN32_DLL] ", ...)
#define dll_log_error(...) log_error("[WIN32_DLL] ", ...)

namespace win32
{
  struct PrmThreadDllLoader
  {
    thread::ThreadInfo* threadInfo;
    //char const* appFilename; //lul?
    AppDll* appDll;
    voidfn* pause_app;
    voidfn* wait_for_pause;
    voidfn* resume_app;
  };
  DWORD thread_DllLoader( void* void_parameter )
  {
    PrmThreadDllLoader* parameter = (PrmThreadDllLoader*) void_parameter;
    thread::ThreadInfo* threadInfo = parameter->threadInfo;

    threadInfo->name = "thread_dll_loader";
    //dll_log_info( "Thread: ", threadInfo->name, "id: ", threadInfo->id, ".\n" );
    constexpr u32 THREAD_SLEEP_DURATION = 500;
    char const* TMP_APP_CODE_FILENAME[2] = { "bs_app0.tmp.dll", "bs_app1.tmp.dll" };
    char const* APP_FILENAME = "brewing_station_app.dll";
    u32 currentDllIndex = 0;

    //char const* filename = parameter.appFilename;
    AppDll newApp {};
    AppDll& currentApp = *parameter->appDll;

    FILETIME lastWriteTime = {};
    while ( true )
    {
      thread::wait_if_requested( threadInfo );

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
              newApp.sample_sound = (win32_app_sample_sound*) GetProcAddress( newApp.dll, "app_sample_sound" );
              newApp.on_load = (win32_app_on_load*) GetProcAddress( newApp.dll, "app_on_load" );
              newApp.tick = (win32_app_tick*) GetProcAddress( newApp.dll, "app_tick" );
              newApp.render = (win32_app_render*) GetProcAddress( newApp.dll, "app_render" );
              newApp.receive_udp_packet = (win32_app_receive_udp_packet*) GetProcAddress( newApp.dll, "app_receive_udp_packet" );
              newApp.register_debug_callbacks = (win32_app_register_debug_callbacks*) GetProcAddress( newApp.dll, "register_debug_callbacks" );

              if ( newApp.sample_sound && newApp.on_load && newApp.tick && newApp.receive_udp_packet && newApp.render && newApp.register_debug_callbacks )
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
              parameter->pause_app();

              currentDllIndex = currentDllIndex ? 0 : 1;
              lastWriteTime = findData.ftLastWriteTime;

              parameter->wait_for_pause();
              FreeLibrary( currentApp.dll );
              currentApp = newApp;

              currentApp.register_debug_callbacks( { &win32::debug_log } );
              parameter->resume_app();
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

    //dll_log_info( "DllLoading closing; thread id: ", threadInfo->id, ".\n" );
    return 0;
  }

}