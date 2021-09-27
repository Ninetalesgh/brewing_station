#pragma once

#ifndef BS_BUILD_NETWORKING
# define BS_BUILD_NETWORKING 1
#endif

#ifndef BS_BUILD_SOUND 
# define BS_BUILD_SOUND 1
#endif


#if BS_BUILD_NETWORKING
#include "net.h"
#endif
#include "common/basic_math.h"
#include "common/memory.h"

constexpr char  APP_FILENAME[]              = "scifi.dll";
constexpr char  APP_NAME[]                  = "scifi";
constexpr char  APP_VERSION[]               = "0.1";

constexpr char  APP_SERVER_IPv4_ADDRESS[]   = "127.0.0.1";
constexpr u32   APP_SERVER_PORT             = 6668;
constexpr u32   APP_NETWORK_PACKET_SIZE_MAX = 1024;
constexpr u32   APP_CLIENT_COUNT_MAX        = 24;

constexpr u64   APP_STATIC_BUFFER_SIZE      = MegaBytes( 128 );
constexpr u64   APP_TEMP_BUFFER_SIZE        = MegaBytes( 512 );
constexpr s32   APP_UPDATE_HZ               = 60;

struct FileInfo;
struct BackBuffer;
struct SoundBuffer;
struct AppData;
struct PlatformData;

namespace platform
{
  struct ReadFileResult
  {
    void* data;
    u32 size;
  };

  struct WriteFileParameter
  {
    char const* filename;
    void* data;
    s32 size;

    enum : u32
    {
      CLOSE,
      KEEP_OPEN,

    };
  };

  using get_file_info = s32( char const*, FileInfo* );
  using     read_file = ReadFileResult( char const*, u32, void* ); //TODO
  using     free_file = void( void* );
  using    write_file = u32( char const*, void* const*, s32 const*, s32 );

  #ifdef BS_BUILD_NETWORKING
  using      send_udp = void( net::UDPSendParameter const& );
  using      send_tcp = void( net::TCPSendParameter const& );
  #endif

  namespace stub
  {
    s32        get_file_info( char const*, FileInfo* out_fileInfo ) { return 0; }
    ReadFileResult read_file( char const*, u32, void* ) { return {}; }
    void           free_file( void* ) {}
    u32           write_file( char const* filename, void* const* data, s32 const* size, s32 count ) { return 0; }



    #ifdef BS_BUILD_NETWORKING
    void            send_udp( net::UDPSendParameter const& parameter ) {}
    void            send_tcp( net::TCPSendParameter const& parameter ) {}
    #endif
  };
};

//platform calls:
#if BS_BUILD_SOUND
extern "C" void app_sample_sound( AppData & appData, SoundBuffer & soundBuffer ); //TODO if multithreaded const appdata? 
#endif

extern "C" void app_on_load( PlatformData const& platformData, AppData & appData );

extern "C" void app_tick( PlatformData const& platformData, AppData & appData, BackBuffer & backBuffer );

#ifdef BS_BUILD_NETWORKING
extern "C" void app_receive_udp_packet( PlatformData const& platformData, AppData & appData, net::UDPReceiveParameter const& parameter );
#endif

//TODO
struct ThreadInfo
{
  u32 id;
};

struct FileInfo
{
  u64 size;
};

struct BackBuffer
{
  //REQUIRED FOR RENDERING:
  //backbuffer the game is supposed to render the image to
  void* data;
  s32   width;
  s32   height;
  s32   pitch;
  s32   bytesPerPixel;
};

struct SoundBuffer
{
  //REQUIRED FOR SOUND:
  //buffer for sound samples to render the sound to
  s32  samplesPerSecond;
  s32  sampleCount;
  s16* samples;
};

struct AppData
{
  //REQUIRED:
  //preallocated memory for the application
  void* staticBuffer;
  u32   staticBufferSize;

  memory::Arena generalPurposeArena;

  u64 currentFrameIndex;

  #if !BS_BUILD_RELEASE
  u32 debug_trigger[10];
  u64 debug_clockRender;
  u64 debug_clockInputProcessing;
  #endif
};

//TODO
struct ButtonState
{
  s32 halfTransitionCount;
  u32 endedDown;
};

struct ControllerInput
{
  float2 start;
  float2 min;
  float2 max;
  float2 end;

