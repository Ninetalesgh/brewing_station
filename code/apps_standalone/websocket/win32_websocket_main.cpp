

#include "sha1.h"

#include <core/internal/bsnet.cpp>
#include <common/bsstring.h>

#include <stdio.h>
#include <winsock2.h>

//winsock
#pragma comment(lib,"ws2_32.lib")

constexpr u32 MAX_DEBUG_MESSAGE_LENGTH = 512;
template<typename... Args> void log( Args... args )
{
  char debugBuffer[MAX_DEBUG_MESSAGE_LENGTH];
  s32 bytesToWrite = bs::string::format( debugBuffer, MAX_DEBUG_MESSAGE_LENGTH, args... ) - 1 /* ommit null */;
  if ( bytesToWrite > 0 )
  {
    if ( debugBuffer[bytesToWrite - 1] != '\n' )
    {
      debugBuffer[bytesToWrite++] = '\n';
      debugBuffer[bytesToWrite] = '\0';
    }

    printf( debugBuffer );
  }
}

u32 tcp_init_socket( SOCKET* out_socket )
{
  *out_socket = socket( AF_INET, SOCK_STREAM, 0 );
  return *out_socket != INVALID_SOCKET;
}

u32 bind_socket( SOCKET sock, bs::net::Connection incomingConnection )
{
  SOCKADDR_IN address;
  address.sin_port = htons( incomingConnection.port );
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = incomingConnection.ipv4_address;

  return bind( sock, (LPSOCKADDR) &address, sizeof( address ) ) != SOCKET_ERROR;
}

u32 tcp_accept( SOCKET sock, bs::net::Connection* out_remoteConnection, SOCKET* out_tcpSocket )
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
    out_remoteConnection->ipv4_address = bs::net::IPv4_ADDRESS_INVALID;
    out_remoteConnection->port = bs::net::PORT_INVALID;
    return 0;
  }
}

u32 tcp_receive( SOCKET sock, char* receiveBuffer, s32 receiveBufferSize, s32* out_bytesReceived )
{
  *out_bytesReceived = recv( sock, receiveBuffer, receiveBufferSize, 0 );
  return *out_bytesReceived != SOCKET_ERROR;
}



int main()
{
  using namespace bs;
  SOCKET acceptSocket;

  bs::net::Connection connection = { bs::net::IPv4_ADDRESS_ANY, 80 };

  sha1( "abc", 0, nullptr, 0 );


  WSADATA wsaData;
  if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 )
  {
    printf( " WSA couldn't start.\n" );
    return 0;
  }

  if ( tcp_init_socket( &acceptSocket ) )
  {
    log( "init\n" );

    if ( bind_socket( acceptSocket, connection ) )
    {
      log( "bound to: ", connection, "\n" );
      listen( acceptSocket, 3000 );

      SOCKET tcpSocket;
      bs::net::Connection tcpConnection;
      if ( tcp_accept( acceptSocket, &tcpConnection, &tcpSocket ) )
      {
        log( "connected with: ", tcpConnection, "\n" );

        constexpr s32 bufferSize = (s32) KiloBytes( 16 );
        char receiveBuffer[bufferSize];
        s32 bytesReceived;
        if ( tcp_receive( tcpSocket, receiveBuffer, bufferSize, &bytesReceived ) )
        {
          log( receiveBuffer );

          if ( string::contains( receiveBuffer, "GET" ) == receiveBuffer )
          {
            char const* key = nullptr;
            char const* keyText = "Sec-WebSocket-Key: ";
            key = string::contains( receiveBuffer, keyText );
            if ( key )
            {
              char const* magic_key = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
              key += string::length( keyText );


            }
          }
          else
          {
            log( "not a websock request" );
          }
        }


      }
    }
  }
  return 0;
}








