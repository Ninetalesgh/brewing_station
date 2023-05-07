#pragma once

#include <core/bsdebuglog.h>
#include <common/bscolor.h>
#include <core/bsfont.h>
#include <platform/platform.h>

struct FirstAppData
{
  bs::graphics::Bitmap bmp;
  bs::font::GlyphTable* glyphTable;
  bs::font::GlyphSheet* glyphSheet;
};

FirstAppData* app;

void update();
void start();
void print_text( char const* text );



static bs::input::State* inputPtr;
const int windowWidth = DEFAULT_WINDOW_SIZE.x;
const int windowHeight = DEFAULT_WINDOW_SIZE.y - 40;



INLINE u32 is_key_held( u32 key )
{
  return bs::input::STATE_COUNT && inputPtr->held[key];
}

INLINE void clear( u32 clearColor )
{
  s32 end = app->bmp.width * app->bmp.height;
  for ( s32 i = 0; i < end; ++i )
  {
    app->bmp.pixel[i] = clearColor;
  }
}

INLINE void plot( int2 pos, u32 color )
{
  if ( pos.x >= 0.0f && pos.x < app->bmp.width &&
       pos.y >= 0.0f && pos.y < app->bmp.height )
  {
    app->bmp.pixel[pos.x + pos.y * app->bmp.width] = color;
  }
}

INLINE void draw_rect( int2 begin, int2 end, u32 color )
{
  begin = max( begin, int2 { 0, 0 } );
  end = min( end, DEFAULT_WINDOW_SIZE );

  for ( s32 y = begin.y; y < end.y; ++y )
    for ( s32 x = begin.x; x < end.x; ++x )
    {
      int2 pos = { x, y };
      plot( pos, color );
    }
}

INLINE void draw_circle( int2 pos, float radius, u32 color )
{
  s32 begin = -(s32) radius;
  s32 end = (s32) radius;

  for ( s32 y = begin; y < end; ++y )
    for ( s32 x = begin; x < end; ++x )
    {
      if ( x * x + y * y < radius * radius )
      {
        int2 tmpPos = pos + int2 { x, y };
        plot( tmpPos, color );
      }
    }
}

void draw_line( int2 begin, int2 end, u32 color )
{
  //TODO
}

namespace bs
{

  void app_sample_sound( PrmAppSampleSound prm )
  {
    BREAK;
  }

  void app_on_load( PrmAppOnLoad prm )
  {
    if ( prm.appData->userData == nullptr )
    {
      prm.appData->userData = memory::allocate_to_zero( sizeof( FirstAppData ) );
      app = (FirstAppData*) prm.appData->userData;
      app->bmp.pixel = (u32*) memory::allocate_to_zero( sizeof( u32 ) * windowWidth * windowHeight );
      app->bmp.height = windowHeight;
      app->bmp.width = windowWidth;

      bs::file::Data ttf;
      platform::load_file_into_memory( "w:/data/bs.ttf", &ttf );

      char const chars[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
      app->glyphTable = bs::font::create_glyph_table_from_ttf( ttf.data );
      bs::font::set_scale_for_glyph_creation( app->glyphTable, 64.0f );
      app->glyphSheet = bs::font::create_glyph_sheet( app->glyphTable, chars );
    }

    app = (FirstAppData*) prm.appData->userData;
    inputPtr = &prm.appData->input;

    start();
  }

  void app_tick( PrmAppTick prm )
  {
    update();
    graphics::RenderGroup rg = graphics::render_group_from_custom_bitmap( &app->bmp );
    platform::render( nullptr, &rg, nullptr );
    clear( color::BLACK );
  }


  void app_receive_udp_packet( PrmAppReceiveUDPPacket prm )
  {
    BREAK;
  }
};

DEPRECATED
#pragma once 

#ifndef BS_DEBUG

# define log_info_con_only( ... )
# define log_warning_con_only( ... )
# define log_error_con_only( ... )

# define log_info( ... )
# define log_warning( ... )
# define log_error( ... )

#else

#ifdef ERROR
#undef ERROR
#endif

# define log_info_con_only( ... )     _debug_log( bs::debug::DebugLogFlags::INFO, __VA_ARGS__ )
# define log_warning_con_only( ... )  _debug_log( bs::debug::DebugLogFlags::WARNING, __VA_ARGS__ )
# define log_error_con_only( ... )    _debug_log( bs::debug::DebugLogFlags::ERROR, __VA_ARGS__ )
# define log_info( ... )     _debug_log( bs::debug::DebugLogFlags::WRITE_TO_DEBUG_LOG_FILE | bs::debug::DebugLogFlags::INFO, __VA_ARGS__ )
# define log_warning( ... )  _debug_log( bs::debug::DebugLogFlags::WRITE_TO_DEBUG_LOG_FILE | bs::debug::DebugLogFlags::WARNING, __VA_ARGS__ )
# define log_error( ... )    _debug_log( bs::debug::DebugLogFlags::WRITE_TO_DEBUG_LOG_FILE | bs::debug::DebugLogFlags::ERROR, __VA_ARGS__ )

#endif

#ifdef _WIN32
#include <common/bs_common.h>
#include <intrin.h>
namespace bs
{
  namespace debug
  {
    class ProfilingObject
    {
    public:
      ProfilingObject( u64* output ): output( output ) { *output =  __rdtsc(); }
      ~ProfilingObject() { *output = __rdtsc() - *output; }
    private:
      u64* output;
    };
  };
};
# define PROFILE_SCOPE( outCycles_u64 ) bs::debug::ProfilingObject dbg_tmp_profiler_object##outCycles_u64 { &outCycles_u64 }
#else
# define PROFILE_SCOPE( outCycles_u64 )
#endif

namespace bs
{
  namespace debug
  {
    constexpr u32 MAX_DEBUG_MESSAGE_LENGTH = 512;

    enum DebugLogFlags: u32
    {
      NONE                    = 0x0,
      INFO                    = 0x1,
      WARNING                 = 0x2,
      ERROR                   = 0x4,
      WRITE_TO_DEBUG_LOG_FILE = 0x8,
      SEND_TO_SERVER          = 0x10,
    };
    DEFINE_ENUM_OPERATORS_U32( DebugLogFlags );

    template<typename... Args> void _debug_log( DebugLogFlags flags, Args... args );
  };
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////inl//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


#include <platform/platform_callbacks.h>
#include <common/bs_string.h>
namespace bs
{
  namespace debug
  {
    template<typename... Args> void _debug_log( DebugLogFlags flags, Args... args )
    {
      char debugBuffer[MAX_DEBUG_MESSAGE_LENGTH];
      s32 bytesToWrite = bs::string_format( debugBuffer, MAX_DEBUG_MESSAGE_LENGTH, args... ) - 1 /* ommit null */;
      if ( bytesToWrite > 0 )
      {
        if ( debugBuffer[bytesToWrite - 1] != '\n' )
        {
          debugBuffer[bytesToWrite++] = '\n';
          debugBuffer[bytesToWrite] = '\0';
        }

        platform::debug_log( flags, debugBuffer, bytesToWrite );
      }
    }
  };
};

DEPRECATED

#pragma once

#include <common/bs_common.h>

//todo
/*
textfields


*/


namespace bs
{
  namespace font
  {
    //GlyphTable holds reference to unrasterized font data
    struct GlyphTable;

    struct Glyph
    {
      s32 codepoint;
      int2 uvBegin;
      int2 uvSize;
      float advance;
      float lsb;
      s32 offsetX;
      s32 offsetY;
    };

    struct GlyphSheet
    {
      Glyph* glyphs;
      s32 glyphCount;
      graphics::TextureID textureID;
      s32 width;
      s32 height;
    };

    //returns nullptr on fail
    GlyphTable* create_glyph_table_from_ttf( void const* data );

    //set scale for create_raw_glyph_data() and create_glyph_sheet() calls
    void set_scale_for_glyph_creation( GlyphTable*, float scale );

    //returns a rasterized sheet with exactly the unicode characters supplied 
    //glyphs are of scale previously set with set_scale_for_glyph_creation()
    GlyphSheet* create_glyph_sheet( GlyphTable* glyphTable, char const* utf8String );

    Glyph* get_glyph_for_codepoint( GlyphSheet* glyphSheet, s32 codepoint );

    struct RawGlyphData
    {
      u8* data;
      s32 advance;
      s32 lsb;
      s32 width;
      s32 height;
      s32 offsetX;
      s32 offsetY;
    };
    //returns nullptr if the table doesn't have the glyph to the requested codepoint
    //returns a rasterized glyph at the scale previously set with set_scale_for_glyph_creation()
    RawGlyphData* create_raw_glyph_data( GlyphTable*, s32 unicodeCodepoint );
  };
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////cpp//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include <common/bscolor.h>
#include <common/bs_string.h>

//#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "internal/stb_truetype.h"

namespace bs
{
  namespace font
  {
    struct GlyphTable
    {
      graphics::ShaderProgram shaderProgram;
      float scale;
      float scaleForPixelHeight;
      stbtt_fontinfo* fontInfo;
    };

    font::GlyphTable* create_glyph_table_from_ttf( void const* data )
    {
      font::GlyphTable* glyphTable = nullptr;
      if ( stbtt_GetFontOffsetForIndex( (u8 const*) data, 0 ) == 0 )
      {
        u32 allocSize = sizeof( GlyphTable ) + sizeof( stbtt_fontinfo );
        u8* allocation = (u8*) bsp::pla tform->allocate( allocSize );
        memset( allocation, 0, allocSize );

        glyphTable = (GlyphTable*) allocation;
        stbtt_fontinfo* stbFontInfo = (stbtt_fontinfo*) (allocation + sizeof( GlyphTable ));

        if ( stbtt_InitFont( stbFontInfo, (u8 const*) data, 0 ) )
        {
          glyphTable->fontInfo = stbFontInfo;
          set_scale_for_glyph_creation( glyphTable, 64.0f );
        }
        else
        {
          bsp::platform->free( allocation );
          glyphTable = nullptr;
          BREAK;
        }
      }
      else
      {
        BREAK;
      }

      return glyphTable;
    }