  union
  {
    ButtonState button[6];
    struct
    {
      ButtonState up;
      ButtonState down;
      ButtonState left;
      ButtonState right;
      ButtonState leftShoulder;
      ButtonState rightShoulder;
    };
  };
};

struct MousePosition
{
  int2 start;
  int2 end;
  int2 min;
  int2 max;
};

struct Input
{
  constexpr_member s32 STATE_COUNT = 0xff;
  enum : u8
  {
    MOUSE_PRIMARY            = 1,
    MOUSE_SECONDARY          = 2,
    MOUSE_3                  = 4,
    MOUSE_4                  = 5,
    MOUSE_5                  = 6,
    KEY_BACK                 = 8,
    KEY_TAB                  = 9,
    KEY_ENTER                = 13,
    KEY_SHIFT                = 16,
    KEY_CTRL                 = 17,
    KEY_ALT                  = 18,
    KEY_PAUSE                = 19,
    KEY_CAPSLOCK             = 20,
    KEY_ESCAPE               = 27,
    KEY_SPACE                = 32,
    KEY_PGUP                 = 33,
    KEY_PGDOWN               = 34,
    KEY_END                  = 35,
    KEY_HOME                 = 36,
    KEY_LEFT                 = 37,
    KEY_UP                   = 38,
    KEY_RIGHT                = 39,
    KEY_DOWN                 = 40,
    KEY_DELETE               = 46,
    KEY_0                    = 48,
    KEY_1                    = 49,
    KEY_2                    = 50,
    KEY_3                    = 51,
    KEY_4                    = 52,
    KEY_5                    = 53,
    KEY_6                    = 54,
    KEY_7                    = 55,
    KEY_8                    = 56,
    KEY_9                    = 57,
    KEY_A                    = 65,
    KEY_B                    = 66,
    KEY_C                    = 67,
    KEY_D                    = 68,
    KEY_E                    = 69,
    KEY_F                    = 70,
    KEY_G                    = 71,
    KEY_H                    = 72,
    KEY_I                    = 73,
    KEY_J                    = 74,
    KEY_K                    = 75,
    KEY_L                    = 76,
    KEY_M                    = 77,
    KEY_N                    = 78,
    KEY_O                    = 79,
    KEY_P                    = 80,
    KEY_Q                    = 81,
    KEY_R                    = 82,
    KEY_S                    = 83,
    KEY_T                    = 84,
    KEY_U                    = 85,
    KEY_V                    = 86,
    KEY_W                    = 87,
    KEY_X                    = 88,
    KEY_Y                    = 89,
    KEY_Z                    = 90,
    KEY_WINDOWS              = 91,
    KEY_MENU                 = 93,
    KEY_INSERT               = 45,
    KEY_F1                   = 112,
    KEY_F2                   = 113,
    KEY_F3                   = 114,
    KEY_F4                   = 115,
    KEY_F5                   = 116,
    KEY_F6                   = 117,
    KEY_F7                   = 118,
    KEY_F8                   = 119,
    KEY_F9                   = 120,
    KEY_F10                  = 121,
    KEY_F11                  = 122,
    KEY_F12                  = 123,
    KEY_SEMICOLON            = 186,
    KEY_EQUALS               = 187,
    KEY_COMMA                = 188,
    KEY_MINUS                = 189,
    KEY_PERIOD               = 190,
    KEY_SLASH                = 191,
    KEY_APOSTROPHE           = 192,
    KEY_SQUARE_BRACKET_OPEN  = 219,
    KEY_SQUARE_BRACKET_CLOSE = 221,
    KEY_BACKSLASH            = 220,
    KEY_QUOTE                = 222
  };

  ControllerInput controller[4];
  MousePosition mousePos[1];
  s32 mouseWheelDelta;

  //mouse & keyboard
  u8 down[STATE_COUNT];
  u8 held[STATE_COUNT];
};

struct NetworkData
{
  net::Connection self;
  net::Connection server;
  net::Connection connections[APP_CLIENT_COUNT_MAX];
  u32             connectionCount;
};

struct PlatformData
{
  Input input;
  NetworkData network;

  platform::get_file_info* get_file_info;
  platform::read_file* read_file;
  platform::write_file* write_file;
  platform::free_file* free_file;

  #if BS_BUILD_NETWORKING
  platform::send_udp* send_udp;
  platform::send_tcp* send_tcp;
  #endif
};