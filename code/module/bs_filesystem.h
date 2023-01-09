#pragma once

#include <common/bscommon.h>


namespace bsm
{
  struct File
  {
    void* data;
    u64 size;
  };

  struct FileSystem;

  using MountPathID = s32;

  [[nodiscard]]
  FileSystem* create_filesystem();
  void destroy_filesystem( FileSystem* );

  //keep the return value if you need the mounted path for writing, for reading it doesn't matter
  //path can be absolute or relative to the initial mounted path
  MountPathID mount_path_to_filesystem( FileSystem*, char const* path );

  char const* get_mounted_path_by_id( FileSystem* fs, MountPathID mountPathID );

  bool unmount_path_from_filesystem( FileSystem*, char const* path ) { return false; } //TODO

  [[nodiscard]]
  File* load_file( FileSystem*, char const* path, MountPathID* out_mountPathID = nullptr );

  //returns false if the file wasn't loaded by this file system
  bool unload_file( FileSystem*, File* );

  bool find_file( FileSystem*, char const* path, MountPathID* out_mountPathID = nullptr );
  bool file_exists( FileSystem*, char const* path, MountPathID mountPathID = 0 );

  bool write_file( FileSystem*, char const* path, void const* data, u32 size, MountPathID mountPathID = 0 );
  bool append_file( FileSystem*, char const* path, void const* data, u32 size, MountPathID mountPathID = 0 );

};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////cpp//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include <platform/bs_platform.h>
#include <core/bsthread.h>

namespace bsm
{
  struct FileSystem
  {
    constexpr static s32 FS_LL_BLOCK_SIZE = 1024;
    constexpr static s32 MAX_LOADED_FILES = 256;
    char mountedPaths[FS_LL_BLOCK_SIZE];
    File loadedFiles[MAX_LOADED_FILES];
    atomic32 fileSlotLock;
    char* writer;
    FileSystem* next;
    s32 mountedPathsCount;
    s32 loadedFilesCount;
  };

  FileSystem* create_filesystem()
  {
    FileSystem* fs = (FileSystem*) bsp::platform->allocate_to_zero( sizeof( FileSystem ) );
    fs->writer = fs->mountedPaths;
    fs->mountedPathsCount = 0;
    fs->loadedFilesCount = 0;
    fs->next = nullptr;
    return fs;
  }

  void destroy_filesystem( FileSystem* fs )
  {
    while ( fs )
    {
      FileSystem* part = fs;
      fs = fs->next;
      //TODO unload loaded files
      bsp::platform->free( part );
    }

  }

  bool is_absolute_path( char const* path )
  {
    return bs::string_contains( path, ":/" ) == (path + 1) || bs::string_contains( path, ":\\" ) == (path + 1);
  }

  s32 path_format_replace_slashes( char* destination, s32 capacity, char const* path )
  {
    char const* reader = path;
    char* writer = destination;
    char* end = writer + capacity - 1;
    while ( *reader && writer < end )
    {
      if ( *reader != '\\' )
      {
        *writer = *reader;
      }
      else
      {
        *writer = '/';
      }

      ++writer;
      ++reader;
    }

    return s32( writer - destination );
  }

  void path_remove_node( char* path )
  {
    char* reader = path + bs::string_length( path ) - 1;
    *reader = '\0';
    while ( *reader != '/' )
    {
      *reader-- = '\0';
    }
  }

  bool mount_absolute_path_to_filesystem( FileSystem* fs, char const* path )
  {
    constexpr s32 STACK_CAPACITY = 512;
    char tmp[STACK_CAPACITY + 1] = {};

    s32 length = path_format_replace_slashes( tmp, STACK_CAPACITY, path );
    if ( bs::string_contains( path, ".exe" ) )
    {
      path_remove_node( tmp );
    }
    else if ( tmp[length - 1] != '/' )
    {
      tmp[length] = '/';
    }

    char* end = fs->mountedPaths + FileSystem::FS_LL_BLOCK_SIZE;

    s32 capacity = s32( end - fs->writer );
    length = bs::string_length( tmp );

    if ( length < capacity )
    {
      fs->writer += bs::string_format( fs->writer, capacity, tmp );
    }
    else
    {
      BREAK; //TODO allocate new file system block and check there
      return false;
    }

    return true;
  }

  bool mount_relative_path_to_filesystem( FileSystem* fs, char const* path )
  {
    constexpr s32 STACK_CAPACITY = 512;
    char tmp[STACK_CAPACITY + 1] = {};

    if ( *fs->mountedPaths == '\0' )
    {
      //can't mount relative path, relative to what?
      return false;
    }

    bs::string_format( tmp, STACK_CAPACITY, fs->mountedPaths );

    char const* reader = path;

    while ( *reader != '\0' )
    {
      if ( *reader == '/' )
      {
        ++reader;
      }
      else if ( bs::string_match( reader, "../" ) )
      {
        path_remove_node( tmp );
        reader += 3;
      }
      else
      {
        char* writer = tmp + bs::string_length( tmp );
        while ( *reader != '/' && *reader != '\0' )
        {
          *writer++ = *reader++;
        }
        *writer++ = '/';
      }
    }

    return mount_absolute_path_to_filesystem( fs, tmp );
  }

