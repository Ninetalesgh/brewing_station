#pragma once
#include "win32_global.h"

#include <core/bsdebuglog.h>
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

  u64 get_file_size( char const* filepath )
  {
    u64 result = 0;
    wchar_t wideChars[MAX_BS_PATH];
    utf8_to_wchar( filepath, wideChars, MAX_BS_PATH );

    _WIN32_FIND_DATAW findData;
    HANDLE findHandle = FindFirstFileW( wideChars, &findData );
    if ( findHandle != INVALID_HANDLE_VALUE )
    {
      FindClose( findHandle );
      result = u64( findData.nFileSizeLow ) + (u64( findData.nFileSizeHigh ) << 32);
    }
    else
    {
      BREAK;
    }

    return result;
  }


  bs::file::LoadedFile load_into_memory( char const* filepath )
  {
    bs::file::LoadedFile loadedFile {};

    wchar_t wideChars[MAX_BS_PATH];
    utf8_to_wchar( filepath, wideChars, MAX_BS_PATH );
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
          loadedFile.data = bs::memory::allocate( fileSize.QuadPart );

          u32 fileSize32 = (u32) fileSize.QuadPart;
          DWORD bytesRead;
          if ( ReadFile( fileHandle, loadedFile.data, fileSize32, &bytesRead, 0 )
            && (fileSize32 == bytesRead) )
          {
            loadedFile.size = fileSize32;
          }
          else
          {
            bs::memory::free( loadedFile.data );
            loadedFile = {};
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

    return loadedFile;
  }

  void write_file();

  u32 append_file( char const* filepath, void const* data, u32 size )
  {
    u32 result = 1;
    wchar_t wideChars[MAX_BS_PATH];
    utf8_to_wchar( filepath, wideChars, MAX_BS_PATH );

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
          log_error( "[WIN32_FILE] ERROR - couldn't write data to file: ", filepath );
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

  u32 write_file( char const* filepath, void const* data, u32 size )
  {
    u32 result = 1;
    wchar_t wideChars[MAX_BS_PATH];
    utf8_to_wchar( filepath, wideChars, MAX_BS_PATH );

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
          log_error( "[WIN32_FILE]", "ERROR - couldn't write data to file: ", filepath );
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

  INLINE void byte_to_hex_as_string( u8 byte, char* out_4ByteString )
  {
    char* writer = out_4ByteString;
    writer[0] = '0';
    writer[1] = 'x';

    u8 tmp = byte >> 4;
    tmp = tmp < 10 ? 48 + tmp : 87 + tmp;
    writer[2] = tmp;
    tmp = byte & 0x0f;
    tmp = tmp < 10 ? 48 + tmp : 87 + tmp;
    writer[3] = tmp;
  }

  struct HexArrayString
  {
    char* data;
    u32 size;
  };

  HexArrayString generate_hex_array_string( u8 const* data, u32 size, char const* assetName )
  {
    s32 const entriesPerLine = 16;
    u32 hexArraySize = size * 5 + (size / entriesPerLine) + (size % entriesPerLine ? 1 : 0);
    char const preContent0[] = "char const ";
    char const preContent1[] = "[]={\"\n";
    char const postContent[] = "\"};";
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
      byte_to_hex_as_string( data[i], writer );
      writer += 4;
      *writer++ = ',';
      if ( !((i + 1) % entriesPerLine) )
      {
        *writer++ = '\n';
      }
    }
    bs::memory::copy( writer, postContent, postContentLength );
    writer += postContentLength;

    assert( u32( writer - hexArray ) == hexArraySize );
    return { hexArray, hexArraySize };
  }

  void generate_compiled_assets_file()
  {
    bs::file::LoadedFile ttf = load_into_memory( "w:/data/bs.ttf" );

    char const preContent[] = "#pragma warning(push)\n#pragma warning(disable:4309)\n#pragma warning(push)\n#pragma warning(disable:4838)\nnamespace compiledasset\n{\n";
    write_file( "w:/code/compiled_assets.cpp", preContent, bs::string_length( preContent ) );

    HexArrayString hexArray = generate_hex_array_string( (u8 const*) ttf.data, (u32) ttf.size, "DEFAULT_FONT" );
    append_file( "w:/code/compiled_assets.cpp", hexArray.data, hexArray.size );
    bs::memory::free( hexArray.data );
    bs::memory::free( ttf.data );


    char const postContent[] = "};\n#pragma warning(pop)\n#pragma warning(pop)\n";
    append_file( "w:/code/compiled_assets.cpp", postContent, bs::string_length( postContent ) );


    //filething( "w:/data/bs.ttf" );


  }
  // u32 write_file( char const* filepath, void const* data, u32 size )
  // {
  //   u32 result = 1;
  //   wchar_t wideChars[MAX_BS_PATH];
  //   utf8_to_wchar( filepath, wideChars, MAX_BS_PATH );

  //   HANDLE fileHandle = CreateFileW( wideChars,
  //                                    GENERIC_WRITE,
  //                                    0, 0,
  //                                    CREATE_ALWAYS,
  //                                    FILE_ATTRIBUTE_NORMAL,
  //                                    0 );

  //   if ( fileHandle != INVALID_HANDLE_VALUE )
  //   {
  //     DWORD bytesWritten;
  //     u8 const* reader = (u8 const*) data;
  //     u32 sizeLeft = size;
  //     u32 const MAX_WRITE = (u32) KiloBytes( 4 );

  //     while ( sizeLeft > 0 )
  //     {
  //       if ( WriteFile( fileHandle, reader, min( MAX_WRITE, sizeLeft ), &bytesWritten, 0 ) )
  //       {
  //         assert( bytesWritten <= sizeLeft );
  //         sizeLeft -= bytesWritten;
  //         reader += bytesWritten;
  //       }
  //       else
  //       {
  //         result = 0;
  //         log_error( "[WIN32_FILE]", "ERROR - couldn't write data to file: ", filepath );
  //         break;
  //       }
  //     }
  //   }
  //   else
  //   {
  //     result = 0;
  //   }

  //   CloseHandle( fileHandle );

  //   return result;
  // }



};