    void set_scale_for_glyph_creation( GlyphTable* glyphTable, float scale )
    {
      if ( glyphTable && glyphTable->fontInfo )
      {
        glyphTable->scale = scale;
        glyphTable->scaleForPixelHeight = stbtt_ScaleForPixelHeight( (stbtt_fontinfo*) glyphTable->fontInfo, scale );
      }
      else
      {
        BREAK;
      }
    }

    Glyph get_glyph_dimensions( GlyphTable* glyphTable, s32 unicodeCodepoint )
    {
      stbtt_fontinfo* fontInfo = glyphTable->fontInfo;
      float scale = glyphTable->scaleForPixelHeight;

      Glyph result = {};

      if ( scale > 0.0f )
      {
        s32 glyphIndex = stbtt_FindGlyphIndex( fontInfo, unicodeCodepoint );

        if ( glyphIndex > 0 )
        {
          s32 advance;
          s32 lsb;
          stbtt_GetGlyphHMetrics( fontInfo, glyphIndex, &advance, &lsb );

          s32 ix0, ix1, iy0, iy1;
          stbtt_GetGlyphBitmapBoxSubpixel( fontInfo, glyphIndex, scale, scale, 0, 0, &ix0, &iy0, &ix1, &iy1 );
          result.uvSize.x = (ix1 - ix0);
          result.uvSize.y = (iy1 - iy0);
          result.offsetX = ix0;
          result.offsetY = iy0;

          result.advance = float( advance ) * scale;
          result.lsb = float( lsb );
        }
        else // !( glyphIndex > 0 )
        {
          BREAK;
        }
      }
      else // !( scale > 0.0f )
      {
        BREAK;
      }

      return result;
    }

    RawGlyphData* create_raw_glyph_data( GlyphTable* glyphTable, s32 unicodeCodepoint )
    {
      stbtt_fontinfo* fontInfo = glyphTable->fontInfo;
      float scale = glyphTable->scaleForPixelHeight;

      RawGlyphData* glyph = nullptr;
      s32 advance;
      s32 lsb;

      if ( scale > 0.0f )
      {
        s32 glyphIndex = stbtt_FindGlyphIndex( fontInfo, unicodeCodepoint );

        if ( glyphIndex > 0 )
        {
          stbtt_GetGlyphHMetrics( fontInfo, glyphIndex, &advance, &lsb );

          s32 ix0, ix1, iy0, iy1;
          float scale_x = scale;
          float scale_y = scale;
          float shift_x = 0;
          float shift_y = 0;
          float flatnessInPixels = 0.35f;
          stbtt__bitmap gbm;

          stbtt_GetGlyphBitmapBoxSubpixel( fontInfo, glyphIndex, scale_x, scale_y, shift_x, shift_y, &ix0, &iy0, &ix1, &iy1 );

          gbm.w = (ix1 - ix0);
          gbm.h = (iy1 - iy0);
          gbm.pixels = nullptr;

          if ( gbm.w && gbm.h )
          {
            u8* allocation = (u8*) bsp::plat form->allocate( sizeof( RawGlyphData ) + gbm.w * gbm.h );
            glyph = (RawGlyphData*) allocation;
            gbm.pixels = allocation + sizeof( RawGlyphData );
            glyph->data = gbm.pixels;
            glyph->advance = advance;
            glyph->lsb = lsb;
            glyph->width = gbm.w;
            glyph->height = gbm.h;
            glyph->offsetX = ix0;
            glyph->offsetY = iy0;
            if ( gbm.pixels )
            {
              gbm.stride = gbm.w;
              stbtt_vertex* vertices = nullptr;
              s32 num_verts = stbtt_GetGlyphShape( fontInfo, glyphIndex, &vertices );
              stbtt_Rasterize( &gbm, flatnessInPixels, vertices, num_verts, scale_x, scale_y, shift_x, shift_y, ix0, iy0, 1, fontInfo->userdata );
              bsp::platform->free( vertices );
            }
            else
            {
              BREAK;
            }
          }
          else if ( advance )
          {
            //it's a space
          }
          else
          {
            BREAK;
          }
        }
        else // !( glyphIndex > 0 )
        {
          BREAK;
        }
      }
      else // !( scale > 0.0f )
      {
        BREAK;
      }

      return glyph;
    }

    GlyphSheet* create_glyph_sheet( GlyphTable* glyphTable, char const* utf8String )
    {
      char const* reader = utf8String;
      s32 glyphCount = bs::string_length_utf8( utf8String );

      Glyph* rects = nullptr;
      GlyphSheet* resultSheet = nullptr;
      {
        s32 allocSize = sizeof( GlyphSheet ) + sizeof( Glyph ) * glyphCount;
        u8* allocation = (u8*) bsp::pla form->allocate( allocSize );
        memset( allocation, 0, allocSize );
        resultSheet = (GlyphSheet*) allocation;
        rects = (Glyph*) (allocation + sizeof( GlyphSheet ));
      }

      int2 sheetDims = 0;
      int2 currentRow = 0;
      constexpr s32 MAX_WIDTH = 512;

      s32 rectsIndex = 0;
      while ( *reader )
      {
        s32 codepoint;
        reader = bs::string_parse_utf8( reader, &codepoint );

        Glyph newGlyph = get_glyph_dimensions( glyphTable, codepoint );
        newGlyph.codepoint = codepoint;

        if ( currentRow.x + (s32) newGlyph.uvSize.x > MAX_WIDTH )
        {
          sheetDims.y += currentRow.y + 1;
          sheetDims.x = max( sheetDims.x, currentRow.x );
          currentRow = { 0, 0 };
        }

        newGlyph.uvBegin = { currentRow.x, sheetDims.y };
        rects[rectsIndex++] = newGlyph;

        currentRow.x += (s32) newGlyph.uvSize.x + 1;
        currentRow.y = max( currentRow.y, (s32) newGlyph.uvSize.y );
      }

      sheetDims.x = max( sheetDims.x, currentRow.x );
      sheetDims.y = sheetDims.y + currentRow.y;
      #define LUL
      graphics::Bitmap* sheetBMP = nullptr;
      {
        u8* allocation = (u8*) bsp::platf orm->allocate( sizeof( graphics::Bitmap ) + sizeof( u32 ) * sheetDims.x * sheetDims.y );
        sheetBMP = (graphics::Bitmap*) allocation;
        sheetBMP->width = sheetDims.x;
        sheetBMP->height = sheetDims.y;
        sheetBMP->pixel = (u32*) (allocation + sizeof( graphics::Bitmap ));
      }

      //rasterize glyphs
      for ( s32 i = 0; i < glyphCount; ++i )
      {
        RawGlyphData* rawGlyphData = create_raw_glyph_data( glyphTable, rects[i].codepoint );
        if ( rawGlyphData )
        {
          assert( rawGlyphData->width == (s32) rects[i].uvSize.x );
          assert( rawGlyphData->height == (s32) rects[i].uvSize.y );

          for ( s32 y = 0; y < (s32) rects[i].uvSize.y; ++y )
          {
            u32* writer = sheetBMP->pixel + (s32) rects[i].uvBegin.x + (((s32) rects[i].uvBegin.y + y) * sheetBMP->width);
            for ( s32 x = 0; x < (s32) rects[i].uvSize.x; ++x )
            {
              s32 index = x + y * rawGlyphData->width;
              //*writer++ = color::rgba( 0xff, 0xff, 0xff, rawGlyphData->data[index] );
              *writer++ = color::rgba( rawGlyphData->data[index], rawGlyphData->data[index], rawGlyphData->data[index], rawGlyphData->data[index] );
            }
          }

          bsp::platform->free( rawGlyphData );
        }
      }

      graphics::TextureData texData {};
      texData.pixel = sheetBMP->pixel;
      texData.width = sheetBMP->width;
      texData.height = sheetBMP->height;
      texData.format = graphics::TextureFormat::RGBA8;

      resultSheet->glyphs =     rects;
      resultSheet->glyphCount = glyphCount;
      resultSheet->textureID =  bsp::pla tform->allocate_texture( &texData );
      resultSheet->width =      sheetBMP->width;
      resultSheet->height =     sheetBMP->height;

      bsp::platform->free( sheetBMP );

      return resultSheet;
    }

    Glyph* get_glyph_for_codepoint( GlyphSheet* glyphSheet, s32 codepoint )
    {
      //TODO filter and sort glypharray and binary search

      Glyph* glyph = glyphSheet->glyphs;
      Glyph* end = glyph + glyphSheet->glyphCount;
      while ( glyph->codepoint != codepoint && glyph != end ) { ++glyph; }

      return glyph == end ? nullptr : glyph;
    }
  };
};

DEPRECATED

#pragma once

#include <core/bsthread.h>
#include <common/bs_common.h>


//TODO arena code
namespace bs
{
  namespace memory
  {
    struct Arena;
    struct ArenaEntry;

    namespace debug
    {
      struct ArenaDebugData
      {
        s64 size;
        s64 capacity;
        s32 totalEntries;
        s32 inactiveEntries;
      };

      struct ArenaObserver
      {
        void fetch_debug_data( ArenaDebugData* out_DebugData );
        memory::Arena const* arena;
      };
    };

