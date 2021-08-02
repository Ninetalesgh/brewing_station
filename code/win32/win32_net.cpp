#include "win32_net.h"
//#include "../platform.h"


namespace win32
{
  constexpr u32 APP_NETWORK_PACKET_SIZE_MAX = 1024;
  struct ClientHandshakePacketHeader
  {
    u16 appVersionLength;
    u16 usernameLength;
    //data following is <app><version><username>
  };

  struct TCPWriteFilePacketHeader
  {
    u32 filesize;
    u16 filenameLength;
    //data following is <filename>
  };

  u32 tcp_init_socket( SOCKET* out_socket )
  {
    *out_socket = socket( AF_INET, SOCK_STREAM, 0 );
    return *out_socket != INVALID_SOCKET;
  }

  u32 udp_init_socket( SOCKET* out_socket )
  {
    *out_socket = socket( AF_INET, SOCK_DGRAM, 0 );
    return *out_socket != INVALID_SOCKET;
  }

  u32 bind_socket( SOCKET sock, net::Connection incomingConnection )
  {
    SOCKADDR_IN address;
    address.sin_port = htons( incomingConnection.port );
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = incomingConnection.ipv4_address;

    return bind( sock, (LPSOCKADDR) &address, sizeof( address ) ) != SOCKET_ERROR;
  }

  u32 tcp_connect( SOCKET sock, net::Connection connection )
  {
    SOCKADDR_IN address;
    address.sin_port = htons( connection.port );
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = connection.ipv4_address;
    return connect( sock, (sockaddr*) &address, sizeof( address ) ) != SOCKET_ERROR;
  }

  u32 tcp_accept( SOCKET sock, net::Connection* out_remoteConnection, SOCKET* out_tcpSocket )
  {
    sockaddr_in remoteAddr;
    s32 remoteAddrSize = sizeof( remoteAddr );
    *out_tcpSocket = accept( sock, (sockaddr*) &remoteAddr, &remoteAddrSize );

    if ( *out_tcpSocket != INVALID_SOCKET )
    {
      out_remoteConnection->ipv4_address = remoteAddr.sin_addr.s_addr;
      out_remoteConnection->port = ntohs( remoteAddr.sin_port );
      return 1;
    }
    else
    {
      out_remoteConnection->ipv4_address = net::Connection::IPv4_ADDRESS_INVALID;
      out_remoteConnection->port = net::Connection::PORT_INVALID;
      return 0;
    }
  }

  u32 tcp_send( SOCKET sock, char const* data, s32 size )
  {
    return send( sock, data, size, 0 ) != SOCKET_ERROR;
  }

  u32 tcp_receive( SOCKET sock, char* receiveBuffer, s32 receiveBufferSize, s32* out_bytesReceived )
  {
    *out_bytesReceived = recv( sock, receiveBuffer, receiveBufferSize, 0 );
    return *out_bytesReceived != SOCKET_ERROR;
  }

  u32 udp_send( SOCKET sock, net::Connection address, char const* packet, s32 packetSize )
  {
    SOCKADDR_IN receiver = {};
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons( address.port );
    receiver.sin_addr.s_addr = address.ipv4_address;

    return sendto( sock, packet, packetSize, 0, (sockaddr*) &receiver, sizeof( receiver ) ) != SOCKET_ERROR;
  }

  u32 udp_receive( SOCKET sock, char* receiveBuffer, net::Connection* out_remoteConnection, s32* out_bytesReceived )
  {
    SOCKADDR_IN remoteAddr;
    s32	remoteAddrLen = sizeof( remoteAddr );
    *out_bytesReceived = recvfrom( sock, receiveBuffer, APP_NETWORK_PACKET_SIZE_MAX, 0, (sockaddr*) &remoteAddr, &remoteAddrLen );
    out_remoteConnection->ipv4_address = remoteAddr.sin_addr.s_addr;
    out_remoteConnection->port = ntohs( remoteAddr.sin_port );

    return *out_bytesReceived > SOCKET_ERROR;
  }

  s32 parse_packet_header( char const* packet, s32 packetSize, win32::PlatformReservedPacketHeader* out_header )
  {
    if ( packetSize >= sizeof( win32::PlatformReservedPacketHeader ) )
    {
      *out_header = *(win32::PlatformReservedPacketHeader*) packet;
      return 1;
    }

    return 0;
  }

