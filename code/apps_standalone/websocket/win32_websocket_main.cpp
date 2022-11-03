

#include "sha1.h"

#include <core/internal/bsnet.cpp>
#include <common/bsstring.h>

#include <stdio.h>
#include <winsock2.h>

//winsock
#pragma comment(lib,"ws2_32.lib")

constexpr u32 MAX_DEBUG_MESSAGE_LENGTH = 8192;
template<typename... Args> void log( Args... args )
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

  char test[100] = {};

  sha1_to_hex( "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11", 0, test, 100 );

  log( test );



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

          if ( string_contains( receiveBuffer, "GET" ) == receiveBuffer )
          {
            char const* key = nullptr;
            char const* keyText = "Sec-WebSocket-Key: ";
            key = string_contains( receiveBuffer, keyText );
            if ( key )
            {
              char const* magic_key = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
              key += string_length( keyText );

              char sendBuffer[8192];

              s32 writerIdx = string_format( sendBuffer, 8192
                                          , "HTTP/1.1 101 Switching Protocols\r\n"
                                          , "Upgrade: websocket\r\n"
                                          , "Connection: Upgrade\r\n"
                                          , "Sec-WebSocket-Accept: " );



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


#if 0



class SHA1
{
public:
  SHA1();
  void update( const std::string& s );
  void update( std::istream& is );
  std::string final();
  static std::string from_file( const std::string& filename );

private:
  uint32_t digest[5];
  std::string buffer;
  uint64_t transforms;
};



inline void SHA1::update( std::istream& is )
{
  while ( true )
  {
    char sbuf[BLOCK_BYTES];
    is.read( sbuf, BLOCK_BYTES - buffer.size() );
    buffer.append( sbuf, (std::size_t) is.gcount() );
    if ( buffer.size() != BLOCK_BYTES )
    {
      return;
    }
    uint32_t block[BLOCK_INTS];
    buffer_to_block( buffer, block );
    transform( digest, block, transforms );
    buffer.clear();
  }
}


/*
 * Add padding and return the message digest.
 */

inline std::string SHA1::final()
{
  /* Total number of hashed bits */
  uint64_t total_bits = (transforms * BLOCK_BYTES + buffer.size()) * 8;

  /* Padding */
  buffer += (char) 0x80;
  size_t orig_size = buffer.size();
  while ( buffer.size() < BLOCK_BYTES )
  {
    buffer += (char) 0x00;
  }

  uint32_t block[BLOCK_INTS];
  buffer_to_block( buffer, block );

  if ( orig_size > BLOCK_BYTES - 8 )
  {
    transform( digest, block, transforms );
    for ( size_t i = 0; i < BLOCK_INTS - 2; i++ )
    {
      block[i] = 0;
    }
  }

  /* Append total_bits, split this uint64_t into two uint32_t */
  block[BLOCK_INTS - 1] = (uint32_t) total_bits;
  block[BLOCK_INTS - 2] = (uint32_t) (total_bits >> 32);
  transform( digest, block, transforms );

  /* Hex std::string */
  std::ostringstream result;
  for ( size_t i = 0; i < sizeof( digest ) / sizeof( digest[0] ); i++ )
  {
    result << std::hex << std::setfill( '0' ) << std::setw( 8 );
    result << digest[i];
  }

  /* Reset for next run */
  reset( digest, buffer, transforms );

  return result.str();
}


#endif /* SHA1_HPP */





