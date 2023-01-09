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

  [[nodiscard]]
  FileSystem* create_filesystem();
  void destroy_filesystem( FileSystem* );

  bool mount_path_to_filesystem( FileSystem*, char const* path );

  File* load_file( FileSystem*, char const* path );

  bool write_file( FileSystem*, char const* path, void const* data, u32 size );
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////cpp//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include <platform/bs_platform.h>


namespace bsm
{
  struct FileSystem
  {
    constexpr static s32 FS_LL_BLOCK_SIZE = 1024;
    constexpr static s32 MAX_LOADED_FILES = 256;
    char mountedPaths[FS_LL_BLOCK_SIZE];
    File loadedFiles[MAX_LOADED_FILES];
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
    if ( bs::string_contains( path, "." ) )
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

  bool mount_path_to_filesystem( FileSystem* fs, char const* path )
  {
    bool result = false;
    if ( is_absolute_path( path ) )
    {
      result = mount_absolute_path_to_filesystem( fs, path );
    }
    else
    {
      result = mount_relative_path_to_filesystem( fs, path );
    }

    if ( result )
    {
      ++fs->mountedPathsCount;
    }

    return result;
  }

  char const* get_mounted_path_by_index( FileSystem* fs, s32 index )
  {
    char const* result = fs->mountedPaths;
    while ( index-- )
    {
      result += bs::string_length( result ) + 1;
    }
    return result;
  }

  bool find_first_valid_file_path( FileSystem* fs, char* out_validPath, s32 capacity, char const* relativePath )
  {
    for ( s32 i = 0; i < fs->mountedPathsCount; ++i )
    {
      bs::string_format( out_validPath, capacity, get_mounted_path_by_index( fs, i ), relativePath );
      u64 size;
      if ( bsp::platform->get_file_info( out_validPath, &size ) )
      {
        return true;
      }
    }

    return false;
  }

  File* get_next_free_file_slot( FileSystem* fs )
  {
    s32 currentIndex = fs->loadedFilesCount;
    while ( fs->loadedFiles[currentIndex].data )
    {
      currentIndex = (currentIndex + 1) % FileSystem::MAX_LOADED_FILES;
    }
    //TODO thread safe queue

    return nullptr;
  }

  File* load_file( FileSystem* fs, char const* path )
  {
    File result = {};

    if ( !path )
    {
      return nullptr;
    }
    if ( *path == '/' ) ++path;

    char actualPath[1024];

    if ( !find_first_valid_file_path( fs, actualPath, 1024, path ) )
    {
      return nullptr;
    }

    if ( !bsp::platform->get_file_info( actualPath, &result.size ) )
    {
      return nullptr;
    }

    result.data = bsp::platform->allocate( result.size );
    if ( result.data )
    {
      if ( bsp::platform->load_file_part( actualPath, 0, result.data, (u32) result.size ) )
      {
        //TODO USE A PROPER QUEUE HERE
        fs->loadedFiles[fs->loadedFilesCount] = result;
        return &fs->loadedFiles[fs->loadedFilesCount++];
      }
      else
      {
        bsp::platform->free( result.data );
      }
    }

    return nullptr;
  }

  bool write_file( FileSystem* fs, char const* path, void const* data, u32 size )
  {
    char actualPath[1024];

    //VFS

    return (bsp::platform->write_file( actualPath, data, size ));
  }

};