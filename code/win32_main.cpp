#ifndef BS_BUILD_RELEASE
# define BS_BUILD_DEBUG   1
# define BS_BUILD_DEVELOP 0
# define BS_BUILD_RELEASE 0
#endif

#ifndef BS_BUILD_SERVER
# define BS_BUILD_SERVER 0
# define BS_BUILD_CLIENT 0
#endif


////////////////////////////////////////

#if BS_BUILD_DEBUG
char global_debugUsername[] = "debug_xamilla";
#elif BS_BUILD_DEVELOP
char global_debugUsername[] = "develop_xamilla";
#elif BS_BUILD_RELEASE
char global_debugUsername[] = "release_xamilla";
#endif

#if BS_BUILD_RELEASE
# include "apps\scifi.cpp"
#else
# include "platform.h"
# define DEBUG_LOG_SOUND                     0
# define DEBUG_LOG_INPUT                     0
# define DEBUG_LOG_DLL_FREE                  0
# define DEBUG_LOG_CLOCK_FPS                 0
# define DEBUG_LOG_CLOCK_CYCLES              0
# define DEBUG_LOG_CLOCK_SLEEP_DELTA         0
# define DEBUG_LOG_CLOCK_MISSED_FRAME        1
# define DEBUG_DRAW_SOUND                    0
# define DEBUG_LOG_APP_ACTIVATED_DEACTIVATED 0
#endif

#include "common/profile.h"
#include "common/atomic.h"
#include "common/basic_rasterizer.h"

#define BS_STRING_IMPLEMENTATION
#include "common/string.h"

//winsock
#pragma comment(lib,"ws2_32.lib")
//WinMain
#pragma comment(lib,"user32.lib")
//StretchDIBits
#pragma comment(lib,"Gdi32.lib")
//time?
#pragma comment(lib,"winmm.lib")
//CoInitialize
#pragma comment(lib,"Ole32.lib")

#include "win32/win32_net.cpp"
#include "win32/win32_global.h"

#include <dsound.h>
#include <Xinput.h>

#include <windows.h>
#ifdef min
# undef min
#endif
#ifdef max
# undef max
#endif

constexpr float APP_TARGET_SPF = 1.0f / float( APP_UPDATE_HZ );

namespace global
{
  global_variable win32::GlobalData win32Data;
  global_variable win32::GlobalNetworkData netData;
};

namespace win32
{
  INLINE LARGE_INTEGER GetTimer()
  {
    LARGE_INTEGER result;
    QueryPerformanceCounter( &result );
    return result;
  }

  INLINE float GetSecondsElapsed( LARGE_INTEGER beginCounter, LARGE_INTEGER endCounter )
  {
    return float( endCounter.QuadPart - beginCounter.QuadPart ) / float( global::win32Data.performanceCounterFrequency );
  }
};

s32 debug_GetFileInfo( char const* filename, FileInfo* out_fileInfo )
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

void OpenPeerConnection( net::Connection connection )
{
  log_info( "[WIN32_NET] Opening peer connection: ", connection );
  global::netData.connections[global::netData.connectionCount] = connection;
  global::netData.connectionCount.increment();
}

void ClosePeerConnection( net::Connection connection )
{
  log_info( "[WIN32_NET] Closing peer connection: ", connection );

  {
    LOCK_SCOPE( global::netData.guard_connections );
    for ( s32 i = 0; i < global::netData.connectionCount;++i )
    {
      if ( connection == global::netData.connections[i] )
      {
        global::netData.connections[i] = global::netData.connections[global::netData.connectionCount - 1];
        break;
      }
    }

    global::netData.connectionCount.decrement_unsafe();
  }
}


struct TCPFileTransferReceiverParameter
{
  ThreadInfo       threadInfo;
  net::Connection  connection;
  HANDLE           fileHandle;
};
DWORD thread_TCPFileTransferReceiver( void* void_parameter )
{
  constexpr float WAIT_FOR_TCP_SECONDS_MAX = 30.0f;
  TCPFileTransferReceiverParameter& parameter = *(TCPFileTransferReceiverParameter*) void_parameter;

  log_info( "[WIN32_THREAD] TCPFileTransferReceiver launching; thread id: ", parameter.threadInfo.id, ".\n" );

  SOCKET receiverSocket;
  if ( win32::tcp_init_socket( &receiverSocket ) )
  {
    auto beginTimer = win32::GetTimer();
    while ( win32::GetSecondsElapsed( beginTimer, win32::GetTimer() ) < WAIT_FOR_TCP_SECONDS_MAX )
    {
      if ( win32::tcp_connect( receiverSocket, parameter.connection ) )
      {
        log_info( "WIN32_NET] TCP connection established with: ", parameter.connection );

        constexpr s32 bufferSize = (s32) KiloBytes( 16 );
        char receiveBuffer[bufferSize];
        for ( ;; )
        {
          s32 bytesReceived;
          if ( win32::tcp_receive( receiverSocket, receiveBuffer, bufferSize, &bytesReceived ) )
          {
            if ( bytesReceived )
            {
              DWORD bytesWritten;
              if ( WriteFile( parameter.fileHandle, receiveBuffer, bytesReceived, &bytesWritten, 0 ) )
              {
              }
              else
              {
                log_error( "[WIN32_NET] ERROR - writing file failed." );
              }

              // if ( bytesReceived < bufferSize )
              // {
              //   log_info( "[WIN32_NET] Transfer complete." );
              //   //break;
              // }
            }
            else
            {
              log_info( "[WIN32_NET] received termination packet from: ", parameter.connection, ".\n" );
              break;
            }
          }
          else
          {
            log_error( "[WIN32_NET] ERROR - TCP receive failed. WSA Code: ", WSAGetLastError() );
          }
        }
        break;
      }
      else
      {
        log_info( "[WIN32_NET] ERROR - connecting to tcp sender. WSA Code: ", WSAGetLastError(), ", Attempting again.\n" );
        Sleep( 10 );
      }
    }
  }
  else
  {
    log_error( "[WIN32_NET] ERROR - TCP socket invalid." );
  }

  shutdown( receiverSocket, SD_SEND );
  CloseHandle( parameter.fileHandle );
  closesocket( receiverSocket );
  log_info( "[WIN32_THREAD] TCPFileTransferReceiver closing; thread id: ", parameter.threadInfo.id, ".\n" );
  return 0;
}

struct TCPFileTransferSenderParameter
{
  ThreadInfo      threadInfo;
  net::Connection connection;
  char const* data;
  s32         size;

