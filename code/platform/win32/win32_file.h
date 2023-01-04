#pragma once

DEPRECATED

#include "win32_global.h"

//#include <module/bs_debuglog.h>
#include <core/bsfile.h>
#include <common/bsstring.h>
#include <common/bscommon.h>

#define MAX_BS_PATH 260
namespace win32
{
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

  s32 get_exe_path( char* out_exePath, s32 exePathLengthMax )
  {
    wchar_t wideChars[MAX_BS_PATH];
    s32 filePathLength = GetModuleFileNameW( 0, wideChars, MAX_BS_PATH );
    assert( filePathLength <= exePathLengthMax );
    return wchar_to_utf8( wideChars, out_exePath, exePathLengthMax );
  }

  u32 get_file_info( char const* filePath, bs::file::Info* out_fileInfo )
  {
    u32 result = 0;
    if ( out_fileInfo )
    {
      wchar_t wideChars[MAX_BS_PATH];
      utf8_to_wchar( filePath, wideChars, MAX_BS_PATH );

      _WIN32_FIND_DATAW findData;
      HANDLE findHandle = FindFirstFileW( wideChars, &findData );
      if ( findHandle != INVALID_HANDLE_VALUE )
      {
        FindClose( findHandle );
        out_fileInfo->size = u64( findData.nFileSizeLow ) + (u64( findData.nFileSizeHigh ) << 32);
        result = 1;
      }
      else
      {
        BREAK;
      }
    }
    else
    {
      BREAK;
    }

    return result;
  }

  u32 load_file_into_memory( char const* filePath, bs::file::Data* out_loadedFileData )
  {
    u32 result = 0;
    if ( out_loadedFileData )
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
        LARGE_INTEGER fileSize;
        if ( GetFileSizeEx( fileHandle, &fileSize ) )
        {
          if ( (u64) fileSize.QuadPart < MegaBytes( 512 ) )
          {
            out_loadedFileData->data = bs::memory::allocate( fileSize.QuadPart );
            if ( out_loadedFileData->data )
            {
              u32 fileSize32 = (u32) fileSize.QuadPart;
              DWORD bytesRead;
              if ( ReadFile( fileHandle, out_loadedFileData->data, fileSize32, &bytesRead, 0 )
                && (fileSize32 == bytesRead) )
              {
                out_loadedFileData->size = fileSize32;
                result = 1;
              }
              else
              {
                bs::memory::free( out_loadedFileData->data );
                (*out_loadedFileData) = {};
              }
            }
          }
          else
          {
            BREAK; //file a bit large, huh 
            //TODO read file in chunks
          }
        }

        CloseHandle( fileHandle );
      }
    }

    return result;
  }

  u32 append_file( char const* filePath, void const* data, u32 size )
  {
    u32 result = 1;
    wchar_t wideChars[MAX_BS_PATH];
    utf8_to_wchar( filePath, wideChars, MAX_BS_PATH );

    HANDLE fileHandle = CreateFileW( wideChars,
                                     FILE_APPEND_DATA,
                                     0, 0,
                                     OPEN_ALWAYS,
                                     FILE_ATTRIBUTE_NORMAL,
                                     0 );

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
          result = 0;
          log_error( "[WIN32_FILE] ERROR - couldn't write data to file: ", filePath );
          break;
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

  u32 write_file( char const* filePath, void const* data, u32 size )
  {
    u32 result = 1;
    wchar_t wideChars[MAX_BS_PATH];
    utf8_to_wchar( filePath, wideChars, MAX_BS_PATH );

    HANDLE fileHandle = CreateFileW( wideChars,
                                     GENERIC_WRITE,
                                     0, 0,
                                     CREATE_ALWAYS,
                                     FILE_ATTRIBUTE_NORMAL,
                                     0 );

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
          result = 0;
          log_error( "[WIN32_FILE]", "ERROR - couldn't write data to file: ", filePath );
          break;
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

  void fetch_paths()
  {
    char pathCollection[2048];
    char* writer = pathCollection;
    s32 capacity = array_count( pathCollection );
    s32 writtenBytes = win32::get_exe_path( writer, capacity );
    writer += writtenBytes;
    capacity -= writtenBytes;
    //get asset path
  }

  char* generate_hex_array_string( u8 const* data, u32 size, char const* assetName, u32* out_size )
  {
    s32 const entriesPerLine = 16;
    u32 hexArraySize = size * 5 + ((size / entriesPerLine) + ((size >= entriesPerLine && size % entriesPerLine) ? 1 : 0)) * 2;

    char const preContent0[] = "\r\nchar const ";
    char const preContent1[] = "[] = { \r\n";
    char const postContent[] = "};\r\n";
    u32 nameLength = bs::string_length( assetName );
    u32 preContent0Length = bs::string_length( preContent0 );
    u32 preContent1Length = bs::string_length( preContent1 );
    u32 postContentLength = bs::string_length( postContent );

    hexArraySize += nameLength + preContent0Length + preContent1Length + postContentLength;

    char* hexArray = (char*) bs::memory::allocate( hexArraySize );
    char* writer = hexArray;

    bs::memory::copy( writer, preContent0, preContent0Length );
    writer += preContent0Length;
    bs::memory::copy( writer, assetName, nameLength );
    writer += nameLength;
    bs::memory::copy( writer, preContent1, preContent1Length );
    writer += preContent1Length;
    for ( u32 i = 0; i < size; ++i )
    {
      *writer++ = '0';
      *writer++ = 'x';
      u8 tmp = data[i] >> 4;
      tmp = tmp < 10 ? 48 + tmp : 87 + tmp;
      *writer++ = (char) tmp;
      tmp = data[i] & 0x0f;
      tmp = tmp < 10 ? 48 + tmp : 87 + tmp;
      *writer++ = (char) tmp;
      *writer++ = ',';
      if ( !((i + 1) % entriesPerLine) )
      {
        *writer++ = '\r';
        *writer++ = '\n';
      }
    }
    bs::memory::copy( writer, postContent, postContentLength );
    writer += postContentLength;

    assert( u32( writer - hexArray ) == hexArraySize );

    *out_size = hexArraySize;
    return hexArray;
  }

  struct AssetToCompile
  {
    char const* name;
    void* data;
    u32 size;
  };

  void generate_compiled_assets_file( AssetToCompile* assetArray, s32 assetCount )
  {
    char const preContent[] = "#pragma warning(push)\r\n#pragma warning(disable:4309)\r\n#pragma warning(push)\r\n#pragma warning(disable:4838)\r\nnamespace compiledasset\r\n{";
    write_file( "w:/code/compiled_assets", preContent, bs::string_length( preContent ) );

    for ( s32 i = 0; i < assetCount; ++i )
    {
      u32 hexArraySize;
      char* hexArrayString = generate_hex_array_string( (u8 const*) assetArray[i].data, (u32) assetArray[i].size, assetArray[i].name, &hexArraySize );
      append_file( "w:/code/compiled_assets", hexArrayString, hexArraySize );
      bs::memory::free( hexArrayString );
    }

    // char* hexArrayString;
    // u32 hexArraySize;
    // generate_hex_array_string( (u8 const*) ttf.data, (u32) ttf.size, "DEFAULT_FONT", &hexArrayString, &hexArraySize );
    // append_file( "w:/code/compiled_assets", hexArrayString, hexArraySize );
    // bs::memory::free( hexArrayString );
    // bs::memory::free( ttf.data );

    char const postContent[] = "};\r\n#pragma warning(pop)\r\n#pragma warning(pop)\r\n";
    append_file( "w:/code/compiled_assets", postContent, bs::string_length( postContent ) );
  }
};