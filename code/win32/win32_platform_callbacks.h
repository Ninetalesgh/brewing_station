#pragma once

#include "platform.h"
#include "common/threading.h"

#include "win32_global.h"
#include <windows.h>

namespace win32
{
  u32                      debug_GetFileInfo( char const* filename, FileInfo* out_fileInfo );
  void                     debug_FreeFile( void* data );
  platform::ReadFileResult debug_ReadFile( char const* filename, u32 maxSize, void* out_data );
  u32                      debug_WriteFile( char const* filename, void* const* data, s32 const* size, s32 count );

  threading::ThreadInfo* create_thread( platform::thread_call* entryFunction, void* parameter );

};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////cpp//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


DWORD brewing_station_thread( void* parameter );

namespace win32
{
  namespace internal
  {
    struct BrewingStationThreadLauncherParameter
    {
      threading::ThreadInfo* threadInfo;
      platform::thread_call* entryFunction;
      void* passInParameter;
    };
    threading::ThreadInfo* try_add_thread_to_platform_data( PlatformData* platform )
    {
      threading::ThreadInfo* result {};

      for ( s32 i = 0; i < APP_THREAD_COUNT_MAX; ++i )
      {
        if ( !platform->threads[i].id )
        {
          result = &platform->threads[i];
          break;
        }
      }

      return result;
    }
  };

  threading::ThreadInfo* create_thread( platform::thread_call* entryFunction, void* parameter )
  {
    threading::ThreadInfo* threadInfo = internal::try_add_thread_to_platform_data( &global::win32Data.platformData );

    if ( threadInfo != nullptr )
    {
      internal::BrewingStationThreadLauncherParameter threadLaunchParam {};
      threadLaunchParam.threadInfo      = threadInfo;
      threadLaunchParam.entryFunction   = entryFunction;
      threadLaunchParam.passInParameter = parameter;
      //TODO remove this from the stack lol

      CloseHandle( CreateThread( 0, 0, brewing_station_thread, &threadLaunchParam, 0, (LPDWORD) &threadInfo->id ) );
    }
    else
    {
      BREAK;
    }

    return threadInfo;
  }

  u32 debug_GetFileInfo( char const* filename, FileInfo* out_fileInfo )
  {
    _WIN32_FIND_DATAA findData;
    HANDLE findHandle = FindFirstFileA( APP_FILENAME, &findData );
    if ( findHandle != INVALID_HANDLE_VALUE )
    {
      FindClose( findHandle );
      out_fileInfo->size = u64( findData.nFileSizeLow ) + (u64( findData.nFileSizeHigh ) << 32);
      return 1;
    }

    return 0;
  }

  void debug_FreeFile( void* data )
  {
    VirtualFree( data, 0, MEM_RELEASE );
  }

  platform::ReadFileResult debug_ReadFile( char const* filename, u32 maxSize, void* out_data )
  {
    //TODO
    platform::ReadFileResult result = {};

    HANDLE fileHandle = CreateFileA( filename,
                                     GENERIC_READ,
                                     FILE_SHARE_READ, 0,
                                     OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL, 0 );

    if ( fileHandle != INVALID_HANDLE_VALUE )
    {
      LARGE_INTEGER fileSize;
      if ( GetFileSizeEx( fileHandle, &fileSize ) )
      {
        result.data = VirtualAlloc( 0, fileSize.QuadPart, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
        assert( fileSize.QuadPart <= 0xFFFFFFFF );

        u32 fileSize32 = (u32) fileSize.QuadPart;

        DWORD bytesRead;
        if ( ReadFile( fileHandle, result.data, fileSize32, &bytesRead, 0 )
          && (fileSize32 == bytesRead) )
        {
          result.size = fileSize32;
        }
        else
        {
          debug_FreeFile( result.data );
          result = {};
        }
      }

      CloseHandle( fileHandle );
    }

    return result;
  }

  u32 debug_WriteFile( char const* filename, void* const* data, s32 const* size, s32 count )
  {
    u32 result = 1;

    // HANDLE result = CreateFileA( filename,
    //                              GENERIC_WRITE,
    //                              FILE_SHARE_READ,
    //                              0,
    //                              CREATE_ALWAYS,
    //                              FILE_ATTRIBUTE_NORMAL,
    //                              0 );

    HANDLE fileHandle = CreateFileA( filename,
                                     GENERIC_WRITE,
                                     0, 0,
                                     CREATE_ALWAYS,
                                     FILE_ATTRIBUTE_NORMAL,
                                     0 );

    if ( fileHandle != INVALID_HANDLE_VALUE )
    {
      for ( s32 i = 0; i < count; ++i )
      {
        DWORD bytesWritten;
        u8* reader = (u8*) data[i];
        s32 sizeLeft = size[i];
        constexpr_member s32 MAX_WRITE = 4096;

        while ( sizeLeft > 0 )
        {
          if ( WriteFile( fileHandle, reader, min( MAX_WRITE, sizeLeft ), &bytesWritten, 0 ) )
          {
            sizeLeft -= (s32) bytesWritten;
            reader += bytesWritten;
          }
          else
          {
            result = 0;
            log_error( "[WIN32_FILE] ERROR - couldn't write data to file: ", filename );
            break;
          }
        }
      }
    }
    else
    {
      result = 0;
    }

    CloseHandle( fileHandle );

    return result;
  }
};

DWORD brewing_station_thread( void* parameter )
{
  win32::internal::BrewingStationThreadLauncherParameter* threadLaunchParameter = (win32::internal::BrewingStationThreadLauncherParameter*) parameter;
  threading::ThreadInfo* threadInfo = threadLaunchParameter->threadInfo;

  threadLaunchParameter->entryFunction( threadInfo, threadLaunchParameter->passInParameter );
  threadInfo->hasReturned.increment();

  return 0;
}