  s32  outstandingRecipients;
  SOCKET        acceptSocket;
};
DWORD thread_TCPFileTransferSender( void* void_parameter )
{
  TCPFileTransferSenderParameter& parameter = *(TCPFileTransferSenderParameter*) void_parameter;
  char const* data    = parameter.data;
  s32 size            = parameter.size;
  SOCKET acceptSocket = parameter.acceptSocket;

  log_info( "[WIN32_THREAD] TCPFileTransferSender launching; thread id: ", parameter.threadInfo.id, ".\n" );

  if ( win32::tcp_init_socket( &acceptSocket ) )
  {
    if ( win32::bind_socket( acceptSocket, parameter.connection ) )
    {
      listen( acceptSocket, 1000 );
      for ( ;parameter.outstandingRecipients > 0; --parameter.outstandingRecipients )
      {
        SOCKET tcpSocket;
        net::Connection tcpConnection;
        if ( win32::tcp_accept( acceptSocket, &tcpConnection, &tcpSocket ) )
        {
          log_info( "WIN32_NET] TCP connection established with: ", tcpConnection );

          assert( size < GigaBytes( 1 ) );

          if ( win32::tcp_send( tcpSocket, data, size ) && win32::tcp_send( tcpSocket, nullptr, 0 ) )
          {
          }
          else
          {
            log_error( "[WIN32_NET] ERROR - TCP send failed. WSA Code: ", WSAGetLastError(), ".\n" );
          }

          closesocket( tcpSocket );
        }
        else
        {
          log_error( "[WIN32_NET] ERROR - TCP accept invalid. WSA Code: ", WSAGetLastError(), ".\n" );
        }
      }
    }
    else
    {
      log_error( "[WIN32_NET] ERROR - TCP bind socket failed. WSA Code: ", WSAGetLastError(), ".\n" );
    }

    closesocket( acceptSocket );
  }
  else
  {
    log_error( "[WIN32_NET] ERROR - init TCP socket failed. WSA Code: ", WSAGetLastError(), ".\n" );
  }

  log_info( "[WIN32_THREAD] TCPFileTransferSender closing; thread id: ", parameter.threadInfo.id, ".\n" );
  return 0;
}

void debug_SendUDPPacket( net::UDPSendParameter const& parameter )
{
  SOCKET& udpSocket = global::netData.udpSocket;
  if ( udpSocket == INVALID_SOCKET )
  {
    log_error( "[WIN32_NET] Can't send packet, socket not initialized." );
    return;
  }

  if ( parameter.packetSize <= APP_NETWORK_PACKET_SIZE_MAX )
  {
    LOCK_SCOPE( global::netData.guard_connections );
    u32 const openConnections = global::netData.connectionCount;
    for ( u32 i = 0; i < openConnections; ++i )
    {
      net::Connection const& connection = global::netData.connections[i];
      log_info( "[WIN32_NET] Sending udp packet to: ", connection );
      if ( !win32::udp_send( udpSocket, connection, parameter.packet, parameter.packetSize ) )
      {
        log_error( "[WIN32_NET] ERROR - UDP send failed. WSA Code: ", WSAGetLastError(), ".\n" );
      }
    }
  }
  else
  {
    log_info( "[WIN32_NET] ERROR - udp packet too large (", parameter.packetSize, "), not sending it." );
  }
}

void debug_SendTCPPacket( net::TCPSendParameter const& parameter )
{
  if ( *parameter.filename )
  {
    char buffer[APP_NETWORK_PACKET_SIZE_MAX];
    net::UDPSendParameter udpSendParameter = {};
    udpSendParameter.to = {};
    udpSendParameter.packet = buffer;
    udpSendParameter.packetSize = win32::serialize_tcp_write_file( buffer, parameter.filename, parameter.fileSize );

    log_info( "[WIN32_NET] Sending tcp info." );
    debug_SendUDPPacket( udpSendParameter );

    TCPFileTransferSenderParameter tcpFileTransferSenderParameter = {};
    tcpFileTransferSenderParameter.connection = { net::Connection::IPv4_ADDRESS_ANY, global::netData.self.port };
    tcpFileTransferSenderParameter.data = parameter.fileData;
    tcpFileTransferSenderParameter.size = parameter.fileSize;
    tcpFileTransferSenderParameter.outstandingRecipients = 1;

    CloseHandle( CreateThread( 0, 0, thread_TCPFileTransferSender, &tcpFileTransferSenderParameter, 0, (LPDWORD) &tcpFileTransferSenderParameter.threadInfo.id ) );
  }
}


namespace win32
{
  #if !BS_BUILD_RELEASE
  struct DllLoadingParameter
  {
    ThreadInfo threadInfo;
  };
  DWORD thread_DllLoading( void* void_parameter )
  {
    DllLoadingParameter& parameter = *(DllLoadingParameter*) void_parameter;

    log_info( "[WIN32_THREAD] DllLoading launching; thread id: ", parameter.threadInfo.id, ".\n" );
    constexpr u32 THREAD_SLEEP_DURATION = 500;
    char const* TMP_APP_CODE_FILENAME[2] = { "tmp_app_code0.dll", "tmp_app_code1.dll" };

    FILETIME lastWriteTime = {};
    App* newApp = &global::win32Data.app_instances[!global::win32Data.guard_currentDllIndex];

    while ( true )
    {
      if ( newApp == &global::win32Data.app_instances[global::win32Data.guard_currentDllIndex] && global::win32Data.guard_oldDllCanBeDiscarded )
      {
        newApp = &global::win32Data.app_instances[!global::win32Data.guard_currentDllIndex];
        if ( newApp->dll )
        {
          if ( FreeLibrary( newApp->dll ) )
          {
            log_if( DEBUG_LOG_DLL_FREE, "[WIN32_DLL] Freed ", TMP_APP_CODE_FILENAME[global::win32Data.guard_currentDllIndex], ".\n" );
          }
          else
          {
            log_if( DEBUG_LOG_DLL_FREE, "[WIN32_DLL] ERROR - couldn't free ", TMP_APP_CODE_FILENAME[global::win32Data.guard_currentDllIndex], ".\n" );
          }
        }
        newApp->dll = 0;
        global::win32Data.guard_oldDllCanBeDiscarded.decrement();
      }
      else
      {
        _WIN32_FIND_DATAA findData;
        HANDLE findHandle = FindFirstFileA( APP_FILENAME, &findData );
        if ( findHandle != INVALID_HANDLE_VALUE )
        {
          FindClose( findHandle );

          if ( CompareFileTime( &lastWriteTime, &findData.ftLastWriteTime ) )
          {
            if ( CopyFileA( APP_FILENAME, TMP_APP_CODE_FILENAME[global::win32Data.guard_currentDllIndex], false ) )//~60M cycles
            {
              log_info( "[WIN32_DLL] copied ", APP_FILENAME, " over to ", TMP_APP_CODE_FILENAME[global::win32Data.guard_currentDllIndex], ".\n" );

              newApp->dll = LoadLibraryA( TMP_APP_CODE_FILENAME[global::win32Data.guard_currentDllIndex] );//~120M cycles
              if ( newApp->dll )
              {
                newApp->sample_sound = (win32_app_sample_sound*) GetProcAddress( newApp->dll, "app_sample_sound" );
                newApp->on_load = (win32_app_on_load*) GetProcAddress( newApp->dll, "app_on_load" );
                newApp->tick = (win32_app_tick*) GetProcAddress( newApp->dll, "app_tick" );
                newApp->receive_udp_packet = (win32_app_receive_udp_packet*) GetProcAddress( newApp->dll, "app_receive_udp_packet" );

                if ( newApp->sample_sound && newApp->on_load && newApp->tick && newApp->receive_udp_packet )
                {
                  log_info( "[WIN32_DLL] Loaded ", TMP_APP_CODE_FILENAME[global::win32Data.guard_currentDllIndex], ".\n" );
                  global::win32Data.guard_currentDllIndex ? global::win32Data.guard_currentDllIndex.decrement()
                    : global::win32Data.guard_currentDllIndex.increment();

                  lastWriteTime = findData.ftLastWriteTime;
                }
                else
                {
                  log_error( "[WIN32_DLL] ERROR - couldn't fetch proc addresses in ", TMP_APP_CODE_FILENAME[global::win32Data.guard_currentDllIndex], ".\n" );
                }
              }
              else
              {
                DeleteFileA( TMP_APP_CODE_FILENAME[global::win32Data.guard_currentDllIndex] );
                log_error( "[WIN32_DLL] ERROR - couldn't load library ", TMP_APP_CODE_FILENAME[global::win32Data.guard_currentDllIndex], ".\n" );
              }
            }
            else
            {
              //log_error( "[WIN32_DLL] ERROR - couldn't copy file ", APP_FILENAME, ", retrying.\n" );
            }
          }
        }
      }

      Sleep( THREAD_SLEEP_DURATION );
    }

    log_info( "[WIN32_THREAD] DllLoading closing; thread id: ", parameter.threadInfo.id, ".\n" );
    return 0;
  }
  #endif // !BS_BUILD_RELEASE

