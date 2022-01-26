#pragma once

#include <common/string.h>
#include <common/basic_types.h>

namespace net
{
  struct PacketHeader
  {
    u32 flags;

    enum : u32
    {
      PLATFORM_RESERVED = 0x80000000, //if set the platform won't forward the packet to the app
    };
  };

  struct Connection
  {
    Connection() : ipv4_address( 0 ), port( 0 ) {}
    Connection( Connection const& other ) : ipv4_address( other.ipv4_address ), port( other.port ) {}
    Connection( u32 ipv4_address, u16 port ) : ipv4_address( ipv4_address ), port( port ) {}
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

    constexpr_member u32 IPv4_ADDRESS_ANY = 0;
    constexpr_member u32 IPv4_ADDRESS_INVALID = U32_MAX;
    constexpr_member u16 PORT_ANY = 0;
    constexpr_member u16 PORT_INVALID = U16_MAX;
  };
  INLINE u32 is_valid_connection( Connection connection ) { return connection.port != U16_MAX && connection.ipv4_address != U32_MAX; }
};

template<> INLINE s32 string_format<true, net::Connection>( char* to, net::Connection connection )
{
  return string_format( to, connection.ipv4_u8_address0, ".", connection.ipv4_u8_address1, ".",
                            connection.ipv4_u8_address2, ".", connection.ipv4_u8_address3,
                            ":", connection.port ) - 1;
}

u32 parse_ipv4( char const* from )
{
  u32 result = 0;
  char const* reader = from;

  for ( s32 i = 0; i < 4; ++i )
  {
    char const* section = reader;
    s32 digit = 0;
    while ( *reader != '.' && *reader != '\0' )
    {
      ++reader;
      if ( ++digit > 3 ) return 0;
    }
    ++reader;

    u8 multiplier = 1;
    u8 u8result = 0;
    for ( ;digit--; multiplier *= 10 )
    {
      u8result += multiplier * (section[digit] - '0');
    }

    result += u32( u8result ) << (i * 8);
  }

  return result;
}