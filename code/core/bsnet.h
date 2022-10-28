#pragma once

#include <common/bsstring.h>
#include <common/bscommon.h>

#define APP_CLIENT_COUNT_MAX 24

namespace bs
{
  namespace net
  {
    constexpr u32 IPv4_ADDRESS_ANY = 0;
    constexpr u32 IPv4_ADDRESS_INVALID = U32_MAX;
    constexpr u16 PORT_ANY = 0;
    constexpr u16 PORT_INVALID = U16_MAX;

    struct PacketHeader
    {
      u32 flags;

      enum: u32
      {
        PLATFORM_RESERVED = 0x80000000, //if set the platform won't forward the packet to the app
      };
    };

    struct Connection
    {
      Connection(): ipv4_address( 0 ), port( 0 ) {}
      Connection( Connection const& other ): ipv4_address( other.ipv4_address ), port( other.port ) {}
      Connection( u32 ipv4_address, u16 port ): ipv4_address( ipv4_address ), port( port ) {}
      union
      {
        u32 ipv4_address;
        struct
        {
          u8 ipv4_u8_address0;
          u8 ipv4_u8_address1;
          u8 ipv4_u8_address2;
          u8 ipv4_u8_address3;
        };
      };
      u16 port;
      //u16 padding;

      INLINE u32 operator ==( Connection const& other ) { return (ipv4_address == other.ipv4_address && port == other.port); }
      INLINE u32 operator !=( Connection const& other ) { return (ipv4_address != other.ipv4_address && port != other.port); }
    };
    INLINE u32 is_valid_connection( Connection connection ) { return connection.port != U16_MAX && connection.ipv4_address != U32_MAX; }

    struct NetworkData
    {
      net::Connection self;
      net::Connection server;
      net::Connection connections[APP_CLIENT_COUNT_MAX];
      u32        connectionCount;
    };

    struct TCPSendParameter
    { //currently only file transfer? 
      net::Connection to;
      char const* fileData;
      u32         fileSize;
      char const* filename;
    };

    struct UDPSendParameter
    {
      net::Connection to;
      char const* packet;
      u32         packetSize;
    };

    struct UDPReceiveParameter
    {
      net::Connection sender;
      char const* packet;
      u32         packetSize;
      u32         id;
    };


    u32 parse_ipv4( char const* from );
  };


  template<> INLINE s32 string_format<true, net::Connection>( char* destination, s32 capacity, net::Connection connection );

};