    // best used LIFO, some unordered deallocation OK, will result in fragmentation though.
    struct Arena
    {
      void* alloc( s64 size, u32 alignment = 4 );
      void* alloc_set_zero( s64 size, u32 alignment = 4 );
      s64 get_entry_size( char* ptr );
      void free( void* ptr );
      void clear();

      char* bufferBegin;
      char* bufferEnd;
      char* current;
      ArenaEntry* lastEntry;
      s32 entryCount;
      atomic32 guard;

    public:
      Arena();
    private:
      Arena( Arena const& ) {}
    };

    INLINE void copy( char* destination, char* source, s64 size );
    INLINE void set_zero( char* target, s64 size );

    void init_arena( char* memory, s64 capacity, Arena* out_Arena );
    Arena* init_arena_in_place( char* memory, s64 capacity );

  };

};



////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////cpp/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////




namespace bs
{
  namespace memory
  {
    enum EntryFlags: u64
    {
      ALIVE = 0x1,

    };
    DEFINE_ENUM_OPERATORS_U64( EntryFlags );

    struct ArenaEntry
    {
      char* begin;
      s64   size;
      EntryFlags flags;
    };

    namespace debug
    {
      void ArenaObserver::fetch_debug_data( ArenaDebugData* out_DebugData )
      {
        if ( out_DebugData )
        {
          s32 inactiveEntries = 0;
          memory::ArenaEntry const* entry = arena->lastEntry;

          while ( entry != (memory::ArenaEntry*) arena->bufferEnd )
          {
            if ( !(entry->flags & ALIVE) )
            {
              ++inactiveEntries;
            }
            ++entry;
          }

          out_DebugData->size = arena->current - arena->bufferBegin;
          out_DebugData->capacity = arena->bufferEnd - arena->bufferBegin;
          out_DebugData->totalEntries = arena->entryCount;
          out_DebugData->inactiveEntries = inactiveEntries;
        }
        else
        {
          BREAK;
        }
      }
    };

    Arena::Arena()
      : bufferBegin( nullptr )
      , bufferEnd( nullptr )
      , current( nullptr )
      , lastEntry( nullptr )
      , entryCount( 0 )
    {}

    void* Arena::alloc( s64 size, u32 alignment /*= 4*/ )
    {
      LOCK_SCOPE( guard );

      //TODO compensate for fragmentation a bit?
      char* result = nullptr;

      current += ((u64) current) & (alignment - 1); //TODO this is wrong, right? it should be ^?

      if ( current + size < (char*) (lastEntry - 1) )
      {
        result = current;
        current += size;
        ++entryCount;
        lastEntry--;
        lastEntry->begin = result;
        lastEntry->size = size;
        lastEntry->flags = ALIVE;
      }
      else
      {
        BREAK;
      }

      return result;
    }

    void* Arena::alloc_set_zero( s64 size, u32 alignment /*= 4*/ )
    {
      char* result = (char*) alloc( size, alignment );
      set_zero( result, size );
      return result;
    }

    s64 Arena::get_entry_size( char* ptr )
    {
      ArenaEntry* entry = lastEntry;

      while ( entry != (ArenaEntry*) bufferEnd )
      {
        if ( entry->begin == ptr )
        {
          return entry->size;
        }
        ++entry;
      }

      //TODO: ptr is not in arena
      assert( 0 );
      return 0;
    }

    void Arena::free( void* ptr )
    {
      LOCK_SCOPE( guard ); //TODO this is a bit more overhead than necessary

      if ( ptr != nullptr && ptr < bufferEnd && ptr >= bufferBegin )
      {
        ArenaEntry* entry = lastEntry;

        //tag entry
        u32 entryFound = 0;
        while ( entry != (ArenaEntry*) bufferEnd )
        {
          if ( entry->begin == (char*) ptr )
          {
            entry->flags &= ~ALIVE;
            entryFound = 1;
            break;
          }
          entry++;
        }

        assert( entryFound );

        //remove dead entries
        while ( lastEntry != (ArenaEntry*) bufferEnd )
        {
          if ( lastEntry->flags & ALIVE )
          {
            break;
          }

          current = lastEntry->begin;
          ++lastEntry;
          --entryCount;
        }
      }
      else
      {
        BREAK;
      }
    }

    void Arena::clear()
    {
      current = bufferBegin;
      lastEntry = (ArenaEntry*) bufferEnd;
    }

    void init_arena( char* memory, s64 capacity, Arena* out_Arena )
    {
      //out of bounds check:
      #if !BS_BUILD_RELEASE
      * (memory + capacity - 1) = 0;
      #endif
      if ( out_Arena )
      {
        out_Arena->bufferBegin = memory;
        out_Arena->bufferEnd = memory + capacity;
        out_Arena->current = out_Arena->bufferBegin;
        out_Arena->lastEntry = (ArenaEntry*) out_Arena->bufferEnd;
        out_Arena->entryCount = 0;
      }
      else
      {
        BREAK;
      }
    }

    Arena* init_arena_in_place( char* memory, s64 capacity )
    {
      char* writer = memory;
      Arena* result = (Arena*) writer;
      writer += sizeof( Arena );
      init_arena( writer, capacity - sizeof( Arena ), result );

      return result;
    }

    INLINE void copy( char* destination, char* source, s64 size )
    {
      while ( size-- > 0 )
      {
        *destination++ = *source++;
      }
    }

    INLINE void set_zero( char* target, s64 size )
    {
      while ( size-- > 0 )
      {
        *target++ = 0;
      }
    }

  };

};


#pragma once

DEPRECATED

#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"opengl32.lib")

#include <ui/bstextarea.h>
#include <scene/bssceneobject.h>

#include <core/bsgraphics.h>
#include <core/bsfile.h>
#include <core/bsfont.h>
#include <common/bscolor.h>
#include <common/bs_common.h>

#include <opengl_ext.h>
#include <windows.h>
#ifdef min
# undef min
#endif
#ifdef max
# undef max
#endif


namespace opengl
{
  struct OpenGlInfo
  {
    u32 extEnabled;
    char const* vendor;
    char const* renderer;
    char const* version;
    char const* shadingLanguageVersion;
    char const* extensions;
    u32 GL_EXT_texture_sRGB;
    u32 GL_EXT_framebuffer_sRGB;
  };

  u32 init( HDC deviceContext );
  void resize_viewport();
  void tick();

  OpenGlInfo const* get_info();
  //void create_opengl_context_for_worker_thread();

  bs::graphics::TextureID allocate_texture( bs::graphics::TextureData const* );
  void free_texture( bs::graphics::TextureID texture );

  bs::graphics::Mesh allocate_mesh( bs::graphics::MeshData const* );
  void free_mesh( bs::graphics::Mesh );

  using VertexArrayObjectID = u32;
  using ProgramID = u32;
  using ShaderID = u32;
  using uniformID = u32;

  ProgramID create_shader_program( bs::file::Data headerFileData, bs::file::Data vsFileData, bs::file::Data fsFileData );

  //all three files in one, mark sections with #h, #vs and #fs.
  ProgramID create_shader_program( bs::file::Data combinedglsl );

  void render( bs::graphics::RenderTarget*, bs::graphics::RenderGroup*, bs::graphics::Camera* );
};


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////cpp/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


namespace opengl
{
  namespace global
  {
    static OpenGlInfo info;
    static u32 defaultTextureFormat;
    static s32 rcAttributes[] =
    {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
      WGL_CONTEXT_MINOR_VERSION_ARB, 0,
      WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB /*| WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB*/,
      WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
      0
    };
    static HGLRC renderContext;
    static HDC   deviceContext;
  }

  OpenGlInfo const* get_info() { return &global::info; }
  void check_gl_error();

  void init_opengl_info()
  {
    global::info.vendor = (char const*) glGetString( GL_VENDOR );
    global::info.renderer = (char const*) glGetString( GL_RENDERER );
    global::info.version = (char const*) glGetString( GL_VERSION );
    global::info.shadingLanguageVersion = (char const*) glGetString( GL_SHADING_LANGUAGE_VERSION );
    global::info.extensions = (char const*) glGetString( GL_EXTENSIONS );
    global::info.GL_EXT_texture_sRGB = bs::string_contains( global::info.extensions, "GL_EXT_texture_sRGB" ) != nullptr;
    global::info.GL_EXT_framebuffer_sRGB = bs::string_contains( global::info.extensions, "GL_EXT_framebuffer_sRGB" ) != nullptr;
  };

  u32 set_pixel_format_for_dc( HDC deviceContext )
  {
    s32 suggestedPixelFormatIndex = 0;

    if ( wglChoosePixelFormatARB )
    {
      s32 intAttribList[] =
      {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 24,
        WGL_RED_BITS_ARB, 8,
        WGL_RED_SHIFT_ARB, 16,
        WGL_GREEN_BITS_ARB, 8,
        WGL_GREEN_SHIFT_ARB, 8,
        WGL_BLUE_BITS_ARB, 8,
        WGL_BLUE_SHIFT_ARB, 0,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_ALPHA_SHIFT_ARB, 24,
      0 };
      //TODO this doesn't give me any alpha bits and alpha shift, whereas the non wgl call does?
      float floatAttribList[] = { 0 };
      u32 extendedPick = 0;
      wglChoosePixelFormatARB( deviceContext, intAttribList, floatAttribList, 1, &suggestedPixelFormatIndex, &extendedPick );
    }
    else
    {
      PIXELFORMATDESCRIPTOR desiredPixelFormat {};
      desiredPixelFormat.nSize = sizeof( desiredPixelFormat );
      desiredPixelFormat.nVersion = 1;
      desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
      desiredPixelFormat.cColorBits = 32;
      desiredPixelFormat.cAlphaBits = 8;
      desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
      suggestedPixelFormatIndex = ChoosePixelFormat( deviceContext, &desiredPixelFormat );
    }

    PIXELFORMATDESCRIPTOR suggestedPixelFormat {};
    DescribePixelFormat( deviceContext, suggestedPixelFormatIndex, sizeof( suggestedPixelFormat ), &suggestedPixelFormat );

    u32 result = SetPixelFormat( deviceContext, suggestedPixelFormatIndex, &suggestedPixelFormat );
    return result;
  }

