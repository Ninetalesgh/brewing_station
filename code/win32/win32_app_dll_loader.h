#pragma once

#include "win32_util.h"
#include "win32_thread.h"
#include <core/bsdebuglog.h>

#define dll_log_info(...) log_info("[WIN32_DLL] ", ...)
#define dll_log_error(...) log_error("[WIN32_DLL] ", ...)

namespace win32
{
  using for_all_threadfn = void( threadfn* );
  struct PrmThreadDllLoader
  {
    thread::ThreadInfo* threadInfo;
    //char const* appFilename; //lul?
    AppDll* appDll;
  };
  DWORD WINAPI thread_DllLoader( void* void_parameter )
  {
    PrmThreadDllLoader* parameter = (PrmThreadDllLoader*) void_parameter;
    thread::ThreadInfo* threadInfo = parameter->threadInfo;
    AppDll* appDll = parameter->appDll;

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
              newApp.sample_sound = (win32_app_sample_sound*) GetProcAddress( newApp.dll, "app_sample_sound" );
              newApp.on_load = (win32_app_on_load*) GetProcAddress( newApp.dll, "app_on_load" );
              newApp.tick = (win32_app_tick*) GetProcAddress( newApp.dll, "app_tick" );
              newApp.render = (win32_app_render*) GetProcAddress( newApp.dll, "app_render" );
              newApp.receive_udp_packet = (win32_app_receive_udp_packet*) GetProcAddress( newApp.dll, "app_receive_udp_packet" );
              newApp.register_callbacks = (win32_app_register_callbacks*) GetProcAddress( newApp.dll, "register_callbacks" );

              if ( newApp.sample_sound && newApp.on_load && newApp.tick && newApp.receive_udp_packet && newApp.render && newApp.register_callbacks )
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
              dll_reload_threads_action( &thread::request_pause );

              currentDllIndex = currentDllIndex ? 0 : 1;
              lastWriteTime = findData.ftLastWriteTime;

              dll_reload_threads_action( &thread::wait_for_thread_to_pause );
              FreeLibrary( currentApp.dll );
              currentApp = newApp;

              platform::Callbacks prm {};

              prm.ptr_debug_log = &win32::debug_log;
              prm.ptr_push_async_task = &win32::push_async_task;
              prm.ptr_push_synced_task = &win32::push_synced_task;
              prm.ptr_complete_synced_tasks = &win32::complete_synced_tasks;
              //TODO
              prm.ptr_get_file_info = &win32::get_file_info;
              prm.ptr_load_file_into_memory = &win32::load_file_into_memory;
              prm.ptr_write_file = &win32::write_file;

              //  prm.send_udp = &win32::send_udp;
              //  prm.send_tcp = &win32::send_tcp;

              prm.mainArena = global::defaultArena;

              currentApp.register_callbacks( prm );

              // TODO: maybe this not here? 
              currentApp.on_load( { &global::appData } );

              dll_reload_threads_action( &thread::request_unpause );
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