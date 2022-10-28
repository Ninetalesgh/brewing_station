#pragma once
#include "bscommon.h"

namespace bs
{

  //returns the number of bytes in the string, not counting the \0
  s32 string_length( char const* string );

  //returns the number of utf8 characters in the string, not counting the \0
  s32 string_length_utf8( char const* utf8String );

  //returns the number of lines in the string
  s32 string_line_count( char const* string );

  //returns the number of bytes until a \n or a \0, including it
  s32 string_line_length( char const* string );

  //returns the number of utf8 characters until a \n or a \0, including it
  s32 string_line_length_utf8( char const* utf8String );

  //returns 1 if the strings match
  //returns 0 if they don't
  u32 string_match( char const* a, char const* b );

  //returns pointer to where subString begins in string
  //returns nullptr if the subString is not part of string
  char* string_contains( char* string, char const* subString );
  char const* string_contains( char const* string, char const* subString );

  //returns the next character in the string after parsing the codepoint
  char const* string_parse_utf8( char const* utf8String, s32* out_codepoint );

  //returns the character after reading the expected value
  char const* string_parse_value( char const* floatString, float* out_float );
  char const* string_parse_value( char const* intString, s32* out_int );


  template<bool internal, typename Arg> INLINE s32 string_format( char* destination, s32 capacity, Arg value );
  template<typename Arg, typename... Args>
  INLINE s32 string_format( char* destination, s32 capacity, Arg arg, Args... args )
  {
    s32 bytesWritten = string_format<true, Arg>( destination, capacity, arg );
    bytesWritten += string_format<Args...>( destination + bytesWritten, capacity - bytesWritten, args... );

    return bytesWritten;
  }

  template<> INLINE s32 string_format<true, char const*>( char* destination, s32 capacity, char const* value );
  template<> INLINE s32 string_format<true, char*>( char* destination, s32 capacity, char* value );
  template<> INLINE s32 string_format<true, u64>( char* destination, s32 capacity, u64 value );
  template<> INLINE s32 string_format<true, u32>( char* destination, s32 capacity, u32 value );
  template<> INLINE s32 string_format<true, u16>( char* destination, s32 capacity, u16 value );
  template<> INLINE s32 string_format<true, u8>( char* destination, s32 capacity, u8 value );
  template<> INLINE s32 string_format<true, s64>( char* destination, s32 capacity, s64 value );
  template<> INLINE s32 string_format<true, s32>( char* destination, s32 capacity, s32 value );
  template<> INLINE s32 string_format<true, s16>( char* destination, s32 capacity, s16 value );
  template<> INLINE s32 string_format<true, s8>( char* destination, s32 capacity, s8 value );

  s32 string_format_float( char* destination, s32 capacity, float value, s32 const postPeriodDigits );
  template<> INLINE s32 string_format<true, float>( char* destination, s32 capacity, float value ) { return string_format_float( destination, capacity, value, 4 ); }

  template<bool internal, typename Arg> INLINE s32 string_format( char* destination, s32 capacity, Arg value ) { static_assert(0); } //type doesn't exist for formatting yet, sorry :(