  u32 load_extensions()
  {
    u32 result = 0;
    HWND dummyWindow = 0;
    WNDCLASSW wndClass {};
    wndClass.lpfnWndProc   = DefWindowProcW;
    wndClass.hInstance     = GetModuleHandle( NULL );
    wndClass.lpszClassName = L"OpenGlExtensionLoaderDummy";

    if ( RegisterClassW( &wndClass ) )
    {
      dummyWindow = CreateWindowExW( 0,
                                     wndClass.lpszClassName, L"dummy",
                                     0,
                                     CW_USEDEFAULT, CW_USEDEFAULT,
                                     CW_USEDEFAULT, CW_USEDEFAULT,
                                     0,
                                     0,
                                     wndClass.hInstance,
                                     0 );

      HDC dummyDC = GetDC( dummyWindow );
      set_pixel_format_for_dc( dummyDC );

      HGLRC dummyRC = wglCreateContext( dummyDC );
      if ( !wglMakeCurrent( dummyDC, dummyRC ) )
      {
        BREAK;
      }
      opengl_ext::init();

      wglMakeCurrent( 0, 0 );
      wglDeleteContext( dummyRC );
      ReleaseDC( dummyWindow, dummyDC );
      DestroyWindow( dummyWindow );

      result = 1;
    }

    return result;
  }

  u32 init( HDC deviceContext )
  {
    if ( !load_extensions() )
    {
      BREAK;
      return 0;
    }

    if ( !set_pixel_format_for_dc( deviceContext ) )
    {
      BREAK;
      return 0;
    }

    global::deviceContext = deviceContext;
    if ( wglCreateContextAttribsARB )
    {
      global::renderContext = wglCreateContextAttribsARB( global::deviceContext, 0, global::rcAttributes );
      global::info.extEnabled = 1;
    }

    if ( !global::renderContext )
    {
      global::renderContext = wglCreateContext( global::deviceContext );
      global::info.extEnabled = 0;
    }

    if ( !wglMakeCurrent( global::deviceContext, global::renderContext ) )
    {
      BREAK;
      return 0;
    }

    init_opengl_info();

    global::defaultTextureFormat = global::info.GL_EXT_texture_sRGB ? GL_RGBA8 : GL_SRGB8_ALPHA8;

    if ( global::info.GL_EXT_framebuffer_sRGB )
    {
      glEnable( GL_FRAMEBUFFER_SRGB );
    }

    if ( wglSwapIntervalEXT ) //vsync
    {
      wglSwapIntervalEXT( 1 );
    }

    resize_viewport();

    // glEnable( GL_CULL_FACE );
    // glCullFace( GL_BACK );

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

    float4 bg = color::float4_from_rgba( color::rgba( 30, 30, 30, 255 ) );
    glClearColor( bg.x * bg.x, bg.y * bg.y, bg.z * bg.z, bg.w );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    check_gl_error();

    return 1;
  }

  HGLRC create_render_context_for_worker_thread()
  {
    check_gl_error();
    HGLRC rc = wglCreateContextAttribsARB( global::deviceContext, global::renderContext, global::rcAttributes );
    check_gl_error();
    return rc;
  }

  void set_worker_thread_render_context( HGLRC renderContext )
  {
    if ( !wglMakeCurrent( global::deviceContext, renderContext ) )
    {
      BREAK;
    }
  }

  void resize_viewport()
  {
    int2 size = ::global::mainWindow.size;
    glViewport( 0, 0, size.x, size.y );
  }

  void tick()
  {
    SwapBuffers( global::deviceContext );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  }

  INLINE u32 get_size_for_index_format( bs::graphics::IndexFormat indexFormat ) { return indexFormat == bs::graphics::IndexFormat::U16 ? sizeof( u16 ) : sizeof( u32 ); }

  INLINE GLenum get_gl_index_format( bs::graphics::IndexFormat indexFormat ) { return indexFormat == bs::graphics::IndexFormat::U16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT; }

  VertexArrayObjectID init_vao_for_mesh( bs::graphics::Mesh const* mesh )
  {
    //not working yet
    VertexArrayObjectID vao;
    BREAK;
    check_gl_error();
    glCreateVertexArrays( 1, &vao );

    glVertexArrayVertexBuffer( vao, 0, mesh->vb, 0, 0 );
    glVertexArrayVertexBuffer( vao, 1, mesh->uvb, 0, 0 );

    glEnableVertexArrayAttrib( vao, 0 );
    glVertexArrayAttribBinding( vao, 0, 0 );
    glVertexArrayAttribFormat( vao, 0, 3, GL_FLOAT, GL_FALSE, 0 );

    glEnableVertexArrayAttrib( vao, 1 );
    glVertexArrayAttribBinding( vao, 1, 1 );
    glVertexArrayAttribFormat( vao, 1, 2, GL_FLOAT, GL_FALSE, 0 );

    glVertexArrayElementBuffer( vao, mesh->ib );

    check_gl_error();
    return vao;
  }

  void init_vao( bs::graphics::RenderObject* renderObject )
  {
    check_gl_error();

    glGenVertexArrays( 1, &renderObject->id );
    glBindVertexArray( renderObject->id );

    glBindBuffer( GL_ARRAY_BUFFER, renderObject->mesh.vb );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0 );

    glBindBuffer( GL_ARRAY_BUFFER, renderObject->mesh.uvb );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, renderObject->mesh.ib );

    check_gl_error();

    glBindVertexArray( 0 );
    glDisableVertexAttribArray( 0 );
    glDisableVertexAttribArray( 1 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    check_gl_error();
  }

  void render_object( bs::graphics::RenderObject* renderObject, bs::graphics::ShaderProgram shaderProgram )
  {
    if ( !renderObject->id )
    {
      init_vao( renderObject );
    }

    glBindVertexArray( renderObject->id );

    glUseProgram( shaderProgram );

    u32 indexSize = get_size_for_index_format( renderObject->mesh.indexFormat );
    GLenum glIndexFormat = get_gl_index_format( renderObject->mesh.indexFormat );
    glDrawElements( GL_TRIANGLES, renderObject->mesh.indexCount * indexSize, glIndexFormat, (void*) 0 );

    glUseProgram( 0 );
    glBindVertexArray( 0 );
  }

  bs::graphics::Mesh allocate_mesh( bs::graphics::MeshData const* raw )
  {
    bs::graphics::Mesh resultMesh {};

    u32 const vertexCount = raw->vertexCount;
    u32 const indexCount = raw->indexCount;
    bs::graphics::IndexFormat const indexFormat = raw->indexFormat;

    if ( indexFormat == bs::graphics::IndexFormat::INVALID ) BREAK;

    check_gl_error();

    glCreateBuffers( 1, &resultMesh.vb );
    glNamedBufferData( resultMesh.vb, vertexCount * sizeof( float3 ), raw->vertices, GL_STATIC_DRAW );

    glCreateBuffers( 1, &resultMesh.uvb );
    glNamedBufferData( resultMesh.uvb, vertexCount * sizeof( float2 ), raw->uvs, GL_STATIC_DRAW );

    glCreateBuffers( 1, &resultMesh.ib );
    glNamedBufferData( resultMesh.ib, indexCount * get_size_for_index_format( indexFormat ), raw->indices, GL_STATIC_DRAW );

    check_gl_error();

    resultMesh.indexCount = indexCount;
    resultMesh.indexFormat = indexFormat;
    return resultMesh;
  }

  void free_mesh( bs::graphics::Mesh mesh )
  {
    u32 buffers[] = { mesh.vb, mesh.uvb, mesh.ib };
    glDeleteBuffers( 3, buffers );
  }

  bs::graphics::TextureID allocate_texture( bs::graphics::TextureData const* textureData )
  {
    u32 const* pixel = (u32 const*) textureData->pixel;
    //TODO use PBO to skip one copy step ?
    GLuint textureHandle = 0;
    glGenTextures( 1, &textureHandle );
    glBindTexture( GL_TEXTURE_2D, textureHandle );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, textureData->width, textureData->height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixel );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    //glGenerateMipmap(GL_TEXTURE_2D); set filter to mipmap if want
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glBindTexture( GL_TEXTURE_2D, 0 );

    return (bs::graphics::TextureID) textureHandle;
  }

  void free_texture( bs::graphics::TextureID texture )
  {
    GLuint handle = (GLuint) texture;
    glDeleteTextures( 1, &handle );
  }

  void allocate_render_object()
  {

    //diffuse texture
    //normal texture

    //mesh

    //animation data?
  }

  void free_render_object()
  {

    //free_texture();
  }



  void render_text_area_fixed_pipeline( bs::graphics::RenderTarget* target, bs::ui::TextArea* textArea )
  {
    bs::font::GlyphSheet* glyphSheet = ::global::defaultGlyphSheet;
    bs::font::GlyphTable* glyphTable = ::global::defaultGlyphTable;
    int2 screenSize = ::global::mainWindow.size;
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, glyphSheet->textureID );

    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glScalef( 1.0f / (float) glyphSheet->width, 1.0f / (float) glyphSheet->height, 1.0f );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    float mvMat[] = {
     1.0f, 0.0f, 0.0f, 0.0f,
     0.0f, 1.0f, 0.0f, 0.0f,
     0.0f, 0.0f, 1.0f, 0.0f,
     0.0f, 0.0f, 0.0f, 1.0f,
    };
    glLoadMatrixf( mvMat );
    glScalef( DEBUG::scale, DEBUG::scale, 1.0f );


    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.f, screenSize.x, screenSize.y, 0.f, 0.f, 1.f );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glBegin( GL_TRIANGLES );
    //render here


