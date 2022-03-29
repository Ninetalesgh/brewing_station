#pragma once
#include "../net.h"
#include "../common/basic_types.h"

#include <winsock2.h>
#ifdef min
# undef min
#endif
#ifdef max
# undef max
#endif

namespace win32
{
  struct PlatformReservedPacketHeader
  {
    u32 flags;

    enum : u32
    {
      PLATFORM_RESERVED = 0x80000000,
      TCP_WRITE_FILE    = 0xc0000000,
      CLIENT_HANDSHAKE  = 0x80000001,
      CLIENT_DISCONNECT = 0x80000002,
    };
  };

  u32 tcp_init_socket( SOCKET* out_socket );
  u32 udp_init_socket( SOCKET* out_socket );
  u32 bind_socket( SOCKET sock, net::Connection incomingConnection );

  u32 tcp_connect( SOCKET sock, net::Connection connection );
  u32 tcp_accept( SOCKET sock, net::Connection* out_remoteConnection, SOCKET* out_tcpSocket );

  u32 tcp_send( SOCKET sock, char const* data, s32 size );
  u32 tcp_receive( SOCKET sock, char* receiveBuffer, s32 receiveBufferSize, s32* out_bytesReceived );

  u32 udp_send( SOCKET sock, net::Connection address, char const* packet, s32 packetSize );
  u32 udp_receive( SOCKET sock, char* receiveBuffer, net::Connection* out_remoteConnection, s32* out_bytesReceived );

  s32 parse_packet_header( char const* packet, s32 packetSize, win32::PlatformReservedPacketHeader* out_header );

  u32 parse_client_handshake( char const* packet, s32 packetSize,
                              net::Connection* out_connection,
                              char const** out_appVersion,
                              s32* out_appVersionLength,
                              char const** out_username,
                              s32* out_usernameLength );
  s32 serialize_client_handshake( char* to, net::Connection const& connection, char const* appVersion, char const* username );

  s32 parse_tcp_write_file( char const* packet, s32 packetSize, char const** out_filename, u32* out_filesize );
  s32 serialize_tcp_write_file( char* to, char const* filename, u32 filesize );
};