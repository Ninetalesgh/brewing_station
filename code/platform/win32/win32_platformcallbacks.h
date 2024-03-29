#pragma once

//#include "win32_file.h"
#include "win32_global.h"
#include "win32_thread.h"

#include <core/bs_debuglog.h>
#include <core/bs_texture.h>

#include <stdio.h>


s32 utf8_to_wchar( char const* utf8String, wchar_t* out_wcharString, s32 wcharLengthMax )
{
  s32 wcharLength = MultiByteToWideChar( CP_UTF8, 0, utf8String, -1, 0, 0 );
  assert( wcharLength <= wcharLengthMax );
  return MultiByteToWideChar( CP_UTF8, 0, utf8String, -1, out_wcharString, min( wcharLength, wcharLengthMax ) );
}

s32 wchar_to_utf8( wchar_t const* wcharString, char* out_utf8String, s32 utf8StringLengthMax )
{
  s32 utf8StringLength = WideCharToMultiByte( CP_UTF8, 0, wcharString, -1, 0, 0, 0, 0 );
  assert( utf8StringLength <= utf8StringLengthMax );
  return WideCharToMultiByte( CP_UTF8, 0, wcharString, -1, out_utf8String, min( utf8StringLength, utf8StringLengthMax ), 0, 0 );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
/////      Debug Log      ///////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

void win32_debug_log( bs::DebugLogFlags flags, char const* string, s32 size )
{
  //wchar_t wideChars[bs::debug::MAX_DEBUG_MESSAGE_LENGTH];
 // utf8_to_wchar( string, wideChars, array_count( wideChars ) );

 // OutputDebugStringA( string );
  printf( string );
  if ( flags & bs::DebugLogFlags::WRITE_TO_DEBUG_LOG_FILE )
  {
    static HANDLE debug_log_file = CreateFileW( L"debug.log", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
    s32 bytesWritten {};

    // LockFile(debug_log_file, dwPos, 0, dwBytesRead, 0); 
    WriteFile( debug_log_file, string, size, (LPDWORD) &bytesWritten, 0 );
    // UnlockFile(debug_log_file, dwPos, 0, dwBytesRead, 0);

  }
  if ( flags & bs::DebugLogFlags::SEND_TO_SERVER )
  {

  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
/////      Memory      //////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

void* win32_allocate( s64 size )
{
  log_info( "Allocating ", size, " Bytes of Application RAM." );
  // return malloc( size );
  return VirtualAlloc( 0, (s64) global::APP_MEMORY_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
}

void win32_free( void* allocationToFree )
{
  //free( allocationToFree );
  VirtualFree( allocationToFree, 0, MEM_RELEASE );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
/////      File IO      /////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_BS_PATH 512

//TODO precompiled asset poop, I guess for release purposes this can be fully hacked in
#include <precompiled_assets/precompiled_assets.generated.cpp>
bool win32_get_precompiled_asset( char const* name, void** out_data, u64* out_size )
{
  s32 precompiledAssetCount = array_count( compiledasset::assetIndex );
  for ( s32 i = 0; i < precompiledAssetCount; ++i )
  {
    if ( bs::string_match( compiledasset::assetIndex[i].name, name ) )
    {
      if ( out_data )
      {
        *out_data = compiledasset::assetIndex[i].data;
      }
      if ( out_size )
      {
        *out_size = compiledasset::assetIndex[i].size;
      }

      return true;
    }
  }

  return false;
}


bool win32_get_file_info( char const* filePath, u64* out_fileSize )
{
  bool result = false;
  wchar_t wideChars[MAX_BS_PATH];
  utf8_to_wchar( filePath, wideChars, MAX_BS_PATH );

  _WIN32_FIND_DATAW findData;
  HANDLE findHandle = FindFirstFileW( wideChars, &findData );
  if ( findHandle != INVALID_HANDLE_VALUE )
  {
    FindClose( findHandle );
    if ( out_fileSize )
    {
      *out_fileSize = u64( findData.nFileSizeLow ) + (u64( findData.nFileSizeHigh ) << 32);
    }
    result = true;
  }
  else
  {
    result = false; //file or directory doesn't exist 
  }

  return result;
}

bool win32_load_file_part_fn( char const* filePath, u64 readOffset, void* targetBuffer, u32 bufferSize )
{
  if ( readOffset )
  {
    BREAK; //TODO
  }

  bool result = false;
  if ( targetBuffer )
  {
    wchar_t wideChars[MAX_BS_PATH];
    utf8_to_wchar( filePath, wideChars, MAX_BS_PATH );
    HANDLE fileHandle = CreateFileW( wideChars,
                                     GENERIC_READ,
                                     FILE_SHARE_READ, 0,
                                     OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL, 0 );

    if ( fileHandle != INVALID_HANDLE_VALUE )
    {
      DWORD bytesRead;
      if ( ReadFile( fileHandle, targetBuffer, bufferSize, &bytesRead, 0 ) )
      {
        result = true;
      }

      CloseHandle( fileHandle );
    }
  }

  return result;
}

bool win32_write_file_fn( char const* filePath, void const* data, u32 size, bsp::WriteFileFlags flags )
{
  bool result = true;
  wchar_t wideChars[MAX_BS_PATH];
  utf8_to_wchar( filePath, wideChars, MAX_BS_PATH );
  HANDLE fileHandle = nullptr;

  DWORD writeMode = (flags == bsp::WriteFileFlags::APPEND_OR_FAIL) ? FILE_APPEND_DATA : GENERIC_WRITE;
  DWORD createMode = (flags == bsp::WriteFileFlags::OVERWRITE_OR_CREATE_NEW) ? CREATE_ALWAYS : OPEN_EXISTING;

  fileHandle = CreateFileW( wideChars, writeMode, 0, 0, createMode, FILE_ATTRIBUTE_NORMAL, 0 );

  if ( fileHandle != INVALID_HANDLE_VALUE )
  {
    DWORD bytesWritten;
    u8 const* reader = (u8 const*) data;
    u32 sizeLeft = size;
    u32 const MAX_WRITE = (u32) KiloBytes( 4 );

    while ( sizeLeft > 0 )
    {
      if ( WriteFile( fileHandle, reader, min( MAX_WRITE, sizeLeft ), &bytesWritten, 0 ) )
      {
        assert( bytesWritten <= sizeLeft );
        sizeLeft -= bytesWritten;
        reader += bytesWritten;
      }
      else
      {
        result = false;
        BREAK;
        //  log_error( "[WIN32_FILE]", "ERROR - couldn't write data to file: ", filePath );
        break;
      }
    }
  }
  else
  {
    result = false;
  }

  CloseHandle( fileHandle );

  return result;
}

bool win32_create_directory( char const* directoryPath )
{
  wchar_t wideChars[MAX_BS_PATH];
  utf8_to_wchar( directoryPath, wideChars, MAX_BS_PATH );

  return CreateDirectoryW( wideChars, NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
/////      System      //////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

void win32_shutdown()
{
  global::running = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
/////      --------      ////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

#include "win32_opengl.h"

void register_callbacks( bsp::PlatformCallbacks* platform )
{
  //logging
  platform->debug_log = &win32_debug_log;

  //allocations for larger chunks
  platform->allocate_new_app_memory = &win32_allocate;
  platform->free_app_memory = &win32_free;

  //file IO
  platform->get_file_info = &win32_get_file_info;
  platform->load_file_part = &win32_load_file_part_fn;
  platform->write_file = &win32_write_file_fn;
  platform->create_directory = &win32_create_directory;
  platform->get_precompiled_asset = &win32_get_precompiled_asset;

  //task scheduling
  platform->push_low_priority_task = &win32::push_async_task;
  platform->push_high_priority_task = &win32::push_synced_task;
  platform->wait_for_high_priority_tasks = &win32::complete_synced_tasks;

  //graphics
  platform->allocate_mesh = &opengl::allocate_mesh;
  platform->free_mesh = &opengl::free_mesh;
  platform->allocate_texture = &opengl::allocate_texture;
  platform->free_texture = &opengl::free_texture;
  platform->create_shader_program = &opengl::create_shader_program;
  platform->render_custom_bitmap = &opengl::render_custom_bitmap;
  //system
  platform->shutdown = &win32_shutdown;
}