    float2 invScreenSize = ::global::mainWindow.size;
    invScreenSize = { 1.0f / invScreenSize.x, 1.0f / invScreenSize.y };

    float2 textAreaOrigin = { 0.0f, 0.0f };
    float2 lineOrigin = textAreaOrigin;

    for ( s32 lineIndex = 0; lineIndex < textArea->lineCount; ++lineIndex )
    {
      lineOrigin.y += glyphTable->scale;
      float2 currentPos = lineOrigin;

      bs::ui::TextLine& textLine = textArea->lines[lineIndex];

      s32 const end = lineIndex == textArea->lineCount - 1 ? (s32) '\0' : (s32) '\n';
      s32 const* reader = textLine.codepoints;
      while ( *reader != end )
      {
        bs::font::Glyph* glyph = get_glyph_for_codepoint( glyphSheet, *reader );
        ++reader;

        {
          float2 min = textAreaOrigin + float2 { currentPos.x + glyph->offsetX, currentPos.y + glyph->offsetY };
          float2 max = { min.x + (float) glyph->uvSize.x,  min.y + (float) glyph->uvSize.y };

          currentPos.x += glyph->advance;
          float2 minuv = glyph->uvBegin;
          float2 maxuv = glyph->uvBegin + glyph->uvSize;

          // minuv = { 0, 0 };
          // maxuv = { 1,1 };
          // min = { 0,0 };
          // max = { 1, 1 };

          glTexCoord2f( minuv.x, maxuv.y );
          glVertex2f( min.x, max.y );

          glTexCoord2f( maxuv.x, maxuv.y );
          glVertex2f( max.x, max.y );

          glTexCoord2f( maxuv.x, minuv.y );
          glVertex2f( max.x, min.y );

          glTexCoord2f( minuv.x, maxuv.y );
          glVertex2f( min.x, max.y );

          glTexCoord2f( maxuv.x, minuv.y );
          glVertex2f( max.x, min.y );

          glTexCoord2f( minuv.x, minuv.y );
          glVertex2f( min.x, min.y );
        }
      }

    }

