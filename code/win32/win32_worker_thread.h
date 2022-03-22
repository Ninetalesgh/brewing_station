#pragma once

#include "win32_thread.h"
#include "win32_global.h"
#include <core/bstask.h>

#include <synchapi.h>

namespace win32
{
  u32 init_worker_threads();

  void push_async_task( bs::taskfn*, bs::Task* out_task );
  void push_synced_task( bs::taskfn* );
  void wait_for_synced_tasks();
};


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////cpp/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace global
{
  HANDLE asyncThreadsSemaphore;
  HANDLE syncedThreadsSemaphore;
};
namespace win32
{


  struct PrmWorkerThreadEntry
  {
    thread::ThreadInfo* threadInfo;
    HANDLE semaphore;
    // bs::TaskQueue* queue;
  };
  DWORD WINAPI thread_worker( LPVOID void_parameter )
  {
    PrmWorkerThreadEntry* parameter = (PrmWorkerThreadEntry*) void_parameter;
    thread::ThreadInfo* threadInfo = parameter->threadInfo;
    HANDLE semaphore = parameter->semaphore;
    // bs::TaskQueue* queue = parameter->queue;
    parameter->threadInfo = nullptr;

    for ( ;;)
    {
      thread::pause_thread_if_requested( threadInfo );
      int  get_task_from_queue = 0;
      if ( get_task_from_queue )
      {

      }
      else
      {
        threadInfo->isPaused.compare_exchange( 1, 0 );
        WaitForSingleObjectEx( semaphore, INFINITE, FALSE );
        threadInfo->isPaused.compare_exchange( 0, 1 );
      }

      thread::sleep( 2 );
    }
  }

  void create_worker_thread( thread::ThreadInfo& threadInfo, HANDLE semaphore )
  {
    win32::PrmWorkerThreadEntry workerThreadParameter = {};
    workerThreadParameter.threadInfo = &threadInfo;
    workerThreadParameter.semaphore = semaphore;
    CloseHandle( CreateThread( 0, 0, win32::thread_worker, &workerThreadParameter, 0, (LPDWORD) &workerThreadParameter.threadInfo->id ) );

    while ( workerThreadParameter.threadInfo != nullptr )
    {
      thread::sleep( 0 );
    }
  }

  void push_async_task( bs::taskfn* fn, bs::Task* out_task )
  {

    //increment

    ReleaseSemaphore( global::asyncThreadsSemaphore, 1, 0 );
  }

  void push_synced_task( bs::taskfn* fn )
  {

    ReleaseSemaphore( global::syncedThreadsSemaphore, 1, 0 );
  }

  void wait_for_synced_tasks()
  {


  }

  u32 init_worker_threads()
  {
    s32 initialCount = 0;
    global::asyncThreadsSemaphore = CreateSemaphoreEx( 0, initialCount, global::ASYNC_THREAD_COUNT, 0, 0, SEMAPHORE_ALL_ACCESS );
    global::syncedThreadsSemaphore = CreateSemaphoreEx( 0, initialCount, global::SYNCED_THREAD_COUNT, 0, 0, SEMAPHORE_ALL_ACCESS );

    char const* asyncThreadNames[] =
    {
      "thread_async_worker_0",
      "thread_async_worker_1",
    };

    for ( s32 i = 0; i < global::ASYNC_THREAD_COUNT; ++i )
    {
      thread::ThreadInfo& new_worker_thread = global::asyncThreads[i];
      new_worker_thread.name = asyncThreadNames[i];
      create_worker_thread( new_worker_thread, global::asyncThreadsSemaphore );
    }

    char const* syncedThreadNames[] =
    {
      "thread_synced_worker_0",
      "thread_synced_worker_1",
      "thread_synced_worker_2",
      "thread_synced_worker_3",
      "thread_synced_worker_4",
      "thread_synced_worker_5",
    };
    for ( s32 i = 0; i < global::SYNCED_THREAD_COUNT; ++i )
    {
      thread::ThreadInfo& new_worker_thread = global::syncedThreads[i];
      new_worker_thread.name = syncedThreadNames[i];
      create_worker_thread( new_worker_thread, global::syncedThreadsSemaphore );
    }

    return 1;
  }
};


//stuff:

  // using threadEntryfn = void( thread::ThreadInfo*&, void* parameter );
  // struct PrmBrewingStationWindowsThread
  // {
  //   thread::ThreadInfo* threadInfo;
  //   threadEntryfn* entryFunction;
  //   void* parameter;
  // };
  // DWORD WINAPI brewing_station_windows_thread( LPVOID void_parameter )
  // {
  //   PrmBrewingStationWindowsThread* parameter = (PrmBrewingStationWindowsThread*) void_parameter;
  //   parameter->entryFunction( parameter->threadInfo, parameter->parameter );
  //   return 0;
  // }

  // void create_thread( thread::ThreadInfo& out_threadInfo, threadEntryfn* entryFunction, void* parameter )
  // {
  //   PrmBrewingStationWindowsThread brewingStationWindowsThreadParameter = {};
  //   brewingStationWindowsThreadParameter.threadInfo = &out_threadInfo;
  //   brewingStationWindowsThreadParameter.entryFunction = entryFunction;
  //   brewingStationWindowsThreadParameter.parameter = parameter;

  //   CloseHandle( CreateThread( 0, 0, win32::brewing_station_windows_thread, &brewingStationWindowsThreadParameter, 0, (LPDWORD) &brewingStationWindowsThreadParameter.threadInfo->id ) );
  //   while ( brewingStationWindowsThreadParameter.threadInfo != nullptr )
  //   {
  //     thread::sleep( 0 );
  //   }
  // }