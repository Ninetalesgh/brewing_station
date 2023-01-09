
#include <platform/bs_platform.h>


#include <common/bsstring.h>
#include <module/bs_filesystem.h>

#include <memory>
namespace bs
{
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

    char* hexArray = (char*) bsp::platform->allocate( hexArraySize );
    char* writer = hexArray;

    memcpy( writer, preContent0, preContent0Length );
    writer += preContent0Length;
    memcpy( writer, assetName, nameLength );
    writer += nameLength;
    memcpy( writer, preContent1, preContent1Length );
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
    memcpy( writer, postContent, postContentLength );
    writer += postContentLength;

    assert( u32( writer - hexArray ) == hexArraySize );

    *out_size = hexArraySize;
    return hexArray;
  }

  struct AssetToCompile
  {
    void* data;
    u32 size;
  };

  char const* read_line( char* destination, s32 capacity, char const* string )
  {
    char const* reader = string;
    char* writer = destination;
    char* end = writer + capacity;
    while ( writer != end && *reader != '\0' && *reader != '\r' && *reader != '\n' )
    {
      *writer++ = *reader++;
    }

    if ( writer != end )
    {
      *writer = '\0';
    }
    else
    {
      BREAK;
    }

    while ( *reader == '\r' || *reader == '\n' )
    {
      reader++;
    }

    return reader;
  }

  struct Index
  {
    char const* name;
    void const* data;
  }assetIndex[] = { {"bs.ttf", nullptr} };

  s32 format_index_entry( char* destination, char const* path, char const* name, u32 size )
  {
    char* writer = destination;
    return bs::string_format( writer, 512, "\r\n{\"", path, "\", &", name, ", ", size, "}," ) - 1;
  }

  void cleanup_name( char* destination, s32 capacity, char const* name )
  {
    char const* reader = name;
    char* writer = destination;
    char* end = writer + capacity;
    while ( writer != end && *reader != '\0' )
    {
      if ( (*reader >= 'a' && *reader <= 'z') || (*reader >= 'A' && *reader <= 'Z') || (*reader >= '0' && *reader <= '9') )
      {
        *writer++ = *reader++;
      }
      else
      {
        *writer++ = '_';
        ++reader;
      }
    }
  }

  void app_on_load( bsp::AppData* appData )
  {
    bsm::FileSystem* fs = bsp::platform->defaultFileSystem;

    bsm::MountPathID precompiledAssetsMountPath = bsm::mount_path_to_filesystem( fs, "/../../code/precompiled_assets" );

    bsm::File* indexFile = bsm::load_file( fs, "precompiled_assets_list.txt" );
    if ( !indexFile )
    {
      log_error( "precompiled_assets_list.txt not found." );
      return;
    }

    char const* const compiledAssetsFile = "precompiled_assets.cpp";

    bsm::MountPathID existingPrecompiledAssetsMountPath;
    if ( bsm::find_file( fs, compiledAssetsFile, &existingPrecompiledAssetsMountPath ) )
    {
      precompiledAssetsMountPath = existingPrecompiledAssetsMountPath;
    }

    char const preContent[] = "#pragma warning(push)\r\n#pragma warning(disable:4309)\r\n#pragma warning(push)\r\n#pragma warning(disable:4838)\r\nnamespace compiledasset\r\n{";
    bsm::write_file( fs, compiledAssetsFile, preContent, bs::string_length( preContent ), precompiledAssetsMountPath );

    char const* reader = (char const*) indexFile->data;
    constexpr s32 INDEX_MAX_SIZE = 16000;
    char index[INDEX_MAX_SIZE] = {};
    char* indexWriter = index;

    char const preIndex[] = "\r\nstruct Index\r\n{\r\n  char const* name;\r\n  void const* data;\r\n  u64 size;\r\n} assetIndex[] = {";
    char const postIndex[] = "\r\n};";
    indexWriter += bs::string_format( index, INDEX_MAX_SIZE, preIndex ) - 1;

    for ( s32 i = 0; *reader != '\0'; ++i )
    {
      char filePath[128] = {};
      reader = read_line( filePath, 128, reader );
      bsm::File* fileToCompile = bsm::load_file( fs, filePath );

      char newAssetFileName[128] = {};
      bs::string_format( newAssetFileName, 128, filePath, "_compiled" );

      if ( fileToCompile || bsm::file_exists( fs, newAssetFileName, precompiledAssetsMountPath ) )
      {
        char newAssetFileIncludeLine[256] = {};
        bs::string_format( newAssetFileIncludeLine, 128, "\r\n#include \"internal/", newAssetFileName, "\"" );
        bsm::append_file( fs, compiledAssetsFile, newAssetFileIncludeLine, bs::string_length( newAssetFileIncludeLine ), precompiledAssetsMountPath );

        char placeholderName[64] = {};
        cleanup_name( placeholderName, 63, filePath );
        indexWriter += format_index_entry( indexWriter, filePath, placeholderName, (u32) fileToCompile->size );

        if ( fileToCompile )
        {

          u32 hexArraySize;
          char* hexArrayString = generate_hex_array_string( (u8 const*) fileToCompile->data, (u32) fileToCompile->size, placeholderName, &hexArraySize );

          char newAssetPath[256] = {};
          bs::string_format( newAssetPath, 256, "internal/", newAssetFileName );
          bsm::write_file( fs, newAssetPath, hexArrayString, hexArraySize, precompiledAssetsMountPath );

          bsp::platform->free( hexArrayString );
        }
      }
    }

    if ( *(indexWriter - 1) == ',' ) --indexWriter;

    indexWriter += bs::string_format( indexWriter, 24, postIndex );
    bsm::append_file( fs, compiledAssetsFile, index, bs::string_length( index ), precompiledAssetsMountPath );

    char const postContent[] = "\r\n};\r\n#pragma warning(pop)\r\n#pragma warning(pop)\r\n";
    bsm::append_file( fs, compiledAssetsFile, postContent, bs::string_length( postContent ), precompiledAssetsMountPath );
  }

  void app_tick( bsp::AppData* appData )
  {
    bsp::platform->shutdown();
  }

};