  void ServerHandshake( SOCKET connectSocket, net::Connection serverConnection, char const* username )
  {
    log_info( "[WIN32_NET] Sending server handshake to: ", serverConnection, ".\n" );

    char appVersion[128];
    string_format( appVersion, APP_NAME, APP_VERSION );

    net::Connection nullConnection = {};
    char buffer[APP_NETWORK_PACKET_SIZE_MAX];
    s32 packetSize = win32::serialize_client_handshake( buffer, nullConnection, appVersion, username );

    if ( !win32::udp_send( connectSocket, serverConnection, buffer, packetSize ) )
    {
      log_error( "[WIN32_NET] ERROR - UDP server handshake failed. WSA Code: ", WSAGetLastError(), ".\n" );
    }
  }


  struct UDPListenerParameter
  {
    ThreadInfo    threadInfo;
    SOCKET        udpSocket;
    PlatformData* platformData;
    AppData* appData;
  };
  DWORD thread_UDPListener( void* void_parameter )
  {
    UDPListenerParameter& parameter = *(UDPListenerParameter*) void_parameter;
    log_info( "[WIN32_THREAD] UDPListener launching; thread id: ", parameter.threadInfo.id, ".\n" );
    AppData* appData = parameter.appData;
    SOCKET udpSocket = parameter.udpSocket;

    char receiveBuffer[APP_NETWORK_PACKET_SIZE_MAX + 1] = {};
    for ( ;; )
    {
      s32 bytesReceived;
      net::Connection sender;
      if ( win32::udp_receive( udpSocket, receiveBuffer, &sender, &bytesReceived ) )
      {
        win32::PlatformReservedPacketHeader packetHeader;
        if ( win32::parse_packet_header( receiveBuffer, bytesReceived, &packetHeader ) )
        {
          if ( packetHeader.flags & win32::PlatformReservedPacketHeader::PLATFORM_RESERVED )
          {
            if ( packetHeader.flags == win32::PlatformReservedPacketHeader::TCP_WRITE_FILE )
            {
              char const* filename;
              u32 filesize;
              if ( win32::parse_tcp_write_file( receiveBuffer, bytesReceived, &filename, &filesize ) )
              {
                TCPFileTransferReceiverParameter tcpParameter = {};
                tcpParameter.connection = sender; //TODO read port out from received packet!!!
                tcpParameter.fileHandle = CreateFileA( filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );

                if ( tcpParameter.fileHandle != INVALID_HANDLE_VALUE )
                {
                  log_info( "[WIN32_NET] Attempting tcp connection with ", sender, " for file download",
                          "\n- - - - - - File name: ", filename,
                          "\n- - - - - - File size: ", filesize );

                  CloseHandle( CreateThread( 0, 0, thread_TCPFileTransferReceiver, &tcpParameter, 0, (LPDWORD) &tcpParameter.threadInfo.id ) );
                }
                else
                {
                  CloseHandle( tcpParameter.fileHandle );
                }
              }
            } //if packetHeader.flags == win32::PlatformReservedPacketHeader::TCP_WRITE_FILE
            else if ( packetHeader.flags == win32::PlatformReservedPacketHeader::CLIENT_HANDSHAKE )
            {
              if ( sender == global::netData.server )
              {
                char const* appVersion;
                char const* username;
                s32 appVersionLength;
                s32 usernameLength;
                net::Connection newConnection;
                if ( win32::parse_client_handshake( receiveBuffer, bytesReceived, &newConnection, &appVersion, &appVersionLength, &username, &usernameLength ) )
                {
                  if ( appVersionLength && usernameLength )
                  {
                    OpenPeerConnection( newConnection );
                    log_info( "[WIN32_NET] P2P connection established with:",
                            "\n - - - - - - - IP: ", newConnection,
                            "\n - - - - Username: ", username );
                  }
                  else
                  {
                    global::netData.self = newConnection;
                  }
                }
                else
                {
                  log_error( "[WIN32_NET] ERROR - error code: ", WSAGetLastError() );
                  log_todo( "error packet from server." );
                }
              }
              else
              {
                log_todo( "received a client handshake packet not from the server but from another client" );
              }
            } // if packetHeader.flags == win32::PlatformReservedPacketHeader::CLIENT_HANDSHAKE
            else
            {
              log_todo( "received unhandled platform reserved packet." );
            }
          } //if packetHeader.flags & win32::PlatformReservedPacketHeader::PLATFORM_RESERVED
          else
          {
            log_info( "[WIN32_NET] Received packet from: ", sender, "\nContent: ", string { receiveBuffer, bytesReceived } );

            net::UDPReceiveParameter udpReceiveParameter = {};
            udpReceiveParameter.sender = sender;
            udpReceiveParameter.packet = receiveBuffer;
            udpReceiveParameter.packetSize = bytesReceived;
            #if BS_BUILD_RELEASE
            app_receive_udp_packet( *parameter.platformData, *appData, udpReceiveParameter );
            #else
            //NOTE: this will always execute on the new dll!
            global::win32Data.app_instances[global::win32Data.guard_currentDllIndex].receive_udp_packet( *parameter.platformData, *appData, udpReceiveParameter );
            #endif
          }
        } //parse_packet_header
        else
        {
          if ( sender == global::netData.server )
          {
            //isalive packet from server?
            log_info( "[WIN32_NET] received isalive from server: ", sender );
          }
        }
      } //udp_receive
      else if ( WSAGetLastError() == WSAECONNRESET )
      {
        if ( sender == global::netData.server )
        {
          log_error( "[WIN32_NET] ERROR - server didn't respond." );
        }
        else
        {
          ClosePeerConnection( sender );
        }
      }
      else
      {
        log_error( "[WIN32_NET] ERROR - UDP receive failed. winsock code: ", WSAGetLastError() );
      }
    }
  }

