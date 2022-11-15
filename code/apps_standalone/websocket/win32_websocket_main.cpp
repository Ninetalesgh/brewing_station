
#include "bswebsocket.h"

#include <stdio.h>

int main()
{
  using namespace bs;
  SOCKET acceptSocket;

  bs::net::Connection connection = { bs::net::IPv4_ADDRESS_ANY, 80 };

  char const* separatorLineThick = "=====================================================================\n";
  char const* separatorLineThin  = "---------------------------------------------------------------------\n";

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

      while ( true )
      {
        log( "Waiting for Connection ...\n" );

        if ( tcp_accept( acceptSocket, &tcpConnection, &tcpSocket ) )
        {
          log( separatorLineThin, "Connected with: ", tcpConnection );

          constexpr s32 bufferSize = (s32) KiloBytes( 200 );
          char receiveBuffer[bufferSize];
          s32 bytesReceived;

          while ( true )
          {
            WebSocketPacketType packetType = websocket_receive( tcpSocket, receiveBuffer, bufferSize, &bytesReceived );

            if ( packetType == WebSocketPacketType::HANDSHAKE )
            {
              log( "Received message of type: HANDSHAKE\nMessage content:\n", separatorLineThin, "\n" );
              log( receiveBuffer, separatorLineThick, "\n" );
              WebSocketHandshakeInfo handshakeInfo = websocket_get_info_from_handshake_packet( receiveBuffer );

              char sendBuffer[8192];
              s32 bytesWritten = websocket_generate_handshake_response( handshakeInfo, sendBuffer, array_count( sendBuffer ) );
              if ( bytesWritten )
              {
                if ( tcp_send( tcpSocket, sendBuffer, bytesWritten ) )
                {
                  log( "Generated & sending response message with content:\n", separatorLineThin, "\n", sendBuffer, separatorLineThick, "\n" );
                }
              }
            }
            else if ( packetType == WebSocketPacketType::TEXT )
            {
              log( "Received message of type: TEXT" );
              log( "Message content:\n", separatorLineThin, "\n" );
              log( receiveBuffer, "\n\n", separatorLineThick, "\n" );

              char testmsg[512];
              //s32 written = string_format( testmsg, array_count( testmsg ), "I received this from you: ", receiveBuffer ) - 1;
              s32 written = string_format( testmsg, array_count( testmsg ), "HelloHello AhYes" ) - 1;
              websocket_send( tcpSocket, testmsg, written, WebSocketPacketType::TEXT );
            }
            else if ( packetType == WebSocketPacketType::BINARY )
            {
              log( "Received message of type: BINARY\nMessage content:\n", separatorLineThin, "\n TODO TODO TODO" );
            }
            else if ( packetType == WebSocketPacketType::CONNECTION_CLOSE )
            {
              log( "Received message of type: CONNECTION_CLOSE" );
              log( "Message content:\n", separatorLineThin, "\n" );
              log( receiveBuffer, "\n\n", separatorLineThick, "\n" );
              break;
            }
            else if ( packetType == WebSocketPacketType::PING )
            {
              log( "Received message of type: PING" );
            }
            else if ( packetType == WebSocketPacketType::PONG )
            {
              log( "Received message of type: PONG" );

            }
          }
        }
      }
    }
  }
  return 0;
}


