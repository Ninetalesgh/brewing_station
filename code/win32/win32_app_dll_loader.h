#pragma once

#include "win32_util.h"

#define log_info(...) LOG_INFO_NO_CONTEXT("[WIN32_DLL] ", ...)
#define log_error(...) LOG_ERROR_NO_CONTEXT("[WIN32_DLL] ", ...)

namespace win32
{
  struct ThreadDllLoadingParameter
  {
    char const* appFilename; //lul?
    AppDll* appDll;
  };
  void thread_DllLoading( threading::ThreadInfo* threadInfo, void* void_parameter )
  {
    threadInfo->name = "thread_DllLoading";
    ThreadDllLoadingParameter& parameter = *(ThreadDllLoadingParameter*) void_parameter;

    log_info( "Thread: ", threadInfo->name, "id: ", threadInfo->id, ".\n" );
    constexpr u32 THREAD_SLEEP_DURATION = 500;
    char const* TMP_APP_CODE_FILENAME[2] = { "bs_app0.tmp.dll", "bs_app1.tmp.dll" };
    char const* APP_FILENAME = "brewing_station_app.dll";
    u32 currentDllIndex = 0;

    char const* filename = parameter.appFilename;
    AppDll newApp {};
    AppDll& currentApp = *parameter.appDll;

    FILETIME lastWriteTime = {};
    while ( true )
    {
      threading::wait_if_requested( threadInfo );

      _WIN32_FIND_DATAA findData;
      HANDLE findHandle = FindFirstFileA( APP_FILENAME, &findData );
      if ( findHandle != INVALID_HANDLE_VALUE )
      {
        FindClose( findHandle );

        if ( CompareFileTime( &lastWriteTime, &findData.ftLastWriteTime ) )
        {
          if ( CopyFileA( APP_FILENAME, TMP_APP_CODE_FILENAME[currentDllIndex], false ) )//~60M cycles
          {
            log_info( "copied ", APP_FILENAME, " over to ", TMP_APP_CODE_FILENAME[currentDllIndex], ".\n" );
            u32 successfulLoad = false;

            newApp.dll = LoadLibraryA( TMP_APP_CODE_FILENAME[currentDllIndex] );//~120M cycles
            if ( newApp.dll )
            {
              newApp.sample_sound = (win32_app_sample_sound*) GetProcAddress( newApp.dll, "app_sample_sound" );
              newApp.on_load = (win32_app_on_load*) GetProcAddress( newApp.dll, "app_on_load" );
              newApp.tick = (win32_app_tick*) GetProcAddress( newApp.dll, "app_tick" );
              newApp.render = (win32_app_render*) GetProcAddress( newApp.dll, "app_render" );
              newApp.receive_udp_packet = (win32_app_receive_udp_packet*) GetProcAddress( newApp.dll, "app_receive_udp_packet" );

              if ( newApp.sample_sound && newApp.on_load && newApp.tick && newApp.receive_udp_packet && newApp.render )
              {
                successfulLoad = true;
                log_info( "Loaded ", TMP_APP_CODE_FILENAME[currentDllIndex], ".\n" );
              }
              else
              {
                BREAK;
                log_error( "Couldn't fetch proc addresses in ", TMP_APP_CODE_FILENAME[currentDllIndex], ".\n" );
              }
            }
            else
            {
              BREAK;
              log_error( "Couldn't load library ", TMP_APP_CODE_FILENAME[currentDllIndex], ".\n" );
            }

            if ( succesfulLoad )
            {
              // TODO request all other threads to wait

              currentDllIndex = currentDllIndex ? 0 : 1;
              lastWriteTime = findData.ftLastWriteTime;
              FreeLibrary( currentApp.dll );
              currentApp = newApp;
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


      threading::sleep( THREAD_SLEEP_DURATION );
    }

    log_info( "[WIN32_THREAD] DllLoading closing; thread id: ", threadInfo->id, ".\n" );
  }
}