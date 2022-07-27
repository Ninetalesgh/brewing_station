#pragma once

#include <core/bstask.h>
#include "win32_global.h"

namespace thread
{
  void request_pause( ThreadInfo* );
  void request_unpause( ThreadInfo* );
  void wait_for_thread_to_pause( ThreadInfo* );
  void pause_thread_if_requested( ThreadInfo* );
  void pause_thread_if_requested( ThreadInfo*, s32 millisecondsSleepPerPoll );

  void write_barrier();
};

namespace win32
{
  u32 init_worker_threads();

  //returns 0 if the queue is full and the task wasn't added
  //out_taskState is optional 
  u32 push_async_task( bs::Task const& task, bs::TaskState volatile* out_taskState );

  //returns 0 if the queue is full and the task wasn't added
  u32 push_synced_task( bs::Task const& task, bs::TaskState volatile* out_taskState );

  //join the synced worker threads until all tasks are completed
  void complete_synced_tasks();

  using threadfn = void( thread::ThreadInfo* );
  void for_all_async_worker_threads( threadfn* fn );
  void for_all_synced_worker_threads( threadfn* fn );
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////inl//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

namespace thread
{
  INLINE void request_pause( ThreadInfo* threadInfo )
  {
    threadInfo->requestPause.compare_exchange( 1, 0 );
  }

  INLINE void request_unpause( ThreadInfo* threadInfo )
  {
    threadInfo->requestPause.compare_exchange( 0, 1 );
  }

  INLINE void wait_for_thread_to_pause( ThreadInfo* threadInfo )
  {
    while ( !threadInfo->isPaused )
    {
      thread::sleep( 0 );
    }
  }

  void pause_thread_if_requested( ThreadInfo* threadInfo, s32 millisecondsSleepPerPoll )
  {
    if ( threadInfo->requestPause )
    {
      threadInfo->isPaused.compare_exchange( 1, 0 );

      while ( threadInfo->requestPause )
      {
        thread::sleep( millisecondsSleepPerPoll );
      }
      threadInfo->isPaused.compare_exchange( 0, 1 );
    }
  }

  INLINE void pause_thread_if_requested( ThreadInfo* threadInfo )
  {
    pause_thread_if_requested( threadInfo, 0 );
  }

  INLINE void write_barrier()
  {
    __faststorefence();
  }
};



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////cpp/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <synchapi.h>

namespace win32
{
  struct TaskQueueEntry
  {
    bs::Task task;
    bs::TaskState volatile* taskState;
  };

  constexpr s32 volatile MAX_TASK_COUNT = 256;
  struct TaskQueue
  {
    TaskQueueEntry tasks[MAX_TASK_COUNT];
    HANDLE semaphore;
    s32 volatile pushIndex;
    s32 volatile popIndex;
    atomic32 pushBarrierObject;
  };
};

namespace global
{
  win32::TaskQueue asyncTaskQueue;
  win32::TaskQueue syncedTaskQueue;
};

namespace win32
{
  u32 push_task( TaskQueue* queue, bs::Task const& task, bs::TaskState volatile* out_taskState )
  {
    u32 successfullyPushedTask = false;
    {
      LOCK_SCOPE( queue->pushBarrierObject );
      s32 pushIndex = queue->pushIndex;
      s32 nextPushIndex = (pushIndex + 1) % MAX_TASK_COUNT;
      if ( nextPushIndex != queue->popIndex )
      {
        TaskQueueEntry& newEntry = queue->tasks[pushIndex];
        newEntry.task = task;
        if ( out_taskState )
        {
          newEntry.taskState = out_taskState;
          *newEntry.taskState = bs::TaskState::IN_QUEUE;
        }
        else
        {
          newEntry.taskState = nullptr;
        }

        successfullyPushedTask = true;
      }

      s32 check = interlocked_compare_exchange( &queue->pushIndex, nextPushIndex, pushIndex );
      assert( check == pushIndex );
    }

    if ( successfullyPushedTask )
    {
      ReleaseSemaphore( queue->semaphore, 1, 0 );
    }

    return successfullyPushedTask;
  }

  u32 pop_task( TaskQueue* queue )
  {
    //returns 0 if the queue was empty and no task could be fetched
    //returns 1 if the task was fetched and executed
    u32 result = false;
    for ( ;;)
    {
      s32 expectedIndex = queue->popIndex;
      if ( expectedIndex != queue->pushIndex )
      {
        TaskQueueEntry taskQueueEntry = queue->tasks[expectedIndex];
        s32 nextPopIndex = (expectedIndex + 1) % MAX_TASK_COUNT;
        s32 popIndex = interlocked_compare_exchange( &queue->popIndex, nextPopIndex, expectedIndex );
        if ( popIndex == expectedIndex )
        {
          //we're good to go to work
          bs::TaskState volatile* state = taskQueueEntry.taskState;
          if ( state )
          {
            interlocked_compare_exchange( (volatile s32*) state, (s32) bs::TaskState::IN_PROGRESS, (s32) bs::TaskState::IN_QUEUE );
            taskQueueEntry.task.function( taskQueueEntry.task.parameter );
            interlocked_compare_exchange( (volatile s32*) state, (s32) bs::TaskState::COMPLETED, (s32) bs::TaskState::IN_PROGRESS );
          }

          result = true;
          break;
        }
        else
        {
          //we try again
          continue;
        }
      }
      else
      {
        //queue is empty
        break;
      }
    }

    return result;
  }

