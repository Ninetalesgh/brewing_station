#include <core/bsnet.h>


namespace bs
{
  namespace net
  {

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
  };

  namespace string
  {
    template<> INLINE s32 format<true, net::Connection>( char* destination, s32 capacity, net::Connection connection )
    {
      return format( destination, capacity, connection.ipv4_u8_address0, ".", connection.ipv4_u8_address1, ".",
                                connection.ipv4_u8_address2, ".", connection.ipv4_u8_address3,
                                ":", connection.port ) - 1;
    }
  }
};