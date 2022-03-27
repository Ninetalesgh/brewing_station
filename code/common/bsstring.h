#pragma once
#include "bscommon.h"

namespace bs
{
  class String
  {
  public:
    String() {}
    String( String const& other ) : data( other.data ), capacity( other.capacity ) {}
    String( char* data, s32 capacity ) : data( data ), capacity( capacity ) {}
    char* data;
    s32 capacity;
  };

  //returns the length of the string, not counting the \0
  u32 string_length( char const* string );

  //returns the number of utf8 characters in the string, not counting the \0
  u32 string_utf8_length( char const* utf8String );

  //returns 1 if the strings match
  //returns 0 if they don't
  u32 string_match( char const* a, char const* b );
  u32 string_match( String const& a, String const& b );

  //returns pointer to where subString begins in string
  //returns nullptr if the subString is not part of string
  char const* string_contains( char const* string, char const* subString );

  //returns the next character in the string after parsing the codepoint
  char const* parse_utf8( char const* utf8String, s32* out_codepoint );

  s32 get_unicode_codepoint( char const* utf8String, s32* out_extraBytes = nullptr );

  template<bool internal, typename Arg> INLINE s32 string_format( String to, Arg value );
  template<typename Arg, typename... Args>
  INLINE s32 string_format( String to, Arg arg, Args... args )
  {
    s32 bytesWritten = string_format<true, Arg>( to, arg );
    bytesWritten += string_format<Args...>( String { to.data + bytesWritten, to.capacity - bytesWritten }, args... );

    return bytesWritten;
  }

  template<> INLINE s32 string_format<true, char const*>( String to, char const* value );
  template<> INLINE s32 string_format<true, char*>( String to, char* value );
  template<> INLINE s32 string_format<true, String>( String to, String value );
  template<> INLINE s32 string_format<true, u64>( String to, u64 value );
  template<> INLINE s32 string_format<true, u32>( String to, u32 value );
  template<> INLINE s32 string_format<true, u16>( String to, u16 value );
  template<> INLINE s32 string_format<true, u8>( String to, u8 value );
  template<> INLINE s32 string_format<true, s64>( String to, s64 value );
  template<> INLINE s32 string_format<true, s32>( String to, s32 value );
  template<> INLINE s32 string_format<true, s16>( String to, s16 value );
  template<> INLINE s32 string_format<true, s8>( String to, s8 value );

  s32 string_format_float( String to, float value, s32 const postPeriodDigits );
  template<> INLINE s32 string_format<true, float>( String to, float value ) { return string_format_float( to, value, 4 ); }

  template<bool internal, typename Arg> INLINE s32 string_format( String to, Arg value ) { static_assert(0); } //type doesn't exist for formatting yet, sorry :(

  template<typename Arg> INLINE s32 string_format( String to, Arg value )
  {
    s32 bytesWritten = string_format<true, Arg>( to, value );
    if ( to.capacity > bytesWritten )
    {
      to.data[bytesWritten] = '\0'; //end of format
    }
    return bytesWritten + 1;
  }
};

//////////////////
////impl//////////
//////////////////


namespace bs
{
  template<> INLINE s32 string_format<true, char const*>( String to, char const* value )
  {
    s32 result = 0;
    if ( value )
    {
      while ( *value != '\0' && to.capacity > result )
      {
        *to.data++ = *value++;
        ++result;
      }
    }
    return result;
  }

  template<> INLINE s32 string_format<true, char*>( String to, char* value )
  {
    return string_format<true, char const*>( to, (char const*) value );
  }

  template<> INLINE s32 string_format<true, u64>( String to, u64 value )
  {
    s32 result = 0;
    if ( to.capacity )
    {
      if ( value )
      {
        u64 divisor = 10000000000000000000;
        while ( divisor && to.capacity > result )
        {
          u8 place = u8( value / divisor );
          if ( result || place )
          {
            value %= divisor;
            *to.data++ = '0' + place;
            ++result;
          }
          divisor /= 10;
        }
      }
      else
      {
        *to.data = '0';
        ++result;
      }
    }
    return result;
  }

  template<> INLINE s32 string_format<true, u32>( String to, u32 value )
  {
    s32 result = 0;
    if ( to.capacity )
    {
      if ( value )
      {
        u32 divisor = 1000000000;
        while ( divisor && to.capacity > result )
        {
          u8 place = u8( value / divisor );
          if ( result || place )
          {
            value %= divisor;
            *to.data++ = '0' + place;
            ++result;
          }
          divisor /= 10;
        }
      }
      else
      {
        *to.data = '0';
        ++result;
      }
    }
    return result;
  }