  MountPathID mount_path_to_filesystem( FileSystem* fs, char const* path )
  {
    bool succeeded = false;
    if ( is_absolute_path( path ) )
    {
      succeeded = mount_absolute_path_to_filesystem( fs, path );
    }
    else
    {
      succeeded = mount_relative_path_to_filesystem( fs, path );
    }

    return succeeded ? fs->mountedPathsCount++ : -1;
  }

  char const* get_mounted_path_by_id( FileSystem* fs, MountPathID mountPathID )
  {
    char const* result = fs->mountedPaths;
    while ( mountPathID-- )
    {
      result += bs::string_length( result ) + 1;
    }
    return result;
  }

  bool find_first_valid_file_path( FileSystem* fs, char* out_validPath, s32 capacity, char const* relativePath, MountPathID* out_mountPathID )
  {
    if ( *relativePath == '/' ) ++relativePath;

    for ( s32 i = 0; i < fs->mountedPathsCount; ++i )
    {
      bs::string_format( out_validPath, capacity, get_mounted_path_by_id( fs, i ), relativePath );

      if ( bsp::platform->get_file_info( out_validPath, nullptr ) )
      {
        if ( out_mountPathID )
        {
          *out_mountPathID = i;
        }
        return true;
      }
    }

    return false;
  }

  File* get_next_free_file_slot( FileSystem* fs )
  {
    File* result = nullptr;
    for ( s32 i = 0; i < fs->MAX_LOADED_FILES; ++i )
    {
      if ( fs->loadedFiles[i].data == nullptr )
      {
        result = &fs->loadedFiles[i];
        fs->loadedFilesCount++;
        break;
      }
    }

    return result;
  }

  File* load_file( FileSystem* fs, char const* path, MountPathID* out_mountPathID )
  {
    if ( !path )
    {
      return nullptr;
    }

    char actualPath[1024];
    if ( !find_first_valid_file_path( fs, actualPath, 1024, path, out_mountPathID ) )
    {
      return nullptr;
    }

    u64 size = 0;
    if ( !bsp::platform->get_file_info( actualPath, &size ) )
    {
      return nullptr;
    }

    void* data = bsp::platform->allocate( size );
    if ( data )
    {
      if ( bsp::platform->load_file_part( actualPath, 0, data, (u32) size ) )
      {
        LOCK_SCOPE( fs->fileSlotLock );
        File* file = get_next_free_file_slot( fs );
        file->data = data;
        file->size = size;
        return file;
      }
      else
      {
        bsp::platform->free( data );
      }
    }

    return nullptr;
  }

  bool unload_file( FileSystem* fs, File* file )
  {
    File* begin = fs->loadedFiles;
    File* end = fs->loadedFiles + FileSystem::MAX_LOADED_FILES;

    if ( file > begin && file < end )
    {
      if ( file->data )
      {
        bsp::platform->free( file->data );
      }

      interlocked_decrement( (s32 volatile*) &fs->loadedFilesCount );
      file->data = nullptr;
      file->size = 0;

      return true;
    }

    return false;
  }

  bool find_file( FileSystem* fs, char const* path, MountPathID* out_mountPathID )
  {
    char actualPath[1024];

    if ( !find_first_valid_file_path( fs, actualPath, 1024, path, out_mountPathID ) )
    {
      return false;
    }

    return true;
  }

  bool file_exists( FileSystem* fs, char const* path, MountPathID mountPathID )
  {
    char actualPath[1024];
    if ( *path == '/' ) ++path;
    bs::string_format( actualPath, 1024, get_mounted_path_by_id( fs, mountPathID ), path );
    return bsp::platform->get_file_info( actualPath, nullptr );
  }

  bool write_file( FileSystem* fs, char const* path, void const* data, u32 size, MountPathID mountPathID )
  {
    char actualPath[1024];

    if ( *path == '/' ) ++path;
    bs::string_format( actualPath, 1024, get_mounted_path_by_id( fs, mountPathID ), path );

    bsp::WriteFileFlags flags = bsp::WriteFileFlags::OVERWRITE_OR_CREATE_NEW;

    return bsp::platform->write_file( actualPath, data, size, flags );
  }

  bool append_file( FileSystem* fs, char const* path, void const* data, u32 size, MountPathID mountPathID )
  {
    char actualPath[1024];

    if ( *path == '/' ) ++path;
    bs::string_format( actualPath, 1024, get_mounted_path_by_id( fs, mountPathID ), path );

    bsp::WriteFileFlags flags = bsp::WriteFileFlags::APPEND_OR_FAIL;
    return bsp::platform->write_file( actualPath, data, size, flags );
  }
};