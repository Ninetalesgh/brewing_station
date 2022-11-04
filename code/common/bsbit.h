#pragma once

#include "bscommon.h"

namespace bs
{

  u16 byte_to_u16( u8 highestByte, u8 byte1 )
  {
    return (u16( highestByte ) << 8) + u16( byte1 );
  }

  u16 byte_to_u16( char highestByte, char byte1 )
  {
    return byte_to_u16( (u8) highestByte, (u8) byte1 );
  }

  u16 byte_to_u16( u8* bytes )
  {
    return byte_to_u16( bytes[0], bytes[1] );
  }

  u16 byte_to_u16( char* bytes )
  {
    return byte_to_u16( (u8) bytes[0], (u8) bytes[1] );
  }

  u64 byte_to_u64( u8 highestByte, u8 byte1, u8 byte2, u8 byte3, u8 byte4, u8 byte5, u8 byte6, u8 byte7 )
  {
    return (u64( highestByte ) << 56)
      + (u64( byte1 ) << 48)
      + (u64( byte2 ) << 40)
      + (u64( byte3 ) << 32)
      + (u64( byte4 ) << 24)
      + (u64( byte5 ) << 16)
      + (u64( byte6 ) << 8)
      + (u64( byte7 ));
  }

  u64 byte_to_u64( char highestByte, char byte1, char byte2, char byte3, char byte4, char byte5, char byte6, char byte7 )
  {
    return byte_to_u64( (u8) highestByte, (u8) byte1, (u8) byte2, (u8) byte3, (u8) byte4, (u8) byte5, (u8) byte6, (u8) byte7 );
  }

  u64 byte_to_u64( u8* bytes )
  {
    return byte_to_u64( bytes[0]
                      , bytes[1]
                      , bytes[2]
                      , bytes[3]
                      , bytes[4]
                      , bytes[5]
                      , bytes[6]
                      , bytes[7] );
  }

  u64 byte_to_u64( char* bytes )
  {
    return byte_to_u64( (u8) bytes[0]
                      , (u8) bytes[1]
                      , (u8) bytes[2]
                      , (u8) bytes[3]
                      , (u8) bytes[4]
                      , (u8) bytes[5]
                      , (u8) bytes[6]
                      , (u8) bytes[7] );
  }

};