    glEnd();
  }


  void render_testDEBUG( bs::graphics::RenderTarget* target, bs::graphics::RenderGroup* group, bs::graphics::Camera* camera )
  {
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end

        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end

        2.0f,-1.0f, 2.0f,
        -1.0f,-1.0f,-1.0f,
        2.0f,-1.0f,-1.0f,

        2.0f, 1.0f,-1.0f,
        2.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,

        -1.0f,-1.0f,-1.0f,
        -1.0f, 2.0f, 1.0f,
        -1.0f, 2.0f,-1.0f,

        2.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,

        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 2.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 2.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 2.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };
    static const GLfloat g_color_buffer_data[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };
    static GLuint vertexbuffer;
    static GLuint colorbuffer;

    static int firsttime = 1;
    if ( firsttime-- )
    {
      GLuint VertexArrayID;
      glGenVertexArrays( 1, &VertexArrayID );
      glBindVertexArray( VertexArrayID );

      glGenBuffers( 1, &vertexbuffer );
      glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
      glBufferData( GL_ARRAY_BUFFER, sizeof( g_vertex_buffer_data ), g_vertex_buffer_data, GL_STATIC_DRAW );

      GLuint colorArrayID;
      glBindVertexArray( 0 );
      glGenVertexArrays( 1, &colorArrayID );
      glBindVertexArray( colorArrayID );

      glGenBuffers( 1, &colorbuffer );
      glBindBuffer( GL_ARRAY_BUFFER, colorbuffer );
      glBufferData( GL_ARRAY_BUFFER, sizeof( g_color_buffer_data ), g_color_buffer_data, GL_STATIC_DRAW );
      glBindVertexArray( 0 );
    }

    static float modelposy = 0.0f;
    modelposy += 0.02f;
    float4x4 model = float4x4::identity();
    model.pos = float4 { 0,2.0f * sinf( modelposy ), 0, 1 };

    float4x4 vp = bs::graphics::get_camera_view_projection_matrix( camera, (float)::global::mainWindow.size.x, (float)::global::mainWindow.size.y, 45.0f, 0.1f, 100.0f );
    float4x4 mvp = model * vp;
    uniformID mvpID = glGetUniformLocation( ::global::defaultGlyphTable->shaderProgram, "MVP" );
    glUniformMatrix4fv( mvpID, 1, GL_FALSE, &mvp.m00 );

    glUseProgram( ::global::defaultGlyphTable->shaderProgram );
    glEnableVertexAttribArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
    glVertexAttribPointer(
       0,                  // attribute 0. match shader
       3,                  // size
       GL_FLOAT,           // type
       GL_FALSE,           // normalized?
       0,                  // stride
       (void*) 0           // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays( GL_TRIANGLES, 0, 3 * 12 ); // Starting from vertex 0; 3 vertices total -> 1 triangle

    glEnableVertexAttribArray( 1 );
    glBindBuffer( GL_ARRAY_BUFFER, colorbuffer );
    glVertexAttribPointer(
        1,                                // attribute. match shader.
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*) 0                         // array buffer offset
    );

    // glDisableVertexAttribArray( 0 );
    // glDisableVertexAttribArray( 1 );
  }

  void render_custom_bitmap( bs::graphics::RenderTarget* target, bs::graphics::Bitmap* bmp )
  {
    int2 screenSize = ::global::mainWindow.size;

    bs::graphics::TextureData texData {};
    texData.pixel = bmp->pixel;
    texData.width = bmp->width;
    texData.height = bmp->height;
    texData.format = bs::graphics::TextureFormat::RGBA8;

    bs::graphics::TextureID id = allocate_texture( &texData );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, id );

    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.f, screenSize.x, screenSize.y, 0.f, 0.f, 10.f );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glBegin( GL_TRIANGLES );

    float2 min = { -1, -1 };
    float2 max = { (float) bmp->width, (float) bmp->height };
    glTexCoord2f( 0.0f, 1.0f );
    glVertex2f( min.x, max.y );

    glTexCoord2f( 1.0f, 1.0f );
    glVertex2f( max.x, max.y );

    glTexCoord2f( 1.0f, 0.0f );
    glVertex2f( max.x, min.y );

    glTexCoord2f( 0.0f, 1.0f );
    glVertex2f( min.x, max.y );

    glTexCoord2f( 1.0f, 0.0f );
    glVertex2f( max.x, min.y );

    glTexCoord2f( 0.0f, 0.0f );
    glVertex2f( min.x, min.y );

    glBindTexture( GL_TEXTURE_2D, 0 );
    glEnd();

    //free_texture( id );
  }





  struct PrmTest
  {
    thread::ThreadInfo* threadInfo;
    bs::graphics::Mesh mesh;
    HGLRC renderContext;
  };
  DWORD WINAPI thread_bullcrap( void* void_parameter )
  {
    PrmTest* parameter = (PrmTest*) void_parameter;
    opengl::set_worker_thread_render_context( parameter->renderContext );

    static float3 vertices[] = {
              float3{-1.0f,-1.0f, -1.0f},
              float3{ 1.0f,-1.0f, -1.0f},
              float3{ 1.0f, 1.0f, -1.0f},
              float3{-1.0f, 1.0f, -1.0f},
    };

    static  float2 uvs[] = {
    float2{0.0f, 1.0f},
    float2{1.0f, 1.0f},
    float2{1.0f, 0.0f},
    float2{0.0f, 0.0f},
    };

    static  u16 indices[] =
    {
      0,1,2,
      0,2,3
    };
    bs::graphics::MeshData raw {};
    raw.vertices = vertices;
    raw.vertexCount = array_count( vertices );
    raw.indices = indices;
    raw.indexCount = array_count( indices );
    raw.uvs = uvs;
    raw.indexFormat = bs::graphics::IndexFormat::U16;
    parameter->mesh = allocate_mesh( &raw );

    check_gl_error();

    thread::write_barrier();
    parameter->threadInfo = nullptr;
    return 0;
  }

  void render_testDEBUG2( bs::graphics::RenderTarget* target, bs::graphics::RenderGroup* group, bs::graphics::Camera* camera )
  {
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f, -1.0f,
         1.0f,-1.0f, -1.0f,
         1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
    };
    static const GLfloat g_uv_buffer_data[] = {
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    };

    static const u16 g_index_buffer_data[] =
    {
      0,1,2,
      0,2,3
    };
    // static const GLfloat g_uv_buffer_data[] = {
    // 0.0f, 0.2f,
    // 0.2f, 0.2f,
    // 0.2f, 0.0f,
    // 0.0f, 0.2f,
    // 0.2f, 0.0f,
    // 0.0f, 0.0f,
    // };
    check_gl_error();

    static bs::graphics::Mesh meshFromThisThread {};
    static bs::graphics::RenderObject* renderObject;

    static GLuint vertexbuffer;
    static GLuint uvBuffer;
    static GLuint indexBuffer;
    static GLuint VertexArrayID;

    static int firsttime = 0;
    if ( !firsttime-- )
    {
      static float3 vertices[] = {
           float3{-1.0f,-1.0f, -1.0f},
           float3{ 1.0f,-1.0f, -1.0f},
           float3{ 1.0f, 1.0f, -1.0f},
           float3{-1.0f, 1.0f, -1.0f},
      };

      static  float2 uvs[] = {
      float2{0.0f, 1.0f},
      float2{1.0f, 1.0f},
      float2{1.0f, 0.0f},
      float2{0.0f, 0.0f},
      };

      static  u16 indices[] =
      {
        0,1,2,
        0,2,3
      };

      bs::graphics::MeshData raw {};
      raw.vertices = vertices;
      raw.vertexCount = array_count( vertices );
      raw.indices = indices;
      raw.indexCount = array_count( indices );
      raw.uvs = uvs;
      raw.indexFormat = bs::graphics::IndexFormat::U16;


      // meshFromThisThread = allocate_mesh( &raw );
       //   glGenVertexArrays( 1, &VertexArrayID );
        //  glBindVertexArray( VertexArrayID );
      check_gl_error();

      renderObject = (bs::graphics::RenderObject*) bs::memory::allocate_to_zero( sizeof( bs::graphics::RenderObject ) );
      //  renderObject->mesh = allocate_mesh( &raw );


      PrmTest poop = {};
      thread::ThreadInfo dumbStandaloneThreadTest {};
      poop.threadInfo = &dumbStandaloneThreadTest;
      poop.renderContext = opengl::create_render_context_for_worker_thread();
      check_gl_error();
      CloseHandle( CreateThread( 0, 0, thread_bullcrap, &poop, 0, (LPDWORD) &poop.threadInfo->id ) );

      //wait for the thread to shutdown
      while ( poop.threadInfo != nullptr ) {}

      renderObject->mesh = poop.mesh;
    }

    static float modelposy = 0.0f;
    modelposy += 0.02f;
    float4x4 model = float4x4::identity();
    model.pos = float4 { 0,2.0f * sinf( modelposy ), 0, 1 };

    float4x4 vp = bs::graphics::get_camera_view_projection_matrix( camera, (float)::global::mainWindow.size.x, (float)::global::mainWindow.size.y, 45.0f, 0.1f, 100.0f );
    float4x4 mvp = model * vp;

    glBindVertexArray( 0 );
    check_gl_error();

    glUseProgram( ::global::defaultGlyphTable->shaderProgram );
    uniformID mvpID = glGetUniformLocation( ::global::defaultGlyphTable->shaderProgram, "MVP" );
    glUniformMatrix4fv( mvpID, 1, GL_FALSE, &mvp.m00 );

    check_gl_error();

    GLuint textureID = ::global::defaultGlyphSheet->textureID;
    glBindTexture( GL_TEXTURE_2D, textureID );
    if ( !renderObject->id )
    {
      //  renderObject->id = init_vao_for_mesh( &renderObject->mesh );

      init_vao( renderObject );
    }
    check_gl_error();


    glBindVertexArray( renderObject->id );

    check_gl_error();

    GLenum glIndexFormat = get_gl_index_format( renderObject->mesh.indexFormat );
    glDrawElements( GL_TRIANGLES, renderObject->mesh.indexCount, glIndexFormat, (void*) 0 );

    glBindVertexArray( 0 );
    glDisableVertexAttribArray( 0 );
    glDisableVertexAttribArray( 1 );

    // glUseProgram( 0 );
    // glBindVertexArray( 0 );
    // glBindBuffer( GL_ARRAY_BUFFER, 0 );
    // glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    // glEnableVertexAttribArray( 0 );
    // glBindBuffer( GL_ARRAY_BUFFER, vertexbuffer );
    // glVertexAttribPointer(
    //    0,                  // attribute 0. match shader
    //    3,                  // size
    //    GL_FLOAT,           // type
    //    GL_FALSE,           // normalized?
    //    0,                  // stride
    //    (void*) 0           // array buffer offset
    // );
    // // Draw the triangle !

    // glEnableVertexAttribArray( 1 );
    // glBindBuffer( GL_ARRAY_BUFFER, uvBuffer );
    // glVertexAttribPointer(
    //     1,                                // attribute. match shader.
    //     2,                                // size
    //     GL_FLOAT,                         // type
    //     GL_FALSE,                         // normalized?
    //     0,                                // stride
    //     (void*) 0                         // array buffer offset
    // );

    // glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );

   // glDrawElements( GL_TRIANGLES, sizeof( g_index_buffer_data ), GL_UNSIGNED_SHORT, (void*) 0 );

  }

  void render_testSCENE( bs::graphics::RenderTarget* target, bs::graphics::Camera* camera, bs::scene::Object* sceneObjects, u32 sceneObjectCount )
  {
    float4x4 vp = bs::graphics::get_camera_view_projection_matrix( camera, (float)::global::mainWindow.size.x, (float)::global::mainWindow.size.y, 45.0f, 0.1f, 100.0f );

    GLuint id = ::global::defaultGlyphSheet->textureID;
    glBindTexture( GL_TEXTURE_2D, id );

    glUseProgram( ::global::defaultGlyphTable->shaderProgram );

    for ( u32 i = 0; i < sceneObjectCount; ++i )
    {
      bs::graphics::RenderObject* renderObject = sceneObjects[i].renderObject;

      if ( !renderObject->id )
      {
        init_vao( renderObject );
      }
      glBindVertexArray( renderObject->id );

      float4x4 mvp = sceneObjects[i].transform * vp;
      uniformID mvpID = glGetUniformLocation( ::global::defaultGlyphTable->shaderProgram, "MVP" );
      glUniformMatrix4fv( mvpID, 1, GL_FALSE, &mvp.m00 );

      //  u32 indexSize = get_size_for_index_format( renderObject->mesh.indexFormat );
      GLenum glIndexFormat = get_gl_index_format( renderObject->mesh.indexFormat );
      glDrawElements( GL_TRIANGLES, renderObject->mesh.indexCount, glIndexFormat, (void*) 0 );

      glBindVertexArray( 0 );
    }
    glUseProgram( 0 );
  }

  void render( bs::graphics::RenderTarget* target, bs::graphics::RenderGroup* group, bs::graphics::Camera* camera )
  {
    render_testDEBUG2( target, group, camera );

    switch ( group->type )
    {
      case bs::graphics::RenderGroup::TEXT_AREA:
      {
        //render_text_area_fixed_pipeline( target, (bs::ui::TextArea*) group->renderObject );
        break;
      }
      case bs::graphics::RenderGroup::CUSTOM_BITMAP:
      {
        render_custom_bitmap( target, (bs::graphics::Bitmap*) group->renderObject );
        break;
      }
      case bs::graphics::RenderGroup::SCENE_OBJECTS:
      {
        render_testSCENE( target, camera, (bs::scene::Object*) group->renderObject, group->count );
        break;
      }
      default:
      {
        //  render_testDEBUG( target, group, camera );
        break;
      }
    }
  }

  u32 validate_shader( ShaderID shaderID )
  {
    constexpr s32 MAX_INFO_LOG_LENGTH = 512;
    char infoLog[MAX_INFO_LOG_LENGTH];
    GLint result = GL_FALSE;
    s32 infoLogLength = 0;
    glGetShaderiv( shaderID, GL_COMPILE_STATUS, &result );
    if ( result != GL_TRUE )
    {
      glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &infoLogLength );
      glGetShaderInfoLog( shaderID, min( infoLogLength, MAX_INFO_LOG_LENGTH ), NULL, infoLog );
      log_error( "[OGL] Errors: \n", infoLog );
      BREAK;
    }
    return result == GL_TRUE;
  }

  u32 validate_program( ProgramID programID )
  {
    constexpr s32 MAX_INFO_LOG_LENGTH = 512;
    char infoLog[MAX_INFO_LOG_LENGTH];
    GLint result = GL_FALSE;
    s32 infoLogLength = 0;
    glGetProgramiv( programID, GL_LINK_STATUS, &result );
    if ( result != GL_TRUE )
    {
      glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &infoLogLength );
      glGetProgramInfoLog( programID, min( infoLogLength, MAX_INFO_LOG_LENGTH ), NULL, infoLog );
      log_error( "[OGL] Errors: \n", infoLog );
      BREAK;
    }
    return result == GL_TRUE;
  }

  ProgramID create_shader_program( bs::file::Data headerFileData, bs::file::Data vsFileData, bs::file::Data fsFileData )
  {
    log_info( "[OGL] Compiling shader." );

    GLint shaderCodeLengths[] = { (s32) headerFileData.size, (s32) vsFileData.size };

    ShaderID vsID = glCreateShader( GL_VERTEX_SHADER );
    GLchar const* vsCode[] =
    {
      (char const*) headerFileData.data,
      (char const*) vsFileData.data
    };
    glShaderSource( vsID, array_count( vsCode ), vsCode, shaderCodeLengths );
    glCompileShader( vsID );

    validate_shader( vsID );

    shaderCodeLengths[1] = (s32) fsFileData.size;

    ShaderID fsID = glCreateShader( GL_FRAGMENT_SHADER );
    GLchar const* fsCode[] =
    {
      (char const*) headerFileData.data,
      (char const*) fsFileData.data
    };
    glShaderSource( fsID, array_count( fsCode ), fsCode, shaderCodeLengths );
    glCompileShader( fsID );

    validate_shader( fsID );

    log_info( "[OGL] Linking program." );

    GLuint programID = glCreateProgram();
    glAttachShader( programID, vsID );
    glAttachShader( programID, fsID );
    glLinkProgram( programID );

    validate_program( programID );

    glDetachShader( programID, vsID );
    glDetachShader( programID, fsID );

    glDeleteShader( vsID );
    glDeleteShader( fsID );

    return programID;
  }

  ProgramID create_shader_program( bs::file::Data combinedglsl )
  {
    char* reader = (char*) combinedglsl.data;
    char* end = reader + combinedglsl.size;
    bs::file::Data h;
    bs::file::Data vs;
    bs::file::Data fs;

    //header
    char* nextSection = bs::string_contains( reader, "#h" );
    assert( nextSection );
    char* currentSection = nextSection + 2;

    nextSection = bs::string_contains( currentSection, "#vs" );
    assert( nextSection );

    h.data = currentSection;
    h.size = nextSection - currentSection;

    //vertex shader
    currentSection = nextSection + 3;
    nextSection = bs::string_contains( currentSection, "#fs" );
    assert( nextSection );

    vs.data = currentSection;
    vs.size = nextSection - currentSection;

    //fragment shader
    currentSection = nextSection + 3;
    nextSection = end;

    fs.data = currentSection;
    fs.size = nextSection - currentSection;

    return create_shader_program( h, vs, fs );
  }
};