  u32 parse_client_handshake( char const* packet, s32 packetSize,
                              net::Connection* out_connection,
                              char const** out_appVersion,
                              s32* out_appVersionLength,
                              char const** out_username,
                              s32* out_usernameLength )
  {
    s32 expectedSize = sizeof( win32::PlatformReservedPacketHeader ) + sizeof( win32::ClientHandshakePacketHeader ) + sizeof( net::Connection );

    win32::PlatformReservedPacketHeader& packetHeader = *(win32::PlatformReservedPacketHeader*) packet;
    packet += sizeof( win32::PlatformReservedPacketHeader );

    if ( packetSize >= expectedSize && packetHeader.flags == win32::PlatformReservedPacketHeader::CLIENT_HANDSHAKE )
    {
      *out_connection = *(net::Connection*) packet;
      packet += sizeof( net::Connection );

      win32::ClientHandshakePacketHeader& clientHeader = *(win32::ClientHandshakePacketHeader*) packet;
      packet += sizeof( win32::ClientHandshakePacketHeader );

      expectedSize += clientHeader.appVersionLength + clientHeader.usernameLength;
      if ( packetSize == expectedSize )
      {
        *out_appVersionLength = clientHeader.appVersionLength;
        *out_usernameLength = clientHeader.usernameLength;
        *out_appVersion = packet;
        *out_username = *out_appVersion + *out_appVersionLength;
        return 1;
      }
    }

    *out_appVersionLength = -1;
    *out_usernameLength = -1;
    *out_appVersion = nullptr;
    *out_username = nullptr;
    return 0;
  }

  s32 serialize_client_handshake( char* to, net::Connection const& connection, char const* appVersion, char const* username )
  {
    char* const begin = to;
    win32::PlatformReservedPacketHeader& header = *(win32::PlatformReservedPacketHeader*) to;
    header.flags = win32::PlatformReservedPacketHeader::CLIENT_HANDSHAKE;
    to += sizeof( win32::PlatformReservedPacketHeader );

    *(net::Connection*) to = connection;
    to += sizeof( net::Connection );

    win32::ClientHandshakePacketHeader& clientHeader = *(win32::ClientHandshakePacketHeader*) to;
    to += sizeof( win32::ClientHandshakePacketHeader );

    if ( appVersion )
    {
      clientHeader.appVersionLength = (u16) string_format( to, appVersion );
      to += clientHeader.appVersionLength;
    }
    else
    {
      clientHeader.appVersionLength = 0;
    }

    if ( username )
    {
      clientHeader.usernameLength = (u16) string_format( to, username );
      to += clientHeader.usernameLength;
    }
    else
    {
      clientHeader.usernameLength = 0;
    }

    return s32( to - begin );
  }

  s32 parse_tcp_write_file( char const* packet, s32 packetSize, char const** out_filename, u32* out_filesize )
  {
    s32 expectedSize = sizeof( win32::PlatformReservedPacketHeader ) + sizeof( win32::TCPWriteFilePacketHeader );
    win32::PlatformReservedPacketHeader& packetHeader = *(win32::PlatformReservedPacketHeader*) packet;
    packet += sizeof( win32::PlatformReservedPacketHeader );
    if ( packetSize >= expectedSize && packetHeader.flags == win32::PlatformReservedPacketHeader::TCP_WRITE_FILE )
    {
      win32::TCPWriteFilePacketHeader& tcpWriteFilePacketHeader = *(win32::TCPWriteFilePacketHeader*) packet;
      packet += sizeof( win32::TCPWriteFilePacketHeader );
      expectedSize += tcpWriteFilePacketHeader.filenameLength;
      if ( packetSize == expectedSize )
      {
        *out_filename = packet;
        *out_filesize = tcpWriteFilePacketHeader.filesize;
        return 1;
      }
    }

    return 0;
  }

  s32 serialize_tcp_write_file( char* to, char const* filename, u32 filesize )
  {
    char* const begin = to;
    win32::PlatformReservedPacketHeader& header = *(win32::PlatformReservedPacketHeader*) to;
    to += sizeof( win32::PlatformReservedPacketHeader );

    header.flags = win32::PlatformReservedPacketHeader::TCP_WRITE_FILE;

    win32::TCPWriteFilePacketHeader& tcpWriteFilePacketHeader = *(win32::TCPWriteFilePacketHeader*) to;
    to += sizeof( win32::TCPWriteFilePacketHeader );

    tcpWriteFilePacketHeader.filesize = filesize;
    tcpWriteFilePacketHeader.filenameLength = (u16) string_format( to, filename );

    to += tcpWriteFilePacketHeader.filenameLength;

    return s32( to - begin );
  }

};