  using win32_xInputGetState = DWORD WINAPI( DWORD, XINPUT_STATE* );
  DWORD WINAPI win32_stub_xInputGetState( DWORD, XINPUT_STATE* ) { return ERROR_DEVICE_NOT_CONNECTED; }

  using win32_xInputSetState = DWORD WINAPI( DWORD, XINPUT_VIBRATION* );
  DWORD WINAPI win32_stub_xInputSetState( DWORD, XINPUT_VIBRATION* ) { return ERROR_DEVICE_NOT_CONNECTED; }

  global_variable win32_xInputGetState* dll_xInputGetState = win32_stub_xInputGetState;
  global_variable win32_xInputSetState* dll_xInputSetState = win32_stub_xInputSetState;

  void LoadXInput()
  {
    HMODULE module = LoadLibraryA( "xinput1_4.dll" );

    if ( !module )
    {
      module = LoadLibraryA( "xinput1_3.dll" );
    }

    if ( module )
    {
      dll_xInputGetState = (win32_xInputGetState*) GetProcAddress( module, "XInputGetState" );
      dll_xInputSetState = (win32_xInputSetState*) GetProcAddress( module, "XInputSetState" );
      #if DEBUG_LOG_INPUT
      OutputDebugStringA( "[WIN32_INPUT] Controller input initialized.\n" );
      #endif
    }
    else
    {
      OutputDebugStringA( "[WIN32_INPUT] ERROR - neither xinput1_4.dll or xinput1_3.dll found.\n" );
    }
  }

  INLINE void ProcessXInputDigitalButton( WORD wButtons, DWORD buttonBit, ButtonState& state, ButtonState const& oldState )
  {
    state.halfTransitionCount = (state.endedDown != oldState.endedDown) ? 1 : 0;
    state.endedDown = wButtons & buttonBit;
  }

  void ProcessControllerInput( Input& input )
  {
    constexpr s32 supportedControllers = array_size( input.controller );
    constexpr s32 maxControllerCount = min( supportedControllers, XUSER_MAX_COUNT );

    for ( DWORD iController = 0; iController < maxControllerCount; ++iController )
    {
      XINPUT_STATE controllerState;
      if ( dll_xInputGetState( iController, &controllerState ) == ERROR_SUCCESS )
      {
        //controller plugged in
        XINPUT_GAMEPAD& pad = controllerState.Gamepad;
        ControllerInput& newController = input.controller[iController];

        ProcessXInputDigitalButton( pad.wButtons, XINPUT_GAMEPAD_DPAD_UP, newController.up, newController.up );
        ProcessXInputDigitalButton( pad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN, newController.down, newController.down );
        ProcessXInputDigitalButton( pad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT, newController.left, newController.left );
        ProcessXInputDigitalButton( pad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT, newController.right, newController.right );
        ProcessXInputDigitalButton( pad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER, newController.leftShoulder, newController.leftShoulder );
        ProcessXInputDigitalButton( pad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER, newController.rightShoulder, newController.rightShoulder );

        float stickX         = float( pad.sThumbLX ) / (pad.sThumbLX < 0 ? 32768.f : 32767.f);
        float stickY         = float( pad.sThumbLY ) / (pad.sThumbLY < 0 ? 32768.f : 32767.f);

        newController.start = newController.end;
        newController.min = newController.max = newController.end = { stickX, stickY };
        //bool start         = pad.wButtons & XINPUT_GAMEPAD_START;
        //bool back          = pad.wButtons & XINPUT_GAMEPAD_BACK;
        //bool buttonA       = pad.wButtons & XINPUT_GAMEPAD_A;
        //bool buttonB       = pad.wButtons & XINPUT_GAMEPAD_B;
        //bool buttonX       = pad.wButtons & XINPUT_GAMEPAD_X;
        //bool buttonY       = pad.wButtons & XINPUT_GAMEPAD_Y;
      }
      else
      {
        //controller not available
      }
    }

    // {
    //   XINPUT_VIBRATION vibrationState { 0, 0 };
    //   dll_xInputSetState( 0, &vibrationState );
    // }
  }

  void InitDirectSound( HWND window, s32 samplesPerSecond, s32 bufferSize )
  {
    HMODULE module = LoadLibraryA( "dsound.dll" );

    if ( module )
    {
      using win32_DirectSoundCreate = HRESULT WINAPI( LPGUID lpGuid, LPDIRECTSOUND* ppDS, LPUNKNOWN  pUnkOuter );
      win32_DirectSoundCreate* directSoundCreate = (win32_DirectSoundCreate*) GetProcAddress( module, "DirectSoundCreate" );

      LPDIRECTSOUND directSound;
      if ( directSoundCreate && SUCCEEDED( directSoundCreate( 0, &directSound, 0 ) ) )
      {
        WAVEFORMATEX waveFormat = {};
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = 2;
        waveFormat.nSamplesPerSec = samplesPerSecond;
        waveFormat.wBitsPerSample = 16;
        waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
        waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
        waveFormat.cbSize = 0;

        if ( SUCCEEDED( directSound->SetCooperativeLevel( window, DSSCL_PRIORITY ) ) )
        {
          DSBUFFERDESC primaryBufferDesc = {};
          primaryBufferDesc.dwSize = sizeof( primaryBufferDesc );
          primaryBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

          LPDIRECTSOUNDBUFFER primaryBuffer;
          if ( SUCCEEDED( directSound->CreateSoundBuffer( &primaryBufferDesc, &primaryBuffer, 0 ) ) )
          {
            if ( SUCCEEDED( primaryBuffer->SetFormat( &waveFormat ) ) )
            {
              #if DEBUG_LOG_SOUND
              OutputDebugStringA( "[WIN32_SOUND] Setting primary sound buffer format.\n" );
              #endif
            }
            else
            {
              #if DEBUG_LOG_SOUND
              OutputDebugStringA( "[WIN32_SOUND] ERROR - Setting primary buffer format.\n" );
              #endif
            }
          }
          else
          {
            #if DEBUG_LOG_SOUND
            OutputDebugStringA( "[WIN32_SOUND] ERROR - Creating primary buffer.\n" );
            #endif
          }
        }
        else
        {
          #if DEBUG_LOG_SOUND
          OutputDebugStringA( "[WIN32_SOUND] ERROR - Setting cooperative level.\n" );
          #endif
        }

        DSBUFFERDESC secondaryBufferDesc = {};
        secondaryBufferDesc.dwSize = sizeof( secondaryBufferDesc );
        secondaryBufferDesc.dwFlags = 0;
        secondaryBufferDesc.dwBufferBytes = bufferSize;
        secondaryBufferDesc.lpwfxFormat = &waveFormat;

        if ( SUCCEEDED( directSound->CreateSoundBuffer( &secondaryBufferDesc, &global::win32Data.soundBuffer, 0 ) ) )
        {
          OutputDebugStringA( "[WIN32_SOUND] Creating secondary sound buffer.\n" );
        }
        else
        {
          OutputDebugStringA( "[WIN32_SOUND] ERROR - Creating secondary buffer.\n" );
        }
      }
      else
      {
        OutputDebugStringA( "[WIN32_SOUND] ERROR - Creating direct sound.\n" );
      }
    }
    else
    {
      OutputDebugStringA( "[WIN32_SOUND] ERROR - dsound.dll not found.\n" );
    }
  }