DEPRECATED

#pragma once




namespace opengl
{

  using ProgramID = u32;

  ProgramID create_shader_program( bs::file::Data headerFileData, bs::file::Data vsFileData, bs::file::Data fsFileData );
  //all three files in one, mark sections with #h, #vs and #fs.
  //example file content:
  //#h
  //#version 450 core
  //#vs
  //void main() { ... }
  //#fs
  //void main() { ... }
  //
  ProgramID create_shader_program( bs::file::Data combinedglsl );

};


////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////inl/////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


namespace opengl
{
  using ShaderID = u32;

  u32 validate_shader( ShaderID shaderID )
  {
    constexpr s32 MAX_INFO_LOG_LENGTH = 512;
    char infoLog[MAX_INFO_LOG_LENGTH];
    GLint result = GL_FALSE;
    s32 infoLogLength = 0;
    glGetShaderiv( shaderID, GL_COMPILE_STATUS, &result );
    if ( result != GL_TRUE )
    {
      glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &infoLogLength );
      glGetShaderInfoLog( shaderID, min( infoLogLength, MAX_INFO_LOG_LENGTH ), NULL, infoLog );
      log_error( "[OGL] Errors: \n", infoLog );
      BREAK;
    }
    return result == GL_TRUE;
  }

  u32 validate_program( ProgramID programID )
  {
    constexpr s32 MAX_INFO_LOG_LENGTH = 512;
    char infoLog[MAX_INFO_LOG_LENGTH];
    GLint result = GL_FALSE;
    s32 infoLogLength = 0;
    glGetProgramiv( programID, GL_LINK_STATUS, &result );
    if ( result != GL_TRUE )
    {
      glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &infoLogLength );
      glGetProgramInfoLog( programID, min( infoLogLength, MAX_INFO_LOG_LENGTH ), NULL, infoLog );
      log_error( "[OGL] Errors: \n", infoLog );
      BREAK;
    }
    return result == GL_TRUE;
  }

  ProgramID create_shader_program( bs::file::Data headerFileData, bs::file::Data vsFileData, bs::file::Data fsFileData )
  {
    log_info( "[OGL] Compiling shader." );

    GLint shaderCodeLengths[] = { (s32) headerFileData.size, (s32) vsFileData.size };

    ShaderID vsID = glCreateShader( GL_VERTEX_SHADER );
    GLchar const* vsCode[] =
    {
      (char const*) headerFileData.data,
      (char const*) vsFileData.data
    };
    glShaderSource( vsID, array_count( vsCode ), vsCode, shaderCodeLengths );
    glCompileShader( vsID );

    validate_shader( vsID );

    shaderCodeLengths[1] = (s32) fsFileData.size;

    ShaderID fsID = glCreateShader( GL_FRAGMENT_SHADER );
    GLchar const* fsCode[] =
    {
      (char const*) headerFileData.data,
      (char const*) fsFileData.data
    };
    glShaderSource( fsID, array_count( fsCode ), fsCode, shaderCodeLengths );
    glCompileShader( fsID );

    validate_shader( fsID );

    log_info( "[OGL] Linking program." );

    GLuint programID = glCreateProgram();
    glAttachShader( programID, vsID );
    glAttachShader( programID, fsID );
    glLinkProgram( programID );

    validate_program( programID );

    glDetachShader( programID, vsID );
    glDetachShader( programID, fsID );

    glDeleteShader( vsID );
    glDeleteShader( fsID );

    return programID;
  }

  ProgramID create_shader_program( bs::file::Data combinedglsl )
  {
    char* reader = (char*) combinedglsl.data;
    char* end = reader + combinedglsl.size;
    bs::file::Data h;
    bs::file::Data vs;
    bs::file::Data fs;

    //header
    char* nextSection = bs::string_contains( reader, "#h" );
    assert( nextSection );
    char* currentSection = nextSection + 2;

    nextSection = bs::string_contains( currentSection, "#vs" );
    assert( nextSection );

    h.data = currentSection;
    h.size = nextSection - currentSection;

    //vertex shader
    currentSection = nextSection + 3;
    nextSection = bs::string_contains( currentSection, "#fs" );
    assert( nextSection );

    vs.data = currentSection;
    vs.size = nextSection - currentSection;

    //fragment shader
    currentSection = nextSection + 3;
    nextSection = end;

    fs.data = currentSection;
    fs.size = nextSection - currentSection;

    return create_shader_program( h, vs, fs );
  }
};

#pragma once

DEPRECATED

#include <common/bs_common.h>

namespace bs { namespace font { struct GlyphTable; struct GlyphSheet; }; };
namespace bs
{
  namespace ui
  {
    struct TextArea;

    //font can be nullptr
    //dimensions relative to glyph scale
    TextArea* create_text_area_from_text( char const* text, font::GlyphTable* font, float2 dimensions );
  };
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////cpp//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


namespace bs
{
  namespace ui
  {
    struct TextLine
    {
      s32* codepoints;
      s32 capacity;
    };
    struct TextArea
    {
      font::GlyphTable* glyphTable;
      font::GlyphSheet* glyphSheet; //TODO
      TextLine* lines;
      s32 lineCount;
    };

    TextArea* create_text_area_from_text( char const* text, font::GlyphTable* glyphTable, float2 dimensions )
    {
      s32 lineCount = string_line_count( text );
      s32 totalTextLength = string_length_utf8( text ) + 1;

      u8* allocation = (u8*) memory::allocate( sizeof( TextArea ) + sizeof( TextLine ) * lineCount + sizeof( s32 ) * totalTextLength );
      TextArea* resultArea = (TextArea*) allocation;
      resultArea->glyphTable = glyphTable;
      resultArea->lines = (TextLine*) (allocation + sizeof( TextArea ));
      resultArea->lineCount = lineCount;

      char const* reader = text;
      s32* writer = (s32*) (allocation + sizeof( TextArea ) + sizeof( TextLine ) * lineCount);

      s32 bytesWritten = 0;
      for ( s32 i = 0; i < lineCount; ++i )
      {
        TextLine& textLine = resultArea->lines[i];
        textLine.codepoints = writer;
        textLine.capacity = 0;

        s32 const end = i == lineCount - 1 ? (s32) '\0' : (s32) '\n';
        s32 codepoint = 1;
        while ( codepoint != end )
        {
          reader = string_parse_utf8( reader, &codepoint );
          *writer++ = codepoint;
          ++textLine.capacity;
        }

        bytesWritten += textLine.capacity;
      }
      assert( bytesWritten == totalTextLength );

      return resultArea;
    }

  };
};


DEPRECATED

#include <ui/bstextarea.h>
#include <scene/bssceneobject.h>

#include <core/bsgraphics.h>
#include <common/bscolor.h>

#include <platform/platform.h>

namespace bs
{
  void app_sample_sound( PrmAppSampleSound prm )
  {
    BREAK;
  }

  static ui::TextArea* testArea;
  static graphics::Camera camera;

  static graphics::RenderObject* renderObject;

  static scene::Object sceneObject;

  void app_on_load( PrmAppOnLoad prm )
  {
    char const* text = "dobiieboo\nhello Klara, my email is:\ncamillolukesch@gmail.com\nPlease mail me. <3.";
    testArea = ui::create_text_area_from_text( text, nullptr, { 0,0 } );

    camera.transform = float4x4::identity();
    camera.transform.pos.xyz = float3 { 0,0,20 };


    static float3 vertices[] = {
          float3{-1.0f,-1.0f, -1.0f},
          float3{ 1.0f,-1.0f, -1.0f},
          float3{ 1.0f, 1.0f, -1.0f},
          float3{-1.0f, 1.0f, -1.0f},
    };

    static  float2 uvs[] = {
    float2{0.0f, 1.0f},
    float2{1.0f, 1.0f},
    float2{1.0f, 0.0f},
    float2{0.0f, 0.0f},
    };

    static  u16 indices[] =
    {
      0,1,2,
      0,2,3
    };

    graphics::MeshData raw {};
    raw.vertices = vertices;
    raw.vertexCount = array_count( vertices );
    raw.indices = indices;
    raw.indexCount = array_count( indices );
    raw.uvs = uvs;
    raw.indexFormat = graphics::IndexFormat::U16;

    //  renderObject = graphics::create_render_object( &raw, nullptr, nullptr );

    sceneObject.transform = float4x4::identity();
    sceneObject.renderObject = renderObject;

  }