  template<> INLINE s32 string_format<true, u16>( String to, u16 value )
  {
    s32 result = 0;
    if ( to.capacity )
    {
      if ( value )
      {
        u32 divisor = 10000;
        while ( divisor && to.capacity > result )
        {
          u8 place = u8( value / divisor );
          if ( result || place )
          {
            value %= divisor;
            *to.data++ = '0' + place;
            ++result;
          }
          divisor /= 10;
        }
      }
      else
      {
        *to.data = '0';
        ++result;
      }
    }
    return result;
  }


  template<> INLINE s32 string_format<true, u8>( String to, u8 value )
  {
    s32 result = 0;
    if ( to.capacity )
    {
      if ( value )
      {
        u32 divisor = 100;
        while ( divisor && to.capacity > result )
        {
          u8 place = u8( value / divisor );
          if ( result || place )
          {
            value %= divisor;
            *to.data++ = '0' + place;
            ++result;
          }
          divisor /= 10;
        }
      }
      else
      {
        *to.data = '0';
        ++result;
      }
    }
    return result;
  }

  template<> INLINE s32 string_format<true, s64>( String to, s64 value )
  {
    s32 result = 0;
    if ( value >> ((sizeof( s64 ) * 8) - 1) && to.capacity >= 0 )
    {
      *to.data++ = '-';
      value = -value;
      ++result;
    }
    return result + string_format<true, u64>( { to.data, to.capacity - result }, u64( value ) );
  }

  template<> INLINE s32 string_format<true, s32>( String to, s32 value )
  {
    s32 result = 0;
    if ( value >> ((sizeof( s32 ) * 8) - 1) && to.capacity >= 0 )
    {
      *to.data++ = '-';
      value = -value;
      ++result;
    }
    return result + string_format<true, u32>( { to.data, to.capacity - result }, u32( value ) );
  }

  template<> INLINE s32 string_format<true, s16>( String to, s16 value )
  {
    s32 result = 0;
    if ( value >> ((sizeof( s16 ) * 8) - 1) && to.capacity >= 0 )
    {
      *to.data++ = '-';
      value = -value;
      ++result;
    }
    return result + string_format<true, u16>( { to.data, to.capacity - result }, u16( value ) );
  }

  template<> INLINE s32 string_format<true, s8>( String to, s8 value )
  {
    s32 result = 0;
    if ( value >> ((sizeof( s8 ) * 8) - 1) && to.capacity >= 0 )
    {
      *to.data++ = '-';
      value = -value;
      ++result;
    }
    return result + string_format<true, u8>( { to.data, to.capacity - result }, u8( value ) );
  }

  s32 string_format_float( String to, float value, s32 const postPeriodDigits )
  {
    constexpr u32 MASK_FIRST_BIT = 0b10000000000000000000000000000000;
    constexpr u32 MASK_EXPONENT  = 0b01111111100000000000000000000000;
    constexpr u32 MASK_MANTISSA  = 0b00000000011111111111111111111111;
    constexpr u32 MASK_EXTRABIT  = 0b00000000100000000000000000000000;

    u32 const bits     = *(u32*) &value;
    u32 const sigfigs  = (bits & MASK_MANTISSA) | MASK_EXTRABIT;
    s32 const exponent = s32( (bits & MASK_EXPONENT) >> 23 ) - 127;

    s32 result = 0;
    {
      if ( bits & MASK_FIRST_BIT && to.capacity > result )
      {
        *to.data++ = '-';
        ++result;
      }

      if ( !(bits << 1) )
      {
        return result + string_format<true, char const*>( { to.data, to.capacity - result }, "0.0" );
      }

      if ( exponent == 128 )
      {
        return result + string_format<true, char const*>( { to.data, to.capacity - result }, "NaN" );
      }
    }

    //TODO
    constexpr s32 EXPONENT_LIMIT_MIN = -18;
    constexpr s32 EXPONENT_LIMIT_MAX = 23;
    if ( exponent > EXPONENT_LIMIT_MAX )
    {
      return result + string_format<true, char const*>( { to.data, to.capacity - result }, "bigfloat" );
    }
    else if ( exponent < EXPONENT_LIMIT_MIN )
    {
      return result + string_format<true, char const*>( { to.data, to.capacity - result }, "smlfloat" );
    }
    else
    {
      u32 whole = exponent >= 0 ? sigfigs >> (23 - exponent) : 0;

      //naive tmp array to figure out whether there's carryover to the whole part when rounding
      char tmpTo[24] = {};
      tmpTo[0] = '0';
      u64 fraction = 10;
      {
        u32 fractionBits;
        if ( exponent < -9 )
        {
          fractionBits = sigfigs >> -(exponent + 9);
        }
        else
        {
          fractionBits = sigfigs << (exponent + 9);
        }

        s32 count = 1;
        u64 num = 5;
        for ( ;;)
        {
          fraction = fraction + num * ((fractionBits & MASK_FIRST_BIT) != 0);
          if ( !(fractionBits<<= 1) ) break;
          if ( count < 19 )
          {
            ++count;
            fraction *= 10;
            num *= 5;
          }
          else
          {
            num >>= 1;
          }
        }

        for ( s32 i = max( 0, count - postPeriodDigits ); i--; )
        {
          fraction = (fraction + 5) / 10;
        }

        s32 resultFraction = 0;
        for ( s32 i = min( count, postPeriodDigits ); i--; )
        {
          u8 leftover = u8( fraction % 10 );
          fraction /= 10;
          if ( resultFraction || leftover )
          {
            tmpTo[i] = '0' + leftover;
            ++resultFraction;
          }
        }

        //rounding carry over to whole numbers
        if ( fraction == 2 )
        {
          ++whole;
        }
      }

      if ( whole )
      {
        s32 wholeResult = string_format<true, u32>( { to.data, to.capacity - result }, whole );
        to.data += wholeResult;
        result += wholeResult;
      }
      else if ( to.capacity > result )
      {
        *to.data++ = '0';
        ++result;
      }

      if ( to.capacity > result )
      {
        *to.data++ = '.';
        ++result;
      }

      return result + string_format<true, char const*>( { to.data, to.capacity - result }, tmpTo );
    }
  }