  INLINE u32 push_async_task( bs::Task const& task, bs::TaskState volatile* out_taskState )
  {
    return push_task( &global::asyncTaskQueue, task, out_taskState );
  }

  INLINE u32 push_synced_task( bs::Task const& task, bs::TaskState volatile* out_taskState )
  {
    return push_task( &global::syncedTaskQueue, task, out_taskState );
  }

  void complete_synced_tasks()
  {
    while ( pop_task( &global::syncedTaskQueue ) ) {}
    win32::for_all_synced_worker_threads( &thread::wait_for_thread_to_pause );
  }

  struct PrmWorkerThreadEntry
  {
    thread::ThreadInfo* threadInfo;
    TaskQueue* taskQueue;
  };
  DWORD WINAPI thread_worker( LPVOID void_parameter )
  {
    PrmWorkerThreadEntry* parameter = (PrmWorkerThreadEntry*) void_parameter;
    thread::ThreadInfo* threadInfo = parameter->threadInfo;
    TaskQueue* queue = parameter->taskQueue;

    thread::write_barrier();
    parameter->threadInfo = nullptr;

    for ( ;;)
    {
      thread::pause_thread_if_requested( threadInfo );

      if ( !pop_task( queue ) )
      {
        threadInfo->isPaused.compare_exchange( 1, 0 );
        WaitForSingleObjectEx( queue->semaphore, INFINITE, FALSE );
        threadInfo->isPaused.compare_exchange( 0, 1 );
      }
    }
  }

  void wait_for_synced_tasks()
  {
  }

  u32 init_worker_threads()
  {
    global::asyncTaskQueue.semaphore = CreateSemaphoreEx( 0, 0/*initialCount*/, global::ASYNC_THREAD_COUNT, 0, 0, SEMAPHORE_ALL_ACCESS );
    global::syncedTaskQueue.semaphore = CreateSemaphoreEx( 0, 0/*initialCount*/, global::SYNCED_THREAD_COUNT, 0, 0, SEMAPHORE_ALL_ACCESS );

    char const* const syncedThreadNames[] =
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

      win32::PrmWorkerThreadEntry workerThreadParameter = {};
      workerThreadParameter.threadInfo = &new_worker_thread;
      workerThreadParameter.taskQueue = &global::syncedTaskQueue;
      CloseHandle( CreateThread( 0, 0, win32::thread_worker, &workerThreadParameter, 0, (LPDWORD) &workerThreadParameter.threadInfo->id ) );

      //wait until the thread is done launching to overwrite the stack again
      while ( workerThreadParameter.threadInfo != nullptr )
      {
        thread::sleep( 0 );
      }
    }

    char const* const asyncThreadNames[] =
    {
      "thread_async_worker_0",
      "thread_async_worker_1",
    };
    for ( s32 i = 0; i < global::ASYNC_THREAD_COUNT; ++i )
    {
      thread::ThreadInfo& new_worker_thread = global::asyncThreads[i];
      new_worker_thread.name = asyncThreadNames[i];

      win32::PrmWorkerThreadEntry workerThreadParameter = {};
      workerThreadParameter.threadInfo = &new_worker_thread;
      workerThreadParameter.taskQueue = &global::asyncTaskQueue;
      CloseHandle( CreateThread( 0, 0, win32::thread_worker, &workerThreadParameter, 0, (LPDWORD) &workerThreadParameter.threadInfo->id ) );

      //wait until the thread is done launching to overwrite the stack again
      while ( workerThreadParameter.threadInfo != nullptr )
      {
        thread::sleep( 0 );
      }
    }

    return 1;
  }

  INLINE void for_all_async_worker_threads( threadfn* fn )
  {
    for ( u32 i = 0; i < global::ASYNC_THREAD_COUNT; ++i )
    {
      if ( global::asyncThreads[i].id ) fn( &global::asyncThreads[i] );
    }
  }

  INLINE void for_all_synced_worker_threads( threadfn* fn )
  {
    for ( u32 i = 0; i < global::SYNCED_THREAD_COUNT; ++i )
    {
      if ( global::syncedThreads[i].id ) fn( &global::syncedThreads[i] );
    }
  }
};


// generic thread stuff to probably throw away:

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