  INLINE win32_WindowDimensions GetWindowDimensions( HWND window )
  {
    RECT rect;
    GetClientRect( window, &rect );
    return win32_WindowDimensions { rect.right - rect.left, rect.bottom - rect.top };
  }

  void ClearSoundBuffer( win32_SoundOutput& soundOutput )
  {
    void* region1;
    DWORD region1Size;
    void* region2;
    DWORD region2Size;

    if ( SUCCEEDED( global::win32Data.soundBuffer->Lock( 0, soundOutput.soundBufferSize, &region1, &region1Size, &region2, &region2Size, 0 ) ) )
    {
      u8* outSample = (u8*) region1;
      for ( DWORD iByte = 0; iByte < region1Size; ++iByte )
      {
        *outSample++ = 0;
      }

      outSample = (u8*) region2;
      for ( DWORD iByte = 0; iByte < region2Size; ++iByte )
      {
        *outSample++ = 0;
      }

      global::win32Data.soundBuffer->Unlock( region1, region1Size, region2, region2Size );
    }
  }

  void FillSoundBuffer( win32_SoundOutput& soundOutput, DWORD byteToLock, DWORD bytesToWrite, SoundBuffer const& sourceBuffer )
  {
    void* region1;
    DWORD region1Size;
    void* region2;
    DWORD region2Size;

    if ( SUCCEEDED( global::win32Data.soundBuffer->Lock( byteToLock, bytesToWrite, &region1, &region1Size, &region2, &region2Size, 0 ) ) )
    {
      s16 const* sourceSample = sourceBuffer.samples;
      s16* outSample = (s16*) region1;
      DWORD const region1SampleCount = region1Size / soundOutput.bytesPerSample;
      for ( DWORD iSample = 0; iSample < region1SampleCount; ++iSample )
      {
        *outSample++ = *sourceSample++;
        *outSample++ = *sourceSample++;
        ++soundOutput.runningSampleIndex;
      }

      outSample = (s16*) region2;
      DWORD const region2SampleCount = region2Size / soundOutput.bytesPerSample;
      for ( DWORD iSample = 0; iSample < region2SampleCount; ++iSample )
      {
        *outSample++ = *sourceSample++;
        *outSample++ = *sourceSample++;
        ++soundOutput.runningSampleIndex;
      }

      global::win32Data.soundBuffer->Unlock( region1, region1Size, region2, region2Size );
    }
  }

  void ResizeDIBSection( win32_BackBuffer& buffer, s32 width, s32 height )
  {
    if ( buffer.bmpBuffer )
    {
      VirtualFree( buffer.bmpBuffer, 0, MEM_RELEASE );
    }

    buffer.bmpWidth = width;
    buffer.bmpHeight = height;
    buffer.bytesPerPixel = 4;
    buffer.pitch = buffer.bmpWidth * buffer.bytesPerPixel;

    buffer.info.bmiHeader.biSize = sizeof( buffer.info.bmiHeader );
    buffer.info.bmiHeader.biWidth = buffer.bmpWidth;
    buffer.info.bmiHeader.biHeight = -buffer.bmpHeight;
    buffer.info.bmiHeader.biPlanes = 1;
    buffer.info.bmiHeader.biBitCount = 32;
    buffer.info.bmiHeader.biCompression = BI_RGB;

    s32 const bufferSize = buffer.bytesPerPixel * buffer.bmpWidth * buffer.bmpHeight;
    buffer.bmpBuffer = VirtualAlloc( 0, bufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
  }

  INLINE void DisplayBufferInWindow( HDC deviceContext, win32_BackBuffer const& buffer, s32 width, s32 height )
  {
    StretchDIBits( deviceContext,
                   0, 0, buffer.bmpWidth, buffer.bmpHeight,
                   0, 0, buffer.bmpWidth, buffer.bmpHeight,
                   buffer.bmpBuffer, &buffer.info,
                   DIB_RGB_COLORS, SRCCOPY );
  }

  INLINE void DebugDrawVertical( Bitmap& buffer, s32 x, s32 top, s32 bottom, u32 color )
  {
    s32 width = buffer.width;
    for ( int y = top; y > bottom;--y )
    {
      buffer.pixel[x + y * width] = color;
    }
  }

  void DebugSyncDisplay( Bitmap& buffer, win32_SoundOutput const& soundBuffer,
                         win32_DebugTimeMarker* markers, DWORD markerCount, s32 currentMarker )
  {
    s32 padX = 16;
    s32 padY = 16;
    s32 bottom = padY;
    u32 lineHeight = 64;

    float c = float( buffer.width - 2 * padX ) / float( soundBuffer.soundBufferSize );

    for ( s32 i = 0; i < s32( markerCount ); ++i )
    {
      s32 top = lineHeight + padY;
      if ( i == currentMarker )
      {
        top += lineHeight;
        bottom += lineHeight;
      }

      DWORD playCursor = markers[i].outputPlayCursor;
      assert( playCursor < u32( soundBuffer.soundBufferSize ) );
      float x =  c * float( playCursor );
      s32 x32 = s32( x ) + padX;
      DebugDrawVertical( buffer, x32, top, bottom, WHITE );

      DWORD writeCursor = markers[i].outputWriteCursor;
      assert( writeCursor < u32( soundBuffer.soundBufferSize ) );
      x =  c * float( writeCursor );
      x32 = s32( x ) + padX;

      DebugDrawVertical( buffer, x32, top, bottom, GREEN );
    }
  }
}; //namespace win32

LRESULT CALLBACK win32_MainWindowCallback( HWND window, UINT message, WPARAM wParam, LPARAM lParam )
{
  LRESULT result = 0;

  switch ( message )
  {
    case WM_PAINT:
    {
      PAINTSTRUCT paint;
      HDC deviceContext = BeginPaint( window, &paint );

      win32_WindowDimensions dimensions = win32::GetWindowDimensions( window );
      win32::DisplayBufferInWindow( deviceContext, global::win32Data.backBuffer, dimensions.width, dimensions.height );

      EndPaint( window, &paint );
      break;
    }
    case WM_CLOSE:
    case WM_QUIT:
    case WM_DESTROY:
    {
      global::win32Data.running = false;
      break;
    }
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
    {
      global::win32Data.frame_flags = (global::win32Data.frame_flags & ~APP_FLAG::APP_ACTIVATED_DEACTIVATED) | (wParam ? APP_FLAG::APP_ACTIVATED : APP_FLAG::APP_DEACTIVATED);
      break;
    }
    case WM_SETCURSOR:
    {
      //TODO
      // auto hCursor = LoadCursor( NULL, IDC_ARROW );
      // if ( LOWORD( lParam ) == HTCLIENT )
      // {
      //   SetCursor( hCursor );
      //   return TRUE;
      // }
     // break;
    }
    case WM_SIZE:
    case WM_IME_SETCONTEXT:
    case WM_IME_NOTIFY:
    case WM_NCACTIVATE:
    case WM_KILLFOCUS:

    default:
    {
      result = DefWindowProc( window, message, wParam, lParam );
      break;
    }
  }

  return result;
}

HANDLE InitDebugLog( char const* filename )
{
  HANDLE result = CreateFileA( filename,
                               GENERIC_WRITE,
                               FILE_SHARE_READ,
                               0,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               0 );
  return result;
}

int CALLBACK WinMain( HINSTANCE hInstance,
                      HINSTANCE hPreVInstance,
                      LPSTR lpCmdLine,
                      s32 cmdShow )
{
  CoInitializeEx( 0, COINIT_MULTITHREADED );

  #if !BS_BUILD_RELEASE
  global::win32Data.debugLogHandle = InitDebugLog( "debug.log" );
  #endif

  {
    QueryPerformanceFrequency( (LARGE_INTEGER*) &global::win32Data.performanceCounterFrequency );
    win32::LoadXInput();
    WSADATA wsaData;
    if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 ) {
      //TODO error
      assert( 0 );
    }
  }

