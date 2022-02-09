#pragma once
#include "bscommon.h"

namespace bs
{
  struct string
  {
    string() {}
    string( string const& other ) : data( other.data ), capacity( other.capacity ) {}
    string( char* data, s32 capacity ) : data( data ), capacity( capacity ) {}
    char* data;
    s32 capacity;
  };

  template<bool internal, typename Arg> INLINE s32 string_format( string to, Arg value );
  template<typename Arg, typename... Args>
  INLINE s32 string_format( string to, Arg arg, Args... args )
  {
    s32 bytesWritten = string_format<true, Arg>( to, arg );
    bytesWritten += string_format<Args...>( string { to.data + bytesWritten, to.capacity - bytesWritten }, args... );

    return bytesWritten;
  }

  template<> INLINE s32 string_format<true, char const*>( string to, char const* value );
  template<> INLINE s32 string_format<true, char*>( string to, char* value );
  template<> INLINE s32 string_format<true, string>( string to, string value );
  template<> INLINE s32 string_format<true, u64>( string to, u64 value );
  template<> INLINE s32 string_format<true, u32>( string to, u32 value );
  template<> INLINE s32 string_format<true, u16>( string to, u16 value );
  template<> INLINE s32 string_format<true, u8>( string to, u8 value );
  template<> INLINE s32 string_format<true, s64>( string to, s64 value );
  template<> INLINE s32 string_format<true, s32>( string to, s32 value );
  template<> INLINE s32 string_format<true, s16>( string to, s16 value );
  template<> INLINE s32 string_format<true, s8>( string to, s8 value );

  s32 string_format_float( string to, float value, s32 const postPeriodDigits );
  template<> INLINE s32 string_format<true, float>( string to, float value ) { return string_format_float( to, value, 4 ); }

  template<bool internal, typename Arg> INLINE s32 string_format( string to, Arg value ) { static_assert(0); } //type doesn't exist for formatting yet, sorry :(

  template<typename Arg> INLINE s32 string_format( string to, Arg value )
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
  template<> INLINE s32 string_format<true, char const*>( string to, char const* value )
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

  template<> INLINE s32 string_format<true, char*>( string to, char* value )
  {
    return string_format<true, char const*>( to, (char const*) value );
  }

  template<> INLINE s32 string_format<true, u64>( string to, u64 value )
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

  template<> INLINE s32 string_format<true, u32>( string to, u32 value )
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

  template<> INLINE s32 string_format<true, u16>( string to, u16 value )
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


  template<> INLINE s32 string_format<true, u8>( string to, u8 value )
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

  template<> INLINE s32 string_format<true, s64>( string to, s64 value )
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

  template<> INLINE s32 string_format<true, s32>( string to, s32 value )
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

  template<> INLINE s32 string_format<true, s16>( string to, s16 value )
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

  template<> INLINE s32 string_format<true, s8>( string to, s8 value )
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

  s32 string_format_float( string to, float value, s32 const postPeriodDigits )
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
        for ( ;; )
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
};