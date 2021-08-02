#include "win32/win32_net.cpp"

#include "platform.h"
#include "common/profile.h"

#include "common/hash.h"
#include "common/string.h"

#include <windows.h>
#include <iostream>

#pragma comment(lib,"ws2_32.lib")

struct Client
{
  Client() {}
  Client( Client const& other ) : connection( other.connection ), id( other.id ) {}
  net::Connection connection;
  hash32 id;
};

global_variable Client global_clients[APP_CLIENT_COUNT_MAX];
global_variable s32 global_clientCount;

int main()
{
  char stringBuffer[1024] = {};
  string_format( stringBuffer, APP_NAME, "_", APP_VERSION, " server" );
  SetConsoleTitleA( stringBuffer );

  WSADATA wsaData;
  if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 )
  {
    printf( " WSA couldn't start.\n" );
    return 0;
  }

  SOCKET serverSocket;
  if ( win32::udp_init_socket( &serverSocket )
    && win32::bind_socket( serverSocket, { net::Connection::IPv4_ADDRESS_ANY, APP_SERVER_PORT } ) )
  {

  }
  else
  {
    printf( "Socket bind failed.\n" );
    return 0;
  }

  {
    int setsockoptparameter = APP_NETWORK_PACKET_SIZE_MAX;
    setsockopt( serverSocket, SOL_SOCKET, SO_SNDBUF, (char*) &setsockoptparameter, sizeof( setsockoptparameter ) );
    setsockopt( serverSocket, SOL_SOCKET, SO_RCVBUF, (char*) &setsockoptparameter, sizeof( setsockoptparameter ) );
    u32 keepAlive = true;
    setsockopt( serverSocket, SOL_SOCKET, SO_KEEPALIVE, (char*) &keepAlive, sizeof( keepAlive ) );
    listen( serverSocket, 1000 );
  }

  printf( "[WIN32_SERVER] Waiting for connections.\n" );

  while ( true )
  {
    char receiveBuffer[APP_NETWORK_PACKET_SIZE_MAX];
    s32 bytesReceived;
    net::Connection sender;
    if ( win32::udp_receive( serverSocket, receiveBuffer, &sender, &bytesReceived ) )
    {
      win32::PlatformReservedPacketHeader packetHeader;
      if ( win32::parse_packet_header( receiveBuffer, bytesReceived, &packetHeader ) )
      {
        if ( packetHeader.flags & win32::PlatformReservedPacketHeader::CLIENT_HANDSHAKE )
        {
          char const* appVersion;
          char const* username;
          s32 appVersionLength;
          s32 usernameLength;
          net::Connection connection;
          if ( !win32::parse_client_handshake( receiveBuffer, bytesReceived, &connection, &appVersion, &appVersionLength, &username, &usernameLength ) )
          {
            printf( "client handshake/header packet couldn't be parsed" );
            continue;
          }

          Client newClient {};
          newClient.connection = sender;
          newClient.id = hash32 { username };

          for ( s32 i = 0; i < global_clientCount; ++i )
          {
            Client& existingConnection = global_clients[i];
            if ( existingConnection.id == newClient.id )
            {
              existingConnection = global_clients[--global_clientCount];
              break;
            }
          }
          global_clients[global_clientCount++] = newClient;

          string_format( stringBuffer, "[WIN32_SERVER] Client connected.",
                                    "\n - - - - - - - IP: ", newClient.connection,
                                    "\n - - - - Username: ", username,
                                    "\n - - - AppVersion: ", appVersion,
                                    "\n - - - - - - - - - - - - - - - - - - - - - -\n" );
          printf( stringBuffer );

          char sendPacket[APP_NETWORK_PACKET_SIZE_MAX];

          s32 packetSize = win32::serialize_client_handshake( sendPacket, newClient.connection, nullptr, nullptr );
          if ( win32::udp_send( serverSocket, newClient.connection, sendPacket, packetSize ) )
          {
            for ( s32 i = 0; i < (global_clientCount - 1); ++i )
            {
              //TODO
              char const namePlaceholder[] = "USERNAME";
              packetSize = win32::serialize_client_handshake( sendPacket, global_clients[i].connection, appVersion, namePlaceholder );
              if ( win32::udp_send( serverSocket, newClient.connection, sendPacket, packetSize ) )
              {
                packetSize = win32::serialize_client_handshake( sendPacket, newClient.connection, appVersion, username );
                if ( win32::udp_send( serverSocket, global_clients[i].connection, sendPacket, packetSize ) )
                {
                  string_format( stringBuffer, "[WIN32_SERVER] Forwarded hole punch for:",
                                             "\n - - - - - - - Address: ", newClient.connection,
                                             "\n - - - - - - - Address: ", global_clients[i].connection,
                                             "\n - - - - - - - - - - - - - - - - - - - - - -\n" );
                  printf( stringBuffer );
                  continue;
                }
              }
              string_format( stringBuffer, "[WIN32_SERVER] Failed to send hole punch, attempted to connect:",
                                         "\n - - - - - - - Address: ", newClient.connection,
                                         "\n - - - - - - - Address: ", global_clients[i].connection,
                                         "\n - - - - - - - - - - - - - - - - - - - - - -\n" );
              printf( stringBuffer );
            }
          }

          string_format( stringBuffer, "[WIN32_SERVER] ", APP_NAME, APP_VERSION, " now has ", global_clientCount, " connected clients.",
                                    "\n - - - - - - - - - - - - - - - - - - - - - -\n" );
          printf( stringBuffer );
        } // if header.flags & win32::PlatformReservedPacketHeader::CLIENT_HANDSHAKE
        else
        {
          string_format( stringBuffer, "[WIN32_SERVER] Packet error from connection:",
                                     "\n - - - - - - - - - - - - - - - - - - - - - -",
                                     "\n - - ERROR - - IP: ", sender,
                                     "\n - - - - - - - - - - - - - - - - - - - - - -\n" );
          printf( stringBuffer );
        }
      }
    }
    Sleep( 1 );
  }

  closesocket( serverSocket );
  WSACleanup();
  return 0;
}