  global::win32Data.running = true;

  #if !BS_BUILD_RELEASE 
  {
    win32::DllLoadingParameter dllParameter = {};
    CloseHandle( CreateThread( 0, 0, win32::thread_DllLoading, &dllParameter, 0, (LPDWORD) &dllParameter.threadInfo.id ) );
  }

  u32 debug_inputTrigger           = 0;
  u64 debug_CyclesForFrame         = 0;
  u64 debug_CyclesForAppTick       = 0;
  u64 debug_CyclesForDisplay       = 0;
  u64 debug_CyclesForInput         = 0;
  u64 debug_CyclesForSoundSampling = 0;
  u64 debug_CyclesSleep            = 0;
  #endif
  #if DEBUG_DRAW_SOUND 
  DWORD dbg_iMarker = 0;
  win32_DebugTimeMarker dbg_Markers[APP_UPDATE_HZ / 2] = {};
  #endif

  WNDCLASSA windowClass = {};
  windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  windowClass.lpfnWndProc = win32_MainWindowCallback;
  windowClass.hInstance = hInstance;
  windowClass.lpszClassName = "brewing_station";
  win32::ResizeDIBSection( global::win32Data.backBuffer, WINDOW_RESOLUTION.x, WINDOW_RESOLUTION.y );

  if ( RegisterClassA( &windowClass ) )
  {
    HWND window = CreateWindowA( "brewing_station", global_debugUsername,
                                 //WS_VISIBLE,
                                 WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                 CW_USEDEFAULT, CW_USEDEFAULT,
                                 WINDOW_RESOLUTION.x, WINDOW_RESOLUTION.y,
                                 0, 0, hInstance, 0 );
    if ( window )
    {
      win32_SoundOutput soundOutput = {};
      soundOutput.samplesPerSecond = 48000;
      soundOutput.bytesPerSample = sizeof( s16 ) * 2;
      soundOutput.soundBufferSize = soundOutput.samplesPerSecond * soundOutput.bytesPerSample;
      soundOutput.runningSampleIndex = 0;
      soundOutput.writeSafetyBytes = soundOutput.soundBufferSize / (APP_UPDATE_HZ * 2); //TODO 
      win32::InitDirectSound( window, soundOutput.samplesPerSecond, soundOutput.soundBufferSize );
      win32::ClearSoundBuffer( soundOutput );
      global::win32Data.soundBuffer->Play( 0, 0, DSBPLAY_LOOPING );
      DWORD lastPlayCursor = 0;

      s16* samples = (s16*) VirtualAlloc( 0, soundOutput.soundBufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );

      PlatformData platformData = {};
      Input& input               = platformData.input;
      platformData.get_file_info = &debug_GetFileInfo;
      platformData.read_file     = &debug_ReadFile;
      platformData.write_file    = &debug_WriteFile;
      platformData.free_file     = &debug_FreeFile;
      platformData.send_udp      = &debug_SendUDPPacket;
      platformData.send_tcp      = &debug_SendTCPPacket;

      AppData appData = {};
      appData.staticBufferSize = APP_STATIC_BUFFER_SIZE;
      appData.staticBuffer = VirtualAlloc( 0, appData.staticBufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
      appData.tempBufferSize = APP_TEMP_BUFFER_SIZE;
      appData.tempBuffer = VirtualAlloc( 0, appData.tempBufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );

      {
        SOCKET udpSocket;
        if ( win32::udp_init_socket( &udpSocket )
          && win32::bind_socket( udpSocket, { net::Connection::IPv4_ADDRESS_ANY, net::Connection::PORT_ANY } ) )
        {
          global::netData.udpSocket = udpSocket;
          global::netData.server.port = APP_SERVER_PORT;
          global::netData.server.ipv4_address = parse_ipv4( APP_SERVER_IPv4_ADDRESS );

          s32 packetSize = APP_NETWORK_PACKET_SIZE_MAX;
          setsockopt( udpSocket, SOL_SOCKET, SO_SNDBUF, (char*) &packetSize, sizeof( packetSize ) );
          setsockopt( udpSocket, SOL_SOCKET, SO_RCVBUF, (char*) &packetSize, sizeof( packetSize ) );
          u32 keepAlive = true;
          setsockopt( udpSocket, SOL_SOCKET, SO_KEEPALIVE, (char*) &keepAlive, sizeof( keepAlive ) );

          win32::ServerHandshake( udpSocket, global::netData.server, global_debugUsername );

          win32::UDPListenerParameter udpListenerParameter = {};
          udpListenerParameter.platformData = &platformData;
          udpListenerParameter.appData = &appData;
          udpListenerParameter.udpSocket = udpSocket;
          CloseHandle( CreateThread( 0, 0, win32::thread_UDPListener, &udpListenerParameter, 0, (LPDWORD) &udpListenerParameter.threadInfo.id ) );
        }
        else
        {
          log_error( "[WIN32_NET] ERROR Socket bind failed. WSA Code: ", WSAGetLastError(),
                   "\n- - - - - - Launching without networking.\n" );
        }
      }

      u32 sleepAllowed = (timeBeginPeriod( 1 ) == TIMERR_NOERROR);
      float sleepMsSubtraction = 0.f;

      LARGE_INTEGER beginCounter = win32::GetTimer();
      while ( global::win32Data.running )
      {
        {
          PROFILE_SCOPE( debug_CyclesForFrame );
          {
            PROFILE_SCOPE( debug_CyclesForInput );
            memset( input.down, 0, Input::STATE_COUNT );
            MSG message;
            while ( PeekMessage( &message, 0, 0, 0, PM_REMOVE ) )
            {
              switch ( message.message )
              {
                case WM_XBUTTONDOWN:
                case WM_XBUTTONUP:
                {
                  assert( message.wParam == 65568 || message.wParam == 131136 );
                  u8 code = message.wParam == 65568 ? Input::MOUSE_4 : Input::MOUSE_5;
                  u8 isDown = (u8) !(message.lParam & (1 << 31));
                  u8 wasDown = input.held[code];
                  input.down[code] = ((!wasDown) && isDown);
                  input.held[code] = isDown;
                  break;
                }
                case WM_MOUSEWHEEL:
                {
                  //TODO
                  break;
                }
                case WM_MOUSEMOVE:
                {
                  input.mousePos[0].start = input.mousePos[0].end;
                  input.mousePos[0].min
                    = input.mousePos[0].max
                    = input.mousePos[0].end
                    = { s32( s16( LOWORD( message.lParam ) ) ), s32( s16( HIWORD( message.lParam ) ) ) };
                  break;
                }
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                case WM_KEYDOWN:
                case WM_KEYUP:
                {
                  assert( message.wParam < Input::STATE_COUNT );
                  u64 code = message.wParam;

                  u8 isDown = (u8) !(message.lParam & (1 << 31));
                  u8 wasDown = input.held[code];
                  input.held[code] = isDown;

                  if ( !wasDown && isDown )
                  {
                    switch ( code )
                    {
                      case Input::KEY_F9:
                        #if !BS_BUILD_RELEASE
                        debug_inputTrigger = !debug_inputTrigger;
                        #endif
                        break;
                      case Input::KEY_F10:
                        break;
                      case Input::KEY_F11:
                        break;
                      case Input::KEY_F12:
                        win32::ServerHandshake( global::netData.udpSocket, global::netData.server, global_debugUsername );
                        break;
                      case Input::KEY_ESCAPE:
                        global::win32Data.running = false;
                        break;
                      default:
                        input.down[code] = 1;
                        break;
                    }
                  }

                  if ( input.held[Input::KEY_ALT] && input.down[Input::KEY_F4] ) global::win32Data.running = false;

                  break;
                }
                default:
                {
                  TranslateMessage( &message );
                  DispatchMessage( &message );
                }
                break;
              }
            }

            win32::ProcessControllerInput( input );
          } // PROFILE_SCOPE( debug_CyclesForInput );

          {
            PROFILE_SCOPE( debug_CyclesForAppTick );
            BackBuffer backBuffer = {};
            {
              backBuffer.data = global::win32Data.backBuffer.bmpBuffer;
              backBuffer.width = global::win32Data.backBuffer.bmpWidth;
              backBuffer.height = global::win32Data.backBuffer.bmpHeight;
              backBuffer.pitch = global::win32Data.backBuffer.pitch;
              backBuffer.bytesPerPixel = global::win32Data.backBuffer.bytesPerPixel;
            }

            #if BS_BUILD_RELEASE
            app_tick( platformData, appData, backBuffer );
            #else
            global::win32Data.app->tick( platformData, appData, backBuffer );
            #endif
          } // PROFILE_SCOPE( debug_CyclesForAppTick );

          {
            PROFILE_SCOPE( debug_CyclesForSoundSampling );
            u32 soundIsValid = true;

            DWORD playCursor  = 0;
            DWORD writeCursor = 0;
            if ( SUCCEEDED( global::win32Data.soundBuffer->GetCurrentPosition( &playCursor, &writeCursor ) ) )
            {
              lastPlayCursor = playCursor;
              if ( !soundIsValid )
              {
                soundOutput.runningSampleIndex = writeCursor / soundOutput.bytesPerSample;
                soundIsValid = true;
              }

              DWORD byteToLock = (soundOutput.runningSampleIndex * soundOutput.bytesPerSample) % soundOutput.soundBufferSize;

              DWORD expectedSoundBytesPerFrame = (soundOutput.samplesPerSecond * soundOutput.bytesPerSample) / APP_UPDATE_HZ;
              DWORD expectedFrameBoundaryByte = playCursor + expectedSoundBytesPerFrame;

              DWORD safeWriteCursor = writeCursor;
              if ( safeWriteCursor < playCursor )
              {
                safeWriteCursor += soundOutput.soundBufferSize;
              }
              assert( safeWriteCursor >= playCursor );
              safeWriteCursor +=soundOutput.writeSafetyBytes;

              u32 audioCardIsLowLatency = (safeWriteCursor < expectedFrameBoundaryByte);
              DWORD targetCursor = 0;
              if ( audioCardIsLowLatency )
              {
                targetCursor = expectedFrameBoundaryByte + expectedSoundBytesPerFrame % soundOutput.soundBufferSize;
              }
              else
              {
                targetCursor = (writeCursor + expectedSoundBytesPerFrame + soundOutput.writeSafetyBytes) % soundOutput.soundBufferSize;
              }

              DWORD bytesToWrite = targetCursor - byteToLock + (byteToLock > targetCursor) * soundOutput.soundBufferSize;

              SoundBuffer soundBuffer = {};
              soundBuffer.samplesPerSecond = soundOutput.samplesPerSecond;
              soundBuffer.sampleCount = bytesToWrite / soundOutput.bytesPerSample;
              soundBuffer.samples = samples;

              #if BS_BUILD_RELEASE
              app_sample_sound( appData, soundBuffer );
              #else
              global::win32Data.app->sample_sound( appData, soundBuffer );
              #endif

              #if DEBUG_DRAW_SOUND
              win32_DebugTimeMarker& marker = dbg_Markers[dbg_iMarker];
              marker.outputPlayCursor = playCursor;
              marker.outputWriteCursor = writeCursor;
              marker.outputLocation = byteToLock;
              marker.outputByteCount = bytesToWrite;
              #endif

              log_if( DEBUG_LOG_SOUND, "[WIN32_SOUND] BTL:", (u32) byteToLock,
                                                    ", TC:", (u32) targetCursor,
                                                   ", BTW:", (u32) bytesToWrite,
                                                    ", PC:", (u32) playCursor,
                                                    ", WC:", (u32) safeWriteCursor, "\n" );
              win32::FillSoundBuffer( soundOutput, byteToLock, bytesToWrite, soundBuffer );
            }
            else
            {
              soundIsValid = false;
            }
          } // PROFILE_SCOPE( debug_CyclesForSoundSampling );

          #if DEBUG_DRAW_SOUND
          {
            BitmapBuffer debug_buffer = {};
            debug_buffer.data = (u32*) global::win32Data.backBuffer.bmpBuffer;
            debug_buffer.width = global::win32Data.backBuffer.bmpWidth;
            debug_buffer.height = global::win32Data.backBuffer.bmpHeight;
            win32::DebugSyncDisplay( debug_buffer, soundOutput, dbg_Markers, array_size( dbg_Markers ), dbg_iMarker - 1 );
          }
          #endif

          {
            PROFILE_SCOPE( debug_CyclesForDisplay );
            HDC deviceContext = GetDC( window );
            win32_WindowDimensions dimensions = win32::GetWindowDimensions( window );
            win32::DisplayBufferInWindow( deviceContext, global::win32Data.backBuffer, dimensions.width, dimensions.height );
          }

          //fixed timestamp
          LARGE_INTEGER checkTimer = win32::GetTimer();
          float secondsElapsed = win32::GetSecondsElapsed( beginCounter, checkTimer );
          {
            PROFILE_SCOPE( debug_CyclesSleep );
            if ( secondsElapsed < APP_TARGET_SPF )
            {
              if ( sleepAllowed
              #if !BS_BUILD_RELEASE
                && !debug_inputTrigger
              #endif
              )
              {
                float const msSleep = ((APP_TARGET_SPF - secondsElapsed) * 1000.f) + sleepMsSubtraction;
                Sleep( s32( max( msSleep, 0.0f ) ) );
                float secondsElapsedIncludingSleep =  win32::GetSecondsElapsed( beginCounter, win32::GetTimer() );
                float const msTarget = 1000.f * APP_TARGET_SPF;
                float const delta = msTarget - 1000.0f * secondsElapsedIncludingSleep;
                sleepMsSubtraction += min( 0.f, delta ) - (delta > 2.0f) * 1.0f;

                log_if( DEBUG_LOG_CLOCK_SLEEP_DELTA, "[WIN32_CLOCK] frame ", appData.currentFrameIndex,
                                                     " had ", delta, " ms left after sleeping for ", max( msSleep, 0.f ),
                                                     " ms\n - - - next sleep reduced by ", -sleepMsSubtraction, " ms\n" );
                do
                {
                  secondsElapsedIncludingSleep = win32::GetSecondsElapsed( beginCounter, win32::GetTimer() );
                } while ( secondsElapsedIncludingSleep < APP_TARGET_SPF );
              }
            }
            else
            {
              log_if( DEBUG_LOG_CLOCK_MISSED_FRAME, "[WIN32_CLOCK] Missed fps target for frame: ", appData.currentFrameIndex,
                                                    "\n- - - - - - - Actual ms: ", 1000.f * secondsElapsed,
                                                    "   fps: ", float( 1.f / secondsElapsed ), "\n" );
            }
          } // PROFILE_SCOPE( debug_CyclesSleep );
        } // PROFILE_SCOPE( debug_CyclesForFrame );
        LARGE_INTEGER endCounter = win32::GetTimer();

        #if DEBUG_LOG_CLOCK_FPS
        {
          float ms = 1000.f * win32::GetSecondsElapsed( beginCounter, endCounter );
          float fps = 1000.f / ms;
          log_info( "[WIN32_CLOCK] ms: ", ms,
                    "  fps: ", FloatFormat( fps, 4 ),
                    "  Mcpf: ", FloatFormat( float( debug_CyclesForFrame ) / 1000000.f, 8 ), "\n" );
        }
        #endif

        log_if( DEBUG_LOG_CLOCK_CYCLES, "[WIN32_CLOCK] Total: ", debug_CyclesForFrame,
                                        " - - Frame: ", appData.currentFrameIndex,
                                        "\n - - - - - App tick: ", debug_CyclesForAppTick,
                                        "\n- - - - - - - Sleep: ", debug_CyclesSleep, "\n" );

        #if !BS_BUILD_RELEASE
        if ( global::win32Data.app != &global::win32Data.app_instances[global::win32Data.guard_currentDllIndex] )
        {
          assert( !global::win32Data.guard_oldDllCanBeDiscarded );
          //discard before switching app, p2p listener depends on it
          global::win32Data.guard_oldDllCanBeDiscarded.increment();
          global::win32Data.app = &global::win32Data.app_instances[global::win32Data.guard_currentDllIndex];

          global::win32Data.app->on_load( appData );
        }
        #endif
        beginCounter = endCounter;
        ++appData.currentFrameIndex;

        if ( global::win32Data.frame_flags & APP_FLAG::APP_ACTIVATED_DEACTIVATED )
        {
          #if DEBUG_LOG_APP_ACTIVATED_DEACTIVATED
          ( global::win32Data.frame_flags & APP_FLAG::APP_ACTIVATED ) ? log_info( "[WIN32_APP] Activated.\n" ) : log_info( "[WIN32_APP] Deactivated.\n" );
          #endif
          memset( input.held, 0, Input::STATE_COUNT );
          global::win32Data.frame_flags &= ~APP_FLAG::APP_ACTIVATED_DEACTIVATED;
        }

        /////////////////////////////////////////////////////////////////////
        ///////////////////////// END OF FRAME //////////////////////////////
        /////////////////////////////////////////////////////////////////////

        #if DEBUG_DRAW_SOUND
        {
          DWORD dbg_PlayCursor  = 0;
          DWORD dbg_WriteCursor = 0;
          global::win32Data.soundBuffer->GetCurrentPosition( &dbg_PlayCursor, &dbg_WriteCursor );

          assert( dbg_iMarker < array_size( dbg_Markers ) );

          dbg_Markers[dbg_iMarker].eofPlayCursor  = dbg_PlayCursor;
          dbg_Markers[dbg_iMarker].eofWriteCursor = dbg_WriteCursor;

          if ( ++dbg_iMarker == array_size( dbg_Markers ) )
          {
            dbg_iMarker = 0;
          }
        }
        #endif
      }
    }
    else
    {
      assert( 0 ); // TODO: window creation failed ERROR
    }
  }

  WSACleanup();
  CoUninitialize();
  return 0;
}