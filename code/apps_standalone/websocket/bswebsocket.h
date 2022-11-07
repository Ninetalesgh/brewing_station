#pragma once
//thanks to
//https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API/Writing_WebSocket_server 
//for easy to understand tutorials

#include <common/bsstring.h>
#include <common/bsbit.h>
#include <core/internal/bsnet.cpp>

#include "sha1.h"

#include <winsock2.h>

//winsock
#pragma comment(lib,"ws2_32.lib")


//hacky logger real quick
constexpr u32 MAX_DEBUG_MESSAGE_LENGTH = 8192;
template<typename... Args> void log( Args... args )
{
  char debugBuffer[MAX_DEBUG_MESSAGE_LENGTH];
  s32 bytesToWrite = bs::string_format( debugBuffer, MAX_DEBUG_MESSAGE_LENGTH - 1, args... ) - 1 /* ommit null */;
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

u32 tcp_send( SOCKET sock, char const* data, s32 size )
{
  return send( sock, data, size, 0 ) != SOCKET_ERROR;
}


namespace bs
{
  enum class WebSocketState
  {
    CONNECTING = 0x1,
    OPEN       = 0x2,
    CLOSED     = 0x3
  };

  enum class WebSocketOpcode: u8
  {
    FRAME_CONTINUATION       = 0x0,
    NON_CONTROL_TEXT_FRAME   = 0x1,
    NON_CONTROL_BINARY_FRAME = 0x2,
    CONTROL_CONNECTION_CLOSE = 0x8,
    CONTROL_PING             = 0x9,
    CONTROL_PONG             = 0xA
  };

  enum class WebSocketPacketType: u32
  {
    INVALID = 0x0,
    TEXT = 0x1,
    BINARY = 0x2,
    HANDSHAKE = 0x10,
    CONNECTION_CLOSE = 0x8,
    PING = 0x9,
    PONG = 0xA
  };


  DEFINE_ENUM_OPERATORS_U32( WebSocketPacketType )

    struct WebSocketHandshakeInfo
  {
    char key[25];
  };

  struct WebSocket
  {
    u64 socket; //TODO, is it relevant to downsize this if pointers are 32bit? 
    net::Connection connection;
    WebSocketState state;
  };



  INLINE WebSocketOpcode websocket_get_packet_opcode( u8* packet )
  {
    return WebSocketOpcode( (*packet) & 0b00001111 );
  }

  WebSocketHandshakeInfo websocket_get_info_from_handshake_packet( char const* handshakePacket )
  {
    WebSocketHandshakeInfo result;
    result.key[0] = 0;
    char const* keyText = "Sec-WebSocket-Key: ";
    char const* key = string_contains( handshakePacket, keyText );
    if ( key )
    {
      key += string_length( keyText );
      string_format( result.key, array_count( result.key ), key );
    }

    return result;
  }

  s32 websocket_generate_handshake_response( WebSocketHandshakeInfo const& handshakeInfo, char* out_messageBuffer, s32 messageBufferSize )
  {
    char const* magicWebSocketValue = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    bs::string_format( out_messageBuffer, messageBufferSize, (char const*) handshakeInfo.key, magicWebSocketValue );

    char hash[29] = {};

    sha1_to_base64( out_messageBuffer, 0, hash, 29 );

    s32 writerIdx = bs::string_format( out_messageBuffer, messageBufferSize
                                     , "HTTP/1.1 101 Switching Protocols\r\n"
                                     , "Upgrade: websocket\r\n"
                                     , "Connection: Upgrade\r\n"
                                     , "Sec-WebSocket-Accept: ", hash, "\r\n\r\n" );

    return writerIdx;
  }

  WebSocketPacketType websocket_receive( SOCKET socket, char* receiveBuffer, s32 receiveBufferSize, s32* out_bytesReceived )
  {
    using namespace bs;
    WebSocketPacketType resultType = WebSocketPacketType::INVALID;

    if ( tcp_receive( socket, receiveBuffer, receiveBufferSize, out_bytesReceived ) )
    {
      if ( string_match( receiveBuffer, "GET" ) )
      {
        resultType = WebSocketPacketType::HANDSHAKE;
      }
      else
      {
        bool fin = (receiveBuffer[0] & 0b10000000) != 0;
        bool mask = (receiveBuffer[1] & 0b10000000) != 0;
        WebSocketOpcode opcode = WebSocketOpcode( receiveBuffer[0] & 0b00001111 );
        int offset = 2;
        u64 packetSize = (u64) (receiveBuffer[1] & 0b01111111);
        if ( packetSize == 126 )
        {
          // convert from big-endian
          packetSize = (u64) bs::byte_to_u16( &receiveBuffer[2] );
          offset = 4;
        }
        else if ( packetSize == 127 )
        {
          // convert from big-endian
          packetSize = bs::byte_to_u64( &receiveBuffer[2] );

          offset = 10;
        }

        //decode message in place
        if ( mask )
        {
          char masks[4] = { receiveBuffer[offset], receiveBuffer[offset + 1], receiveBuffer[offset + 2], receiveBuffer[offset + 3] };
          offset += 4;

          assert( packetSize <= (u64) S32_MAX );
          s32 bytesToWrite = min( (s32) packetSize, receiveBufferSize - 1 );
          for ( u64 i = 0; i < bytesToWrite; ++i )
          {
            receiveBuffer[i] = (char) (receiveBuffer[offset + s32( i )] ^ masks[i % 4]);
          }

          receiveBuffer[bytesToWrite] = '\0';
        }
        else
        {
          //an error if coming from a client but fine if coming from a server?
          assert( 0 );
        }

        switch ( opcode )
        {
          case WebSocketOpcode::FRAME_CONTINUATION:
          {
            assert( 0 );//TODO
            break;
          }
          case WebSocketOpcode::NON_CONTROL_TEXT_FRAME:
          {
            resultType = WebSocketPacketType::TEXT;
            break;
          }
          case WebSocketOpcode::NON_CONTROL_BINARY_FRAME:
          {
            assert( 0 );//TODO
            break;
          }
          case WebSocketOpcode::CONTROL_CONNECTION_CLOSE:
          {
            resultType = WebSocketPacketType::CONNECTION_CLOSE;
            break;
          }
          case WebSocketOpcode::CONTROL_PING:
          {
            resultType = WebSocketPacketType::PING;
            break;
          }
          case WebSocketOpcode::CONTROL_PONG:
          {
            resultType = WebSocketPacketType::PONG;
            break;
          }
          default:
          {
            assert( 0 );//TODO
            break;
          }
        }
      }
    }
    return resultType;
  }


  void websocket_send()
  {
    ///FOR SENDING 
    /*
    set this if it's the last package:
    bool fin = (receiveBuffer[0] & 0b10000000) != 0;

  //<FIRST BYTE
    u8 fin    = 0b10000000;

    u8 text   = 0b00000001;
    u8 binary = 0b00000010;

    u8 close  = 0b00001000;
    u8 ping   = 0b00001001;
    u8 pong   = 0b00001010;
  //FIRST BYTE>

  //<SECOND BYTE
    u8 mask          = 0b10000000;
    u8 payloadLength = 0b01111111; IF SIZE <126
  //


websocket send type:

0x1 text
0x2 binary
0x3-7 reserved

fragmented / non-fragmented

control frames:
0x8 close   -> body must either be empty or start with big-endian u16 then message for why it closed ;;;; close must be responded with a close message back
0x9 ping  -> should respond with pong  ;;;;; keepalive
0xA pong   -> can send pong without ping, serves as unidirectional heartbeat
0xB - 0xF reserved


                  */
  }


};



////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////inl/////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////




namespace bs
{




};