  template<typename Arg> INLINE s32 string_format( char* destination, s32 capacity, Arg value )
  {
    s32 bytesWritten = string_format<true, Arg>( destination, capacity, value );
    if ( capacity > bytesWritten )
    {
      destination[bytesWritten] = '\0'; //end of string_format
    }
    return bytesWritten + 1;
  }


};


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////inl/////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace bs
{

  template<> INLINE s32 string_format<true, char const*>( char* destination, s32 capacity, char const* value )
  {
    s32 result = 0;
    if ( value )
    {
      while ( *value != '\0' && capacity > result )
      {
        *destination++ = *value++;
        ++result;
      }
    }
    return result;
  }

  template<> INLINE s32 string_format<true, char*>( char* destination, s32 capacity, char* value )
  {
    return string_format<true, char const*>( destination, capacity, (char const*) value );
  }

  template<> INLINE s32 string_format<true, u64>( char* destination, s32 capacity, u64 value )
  {
    s32 result = 0;
    if ( capacity )
    {
      if ( value )
      {
        u64 divisor = 10000000000000000000;
        while ( divisor && capacity > result )
        {
          u8 place = u8( value / divisor );
          if ( result || place )
          {
            value %= divisor;
            *destination++ = '0' + place;
            ++result;
          }
          divisor /= 10;
        }
      }
      else
      {
        *destination = '0';
        ++result;
      }
    }
    return result;
  }

  template<> INLINE s32 string_format<true, u32>( char* destination, s32 capacity, u32 value )
  {
    s32 result = 0;
    if ( capacity )
    {
      if ( value )
      {
        u32 divisor = 1000000000;
        while ( divisor && capacity > result )
        {
          u8 place = u8( value / divisor );
          if ( result || place )
          {
            value %= divisor;
            *destination++ = '0' + place;
            ++result;
          }
          divisor /= 10;
        }
      }
      else
      {
        *destination = '0';
        ++result;
      }
    }
    return result;
  }

  template<> INLINE s32 string_format<true, u16>( char* destination, s32 capacity, u16 value )
  {
    s32 result = 0;
    if ( capacity )
    {
      if ( value )
      {
        u32 divisor = 10000;
        while ( divisor && capacity > result )
        {
          u8 place = u8( value / divisor );
          if ( result || place )
          {
            value %= divisor;
            *destination++ = '0' + place;
            ++result;
          }
          divisor /= 10;
        }
      }
      else
      {
        *destination = '0';
        ++result;
      }
    }
    return result;
  }


  template<> INLINE s32 string_format<true, u8>( char* destination, s32 capacity, u8 value )
  {
    s32 result = 0;
    if ( capacity )
    {
      if ( value )
      {
        u32 divisor = 100;
        while ( divisor && capacity > result )
        {
          u8 place = u8( value / divisor );
          if ( result || place )
          {
            value %= divisor;
            *destination++ = '0' + place;
            ++result;
          }
          divisor /= 10;
        }
      }
      else
      {
        *destination = '0';
        ++result;
      }
    }
    return result;
  }

  template<> INLINE s32 string_format<true, s64>( char* destination, s32 capacity, s64 value )
  {
    s32 result = 0;
    if ( value >> ((sizeof( s64 ) * 8) - 1) && capacity >= 0 )
    {
      *destination++ = '-';
      value = -value;
      ++result;
    }
    return result + string_format<true, u64>( destination, capacity - result, u64( value ) );
  }

  template<> INLINE s32 string_format<true, s32>( char* destination, s32 capacity, s32 value )
  {
    s32 result = 0;
    if ( value >> ((sizeof( s32 ) * 8) - 1) && capacity >= 0 )
    {
      *destination++ = '-';
      value = -value;
      ++result;
    }
    return result + string_format<true, u32>( destination, capacity - result, u32( value ) );
  }

  template<> INLINE s32 string_format<true, s16>( char* destination, s32 capacity, s16 value )
  {
    s32 result = 0;
    if ( value >> ((sizeof( s16 ) * 8) - 1) && capacity >= 0 )
    {
      *destination++ = '-';
      value = -value;
      ++result;
    }
    return result + string_format<true, u16>( destination, capacity - result, u16( value ) );
  }

  template<> INLINE s32 string_format<true, s8>( char* destination, s32 capacity, s8 value )
  {
    s32 result = 0;
    if ( value >> ((sizeof( s8 ) * 8) - 1) && capacity >= 0 )
    {
      *destination++ = '-';
      value = -value;
      ++result;
    }
    return result + string_format<true, u8>( destination, capacity - result, u8( value ) );
  }

  s32 string_format_float( char* destination, s32 capacity, float value, s32 const postPeriodDigits )
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
      if ( bits & MASK_FIRST_BIT && capacity > result )
      {
        *destination++ = '-';
        ++result;
      }

      if ( !(bits << 1) )
      {
        return result + string_format<true, char const*>( destination, capacity - result, "0.0" );
      }

      if ( exponent == 128 )
      {
        return result + string_format<true, char const*>( destination, capacity - result, "NaN" );
      }
    }

    //TODO
    constexpr s32 EXPONENT_LIMIT_MIN = -18;
    constexpr s32 EXPONENT_LIMIT_MAX = 23;
    if ( exponent > EXPONENT_LIMIT_MAX )
    {
      return result + string_format<true, char const*>( destination, capacity - result, "bigfloat" );
    }
    else if ( exponent < EXPONENT_LIMIT_MIN )
    {
      return result + string_format<true, char const*>( destination, capacity - result, "smlfloat" );
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
        s32 wholeResult = string_format<true, u32>( destination, capacity - result, whole );
        destination += wholeResult;
        result += wholeResult;
      }
      else if ( capacity > result )
      {
        *destination++ = '0';
        ++result;
      }

      if ( capacity > result )
      {
        *destination++ = '.';
        ++result;
      }

      return result + string_format<true, char const*>( destination, capacity - result, tmpTo );
    }
  }

  INLINE s32 string_length( char const* string )
  {
    char const* reader = string;
    while ( *reader++ != '\0' ) {}
    return u32( reader - string ) - 1;
  }

  INLINE s32 string_length_utf8( char const* utf8String )
  {
    u32 result = 0;
    while ( *utf8String )
    {
      s32 codepoint;
      utf8String = string_parse_utf8( utf8String, &codepoint );
      ++result;
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

  INLINE s32 string_line_count( char const* string )
  {
    char const* reader = string;
    s32 result = 1;
    while ( *reader != '\0' )
    {
      if ( *reader == '\n' )
      {
        ++result;
      }

      ++reader;
    }

    return result;
  }

  INLINE s32 string_line_length( char const* string )
  {
    char const* reader = string;
    while ( *reader != '\0' && *reader != '\n' ) { ++reader; }
    return u32( reader - string );
  }

  INLINE s32 string_line_length_utf8( char const* utf8String )
  {
    u32 result = 1;
    while ( *utf8String && *utf8String != '\n' )
    {
      s32 codepoint;
      utf8String = string_parse_utf8( utf8String, &codepoint );
      ++result;
    }
    return result;
  }

  char* string_contains( char* string, char const* subString )
  {
    if ( string == nullptr || subString == nullptr ) return 0;

    char* result = nullptr;
    char* reader = string;

    while ( *reader != '\0' )
    {
      if ( *reader == *subString )
      {
        char* a = reader;
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

  s32 string_get_unicode_codepoint( char const* utf8String, s32* out_extraBytes /*= nullptr*/ )
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

  INLINE char const* string_parse_utf8( char const* utf8String, s32* out_codepoint )
  {
    char const* nextChar = nullptr;
    if ( *utf8String != '\0' )
    {
      s32 extraBytes = 0;
      *out_codepoint = string_get_unicode_codepoint( utf8String, &extraBytes );
      nextChar = utf8String + 1 + extraBytes;
    }
    else
    {
      *out_codepoint = 0;
    }
    return nextChar;
  }

  char const* string_parse_value( char const* floatString, float* out_float )
  {
    float result = 0.0f;
    float sign = 1.0f;
    char const* reader = floatString;
    float divisor = 0.0f;

    while ( *reader == ' ' ) { ++reader; }
    while ( *reader != ' ' && *reader != '\0' )
    {
      if ( *reader == '-' )
      {
        sign = -1.0f;
      }
      else if ( *reader == '.' && divisor == 0.0f )
      {
        divisor = 10.0f;
      }
      else if ( *reader == 'f' )
      {
        ++reader;
        break;
      }
      else if ( *reader >= '0' && *reader <= '9' )
      {
        if ( divisor == 0.0f )
        {
          result *= 10.0f;
          result += float( *reader - '0' );
        }
        else
        {
          result += float( *reader - '0' ) / divisor;
          divisor *= 10.0f;
        }
      }

      ++reader;
    }

    *out_float = result * sign;
    return reader;
  }

  char const* string_parse_value( char const* intString, s32* out_int )
  {
    s32 result = 0;
    s32 sign = 1;
    char const* reader = intString;
    while ( *reader == ' ' ) { ++reader; }
    while ( *reader != ' ' && *reader != '\0' )
    {
      if ( *reader == '-' )
      {
        sign = -1;
      }
      else if ( *reader >= '0' && *reader <= '9' )
      {
        result *= 10;
        result += s32( *reader - '0' );

      }

      ++reader;
    }

    *out_int = result * sign;
    return reader;
  }
};