  INLINE u32 string_length( char const* string )
  {
    char const* reader = string;
    while ( *reader++ != '\0' ) {}
    return u32( reader - string ) - 1;
  }

  INLINE u32 string_utf8_length( char const* utf8String )
  {
    u32 result = 0;
    while ( *utf8String )
    {
      s32 codepoint;
      utf8String = parse_utf8( utf8String, &codepoint );
      ++result;
    }
    return result;
  }

  INLINE u32 string_match( String const& a, String const& b )
  {
    u32 result = 1;
    s32 capacity = min( a.capacity, b.capacity );
    for ( s32 i = 0; i < capacity; ++i )
    {
      if ( a.data[i] != b.data[i] )
      {
        result = 0;
        break;
      }
      else if ( a.data[i] == '\0' )
      {
        break;
      }
    }

    return result;
  }

  INLINE u32 string_match( char const* a, char const* b )
  {
    if ( a == nullptr || b == nullptr )
    {
      return 0;
    }

    u32 result = 0;
    while ( *a == *b )
    {
      if ( *a == '\0' )
      {
        result = 1;
        break;
      }

      ++a;
      ++b;
    }

    if ( (*a) * (*b) == 0 )
    {
      result = 1;
    }

    return 0;
  }

  char const* string_contains( char const* string, char const* subString )
  {
    if ( string == nullptr || subString == nullptr ) return 0;

    char const* result = nullptr;
    char const* reader = string;

    while ( *reader != '\0' )
    {
      if ( *reader == *subString )
      {
        char const* a = reader;
        char const* b = subString;
        while ( *a == *b )
        {
          ++a;
          ++b;
        }

        if ( *b == '\0' )
        {
          result = reader;
          break;
        }

        reader = a;
      }
      else
      {
        ++reader;
      }
    }

    return result;
  }

  s32 get_unicode_codepoint( char const* utf8String, s32* out_extraBytes /*= nullptr*/ )
  {
    s32 result = 0;
    u8 const* reader = (u8 const*) utf8String;
    u8 const extraByteCheckMask = 0b10000000;
    u8 const extraByteValueMask = 0b00111111;

    u8 unicodeMask = 0b11000000;

    s32 extraBytes = 0;
    result = *(u8*) (reader);
    while ( *utf8String & unicodeMask )
    {
      unicodeMask >>= 1;
      ++reader;
      if ( (*reader & ~extraByteValueMask) == extraByteCheckMask )
      {
        result <<= 6;
        result += (*reader) & extraByteValueMask;

        ++extraBytes;
      }
      else
      {
        break;
      }
    }

    if ( extraBytes )
    {
      s32 maskLength = 1 << (5 * extraBytes + 6);
      result &= (maskLength - 1);
    }
    else
    {
      //TODO maybe check whether it's a valid 1 byte character? aka: < 128
    }

    if ( out_extraBytes ) *out_extraBytes = extraBytes;
    return result;
  }

  char const* parse_utf8( char const* utf8String, s32* out_codepoint )
  {
    char const* nextChar = nullptr;
    if ( *utf8String != '\0' )
    {
      s32 extraBytes = 0;
      *out_codepoint = get_unicode_codepoint( utf8String, &extraBytes );
      nextChar = utf8String + 1 + extraBytes;
    }
    return nextChar;
  }
};