  void app_tick( PrmAppTick prm )
  {
    static s32 firstTime = 0;
    if ( !firstTime-- )
    {
      static float3 vertices[] = {
      float3{-1.0f,-1.0f, -1.0f},
      float3{ 1.0f,-1.0f, -1.0f},
      float3{ 1.0f, 1.0f, -1.0f},
      float3{-1.0f, 1.0f, -1.0f},
      };

      static  float2 uvs[] = {
      float2{0.0f, 1.0f},
      float2{1.0f, 1.0f},
      float2{1.0f, 0.0f},
      float2{0.0f, 0.0f},
      };

      static  u16 indices[] =
      {
        0,1,2,
        0,2,3
      };

      graphics::MeshData raw {};
      raw.vertices = vertices;
      raw.vertexCount = array_count( vertices );
      raw.indices = indices;
      raw.indexCount = array_count( indices );
      raw.uvs = uvs;
      raw.indexFormat = graphics::IndexFormat::U16;
      renderObject = graphics::create_render_object( &raw, nullptr, nullptr );

      sceneObject.transform = float4x4::identity();
      sceneObject.renderObject = renderObject;
    }

    graphics::RenderGroup rg = graphics::render_group_from_text_area( testArea );

    graphics::RenderGroup rg2 = graphics::render_group_from_scene_objects( &sceneObject, 1 );

    static float modelposy = 0.0f;
    modelposy += 0.02f;
    sceneObject.transform.pos = float4 { 2.0f * sinf( modelposy ), 0, 0, 1 };


    graphics::camera_move( &camera, &prm.appData->input );
    platform::render( nullptr, &rg, &camera );
    //platform::render( nullptr, &rg2, &camera );
  }

  void app_receive_udp_packet( PrmAppReceiveUDPPacket prm )
  {
    BREAK;
  }
};

DEPRECATED
#pragma once

#include <common/bsmath.h>
#include <common/bsmatrix.h>

namespace bs
{
  namespace input { struct State; }
  namespace ui { struct TextArea; }
  namespace scene { struct Object; }

  namespace graphics
  {
    using TextureID = u32;
    using VertexBufferID = u32;
    using UVBufferID = u32;
    using IndexBufferID = u32;
    using ShaderProgram = u32;
    using RenderObjectID = u32;

    struct Bitmap;

    enum class IndexFormat: u32
    {
      INVALID = 0,
      U16 = 1,
      U32 = 2,
    };

    struct MeshData
    {
      float3* vertices;
      float2* uvs;
      void* indices;
      u32 vertexCount;
      u32 indexCount;
      IndexFormat indexFormat;
    };

    struct Mesh
    {
      VertexBufferID vb;
      UVBufferID uvb;
      IndexBufferID ib;
      u32 indexCount;
      IndexFormat indexFormat;
    };

    enum class TextureFormat: u32
    {
      INVALID = 0,
      RGBA8 = 1,
    };

    struct TextureData
    {
      void* pixel;
      TextureFormat format;
      s32 width;
      s32 height;
    };

    struct RenderObject
    {
      // enum : u16
      // {
      //   SCENE_OBJECT = 1,
      // } type;
      // enum : u8
      // {
      //   NONE = 0,
      // } subtype;

      // u32 flags;
      RenderObjectID id;
      TextureID diffuseTexture;
      TextureID normalMap;
      Mesh mesh;
    };

    struct RenderGroup
    {
      enum: u32
      {
        SCENE_OBJECTS,
        TEXT_AREA,
        CUSTOM_BITMAP,
      } type;
      void* renderObject;
      u32 count;
    };

    RenderObject* create_render_object( MeshData const* meshData, TextureData const* diffuseTextureData, TextureData const* normalMapData );

    RenderGroup render_group_from_text_area( ui::TextArea* ta );
    RenderGroup render_group_from_custom_bitmap( Bitmap* bmp );

    RenderGroup render_group_from_scene_objects( scene::Object* objects, u32 objectCount );


    struct RenderTarget;

    struct Rect
    {
      float2 pos;
      float2 size;
    };

    struct Camera
    {
      float4x4 transform;
      float yaw;
      float pitch;
    };

    void camera_init( Camera* camera, float viewportWidth, float viewportHeight, float fovRadians, float zNear, float zFar );
    void camera_move( Camera* camera, input::State* inputState );

    struct Bitmap
    {
      u32* pixel;
      s32 width;
      s32 height;
    };
  };
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////inl//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#include <core/bsinput.h>
namespace bs
{
  namespace graphics
  {
    struct RenderTarget
    {
      void* placeholder;
    };

    RenderGroup render_group_from_text_area( ui::TextArea* ta )
    {
      RenderGroup result {};
      result.type = RenderGroup::TEXT_AREA;
      result.renderObject = ta;
      return result;
    }

    RenderGroup render_group_from_custom_bitmap( Bitmap* bmp )
    {
      RenderGroup result {};
      result.type = RenderGroup::CUSTOM_BITMAP;
      result.renderObject = bmp;
      return result;
    }

    RenderGroup render_group_from_scene_objects( scene::Object* objects, u32 objectCount )
    {
      RenderGroup result {};
      result.type = RenderGroup::SCENE_OBJECTS;
      result.renderObject = objects;
      result.count = objectCount;
      return result;
    }

    void camera_init( Camera* camera, float fovRadians )
    {
      camera->transform = float4x4::identity();
      camera->yaw = 0.0f;
      camera->pitch = 0.0f;
    }

    float4x4 get_camera_view_projection_matrix( Camera* camera, float viewportWidth, float viewportHeight, float fovRadians, float zNear, float zFar )
    {
      float tanHalfFovx = 1.0f / tanf( fovRadians * 0.5f );
      float aspectRatio = viewportWidth / viewportHeight;
      float z = 1.0f / (zFar - zNear);
      float4x4 m = float4x4::identity();
      m.m00 = tanHalfFovx;
      m.m11 = aspectRatio * tanHalfFovx;
      m.m22 = -zFar * z;
      m.m23 = -1.0f;
      m.m32 = -(zFar * zNear) * z;
      m.m33 = 0.0f;

      return matrix_inverse_orthonormal( camera->transform ) * m;
    }

    void camera_move( Camera* camera, input::State* inputState )
    {
      input::State& input = *inputState;
      float2 mouseDelta = float2 { input.mousePos[0].end - input.mousePos[0].start };
      if ( input.held[input::KEY_CTRL] )
      {
        float factor = 0.005f;
        camera->yaw -= factor * mouseDelta.x;
        camera->pitch -= factor * mouseDelta.y;
      }

      float4x4& transform = camera->transform;
      float4 pos = transform.pos;
      transform = matrix_from_euler( camera->yaw, camera->pitch );

      float4 const& xAxis = transform.xAxis;
      float4 const& yAxis = transform.yAxis;
      float4 const& zAxis = transform.zAxis;

      float4 move = float4 { 0.0f };
      float speed = .5f;

      if ( input.held[input::KEY_A] )
      {
        move -= xAxis * speed;
      }
      if ( input.held[input::KEY_D] )
      {
        move += xAxis * speed;
      }

      if ( input.held[input::KEY_W] )
      {
        move -= zAxis * speed;
      }
      if ( input.held[input::KEY_S] )
      {
        move += zAxis * speed;
      }

      if ( input.held[input::KEY_Q] )
      {
        move -= yAxis * speed;
      }
      if ( input.held[input::KEY_E] )
      {
        move += yAxis * speed;
      }
      transform.pos = pos + move;
    }
  };
};

DEPRECATED
#pragma once


#include <platform/bs_platform.h>
#include <common/bs_common.h>


namespace bs
{
  //for memory blocks larger than 4GB
  struct GeneralAllocator64
  {
    void* allocate( s64 size, u32 alignment = 16 ) { return nullptr; }
    void deallocate( void* ) {}
    char* bufferBegin;
    char* bufferEnd;
    struct Entry
    {
      char* begin;
      char* end;
    };
    Entry* lastEntry;
    Entry* nextEntry;
  };

  [[nodiscard]]
  GeneralAllocator64* create_general_allocator64( void* memoryBlock, s64 sizeOfBlockInBytes );


  //for memory blocks smaller than 4GB
  struct GeneralAllocator32
  {
    void* allocate( u32 size, u32 alignment = 16 );
    void deallocate( void* );
    char* bufferBegin;
    char* bufferEnd;
    struct Entry
    {
      u32 begin;
      u32 end;
    };
    Entry* lastEntry;
    Entry* nextEntry;
  };

  [[nodiscard]]
  GeneralAllocator32* create_general_allocator32( void* memoryBlock, u32 sizeOfBlockInBytes );


};


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////cpp/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


namespace bs
{
  void* GeneralAllocator32::allocate( u32 size, u32 alignment = 16 )
  {

    char* result = nullptr;

    u32 expectedOffset = lastEntry->end;



    current += ((u64) current) & (alignment - 1);
  }

  void GeneralAllocator32::deallocate( void* )
  {

  }

  GeneralAllocator32* create_general_allocator32( void* memoryBlock, u32 sizeOfBlockInBytes )
  {
    if ( sizeOfBlockInBytes < 1024 )
    {
      BREAK; //that's a bit of a small block to be allocating into, isn't it?
    }
    char* allocation = (char*) memoryBlock;

    GeneralAllocator64* result = (GeneralAllocator64*) allocation;
    result->bufferBegin = allocation;
    result->bufferEnd = allocation + sizeOfBlockInBytes;

    result->lastEntry = (GeneralAllocator64::Entry*) result->bufferEnd;
    result->nextEntry = result->lastEntry - 1;
  }

  GeneralAllocator64* create_general_allocator64( void* memoryBlock, s64 sizeOfBlockInBytes )
  {
    if ( sizeOfBlockInBytes < 1024 )
    {
      BREAK; //that's a bit of a small block to be allocating into, isn't it?
    }
    char* allocation = (char*) memoryBlock;

    GeneralAllocator64* result = (GeneralAllocator64*) allocation;
    result->bufferBegin = allocation;
    result->bufferEnd = allocation + sizeOfBlockInBytes;

    result->lastEntry = (GeneralAllocator64::Entry*) result->bufferEnd;
    result->nextEntry = result->lastEntry - 1;
  }
};
};

DEPRECATED

#pragma once

#include "win32_global.h"

//#include <core/bs_debuglog.h>
#include <core/bsfile.h>
#include <common/bs_string.h>
#include <common/bs_common.h>

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