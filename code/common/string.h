#pragma once
#include "basic_types.h"

struct string
{
  string() {}
  string( char charArray[] ) : data( charArray ), capacity( array_size( charArray ) ) {}
  string( char const* data, s32 capacity ) : data( data ), capacity( capacity ) {}
  char const* data;
  s32 capacity;
};

INLINE void string_copy( char* to, char const* from, s32 const size );
INLINE u32 string_match( char const* a, char const* b );
INLINE s32 string_first( char const* begin, s32 size, char const find );
INLINE s32 string_last( char const* begin, s32 size, char const find );
INLINE s32 string_length( char const* begin );


//naive UNSAFE string formatting 

//simple explicit casts for string_format overloading
struct Hex64 { u64 value; Hex64( u64 u ) : value( u ) {} Hex64( s64 s ) : value( u64( s ) ) {} };
struct Hex32 { u32 value; Hex32( u32 u ) : value( u ) {} Hex32( s32 s ) : value( u32( s ) ) {} };
struct Hex16 { u16 value; Hex16( u16 u ) : value( u ) {} Hex16( s16 s ) : value( u16( s ) ) {} };
struct Hex8 { u8 value; Hex8( u8 u ) : value( u ) {} Hex8( s8 s ) : value( u8( s ) ) {} };

struct Binary64 { u64 value; Binary64( u64 u ) : value( u ) {} Binary64( s64 s ) : value( s ) {} };
struct Binary32 { u32 value; Binary32( u32 u ) : value( u ) {} Binary32( s32 s ) : value( s ) {} Binary32( float f ) : value( *(u32*) &f ) {} };
struct Binary16 { u16 value; Binary16( u16 u ) : value( u ) {} Binary16( s16 s ) : value( s ) {} };
struct Binary8 { u8 value; Binary8( u8 u ) : value( u ) {} Binary8( s8 s ) : value( s ) {} };

struct FloatFormat
{
  FloatFormat( float f ) : value( f ), postPeriodDigits( 4 ) {}
  FloatFormat( float f, s32 postPeriodDigits ) : value( f ), postPeriodDigits( postPeriodDigits ) {}
  float value;
  s32 postPeriodDigits;
};

template<bool internal, typename Arg> INLINE s32 string_format( char* to, Arg value );

template<typename Arg, typename... Args>
INLINE s32 string_format( char* to, Arg arg, Args... args )
{
  s32 bytesWritten = string_format<true, Arg>( to, arg );
  bytesWritten += string_format<Args...>( to + bytesWritten, args... );

  return bytesWritten;
}

template<> INLINE s32 string_format<true, char const*>( char* to, char const* value );
template<> INLINE s32 string_format<true, char*>( char* to, char* value );
template<> INLINE s32 string_format<true, string>( char* to, string value );
template<> INLINE s32 string_format<true, u64>( char* to, u64 value );
template<> INLINE s32 string_format<true, u32>( char* to, u32 value );
template<> INLINE s32 string_format<true, u16>( char* to, u16 value );
template<> INLINE s32 string_format<true, u8>( char* to, u8 value );
template<> INLINE s32 string_format<true, s64>( char* to, s64 value );
template<> INLINE s32 string_format<true, s32>( char* to, s32 value );
template<> INLINE s32 string_format<true, s16>( char* to, s16 value );
template<> INLINE s32 string_format<true, s8>( char* to, s8 value );
template<> INLINE s32 string_format<true, Hex64>( char* to, Hex64 value );
template<> INLINE s32 string_format<true, Hex32>( char* to, Hex32 value );
template<> INLINE s32 string_format<true, Hex16>( char* to, Hex16 value );
template<> INLINE s32 string_format<true, Hex8>( char* to, Hex8 value );
template<> INLINE s32 string_format<true, Binary64>( char* to, Binary64 value );
template<> INLINE s32 string_format<true, Binary32>( char* to, Binary32 value );
template<> INLINE s32 string_format<true, Binary16>( char* to, Binary16 value );
template<> INLINE s32 string_format<true, Binary8>( char* to, Binary8 value );

s32 string_format_float( char* to, float value, s32 const postPeriodDigits );
template<> INLINE s32 string_format<true, float>( char* to, float value ) { return string_format_float( to, value, 4 ); }
template<> INLINE s32 string_format<true, FloatFormat>( char* to, FloatFormat value ) { return string_format_float( to, value.value, value.postPeriodDigits ); }

template<bool internal, typename Arg> INLINE s32 string_format( char* to, Arg value ) { static_assert(0); } //type doesn't exist for formatting yet, sorry :(

template<typename Arg> INLINE s32 string_format( char* to, Arg value )
{
  s32 bytesWritten = string_format<true, Arg>( to, value );
  to[bytesWritten] = '\0'; //end of format
  return bytesWritten + 1;
}

INLINE void string_copy( char* to, char const* from, s32 const size )
{
  for ( s32 i = 0; i < size; ++i )
  {
    *to++ = *from++;
  }
}

INLINE u32 string_match( char const* a, char const* b )
{
  while ( *a == *b )
  {
    if ( *a == '\0' ) return 1;
    ++a;
    ++b;
  }

  return 0;
}

//TODO untested
INLINE s32 string_first( char const* begin, s32 size, char const find )
{
  for ( s32 i = 0;i < size; ++i )
  {
    if ( *begin == find )
    {
      return i;
    }
  }

  return -1;
}

//TODO untested
INLINE s32 string_last( char const* begin, s32 size, char const find )
{
  begin += size;
  for ( s32 i = 0;i < size; ++i )
  {
    if ( *begin-- == find )
    {
      return size - i;
    }
  }

  return -1;
}

INLINE s32 string_length( char const* begin )
{
  s32 result = 0;
  while ( *begin++ != '\0' )
  {
    ++result;
  }
  return result;
}



s32 string_format_float( char* to, float value, s32 const postPeriodDigits )
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
    if ( bits & MASK_FIRST_BIT )
    {
      *to++ = '-';
      ++result;
    }

    if ( !(bits << 1) )
    {
      return result + string_format<true, char const*>( to, "0.0" );
    }

    if ( exponent == 128 )
    {
      return result + string_format<true, char const*>( to, "NaN" );
    }
  }

  //TODO
  constexpr s32 EXPONENT_LIMIT_MIN = -18;
  constexpr s32 EXPONENT_LIMIT_MAX = 23;
  if ( exponent > EXPONENT_LIMIT_MAX )
  {
    return result + string_format<true, char const*>( to, "bigfloat" );
  }
  else if ( exponent < EXPONENT_LIMIT_MIN )
  {
    return result + string_format<true, char const*>( to, "smlfloat" );
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
      s32 wholeResult = string_format<true, u32>( to, whole );
      to += wholeResult;
      result += wholeResult;
    }
    else
    {
      *to++ = '0';
      ++result;
    }

    *to++ = '.';
    ++result;

    return result + string_format<true, char const*>( to, tmpTo );
  }
}

template<> INLINE s32 string_format<true, char const*>( char* to, char const* value )
{
  s32 result = 0;
  if ( value )
  {
    while ( *value != '\0' )
    {
      *to++ = *value++;
      ++result;
    }
  }
  return result;
}

template<> INLINE s32 string_format<true, char*>( char* to, char* value )
{
  return string_format<true, char const*>( to, (char const*) value );
}


template<> INLINE s32 string_format<true, string>( char* to, string value )
{
  char const* reader = value.data;
  for ( s32 i = 0; i < value.capacity; ++i )
  {
    *to++ = *reader ? *reader : ' ';
    ++reader;
  }
  return value.capacity;
}


template<> INLINE s32 string_format<true, u64>( char* to, u64 value )
{
  if ( !value )
  {
    *to = '0';
    return 1;
  }

  s32 result = 0;
  u64 divisor = 10000000000000000000;
  while ( divisor )
  {
    u8 place = u8( value / divisor );
    if ( result || place )
    {
      value %= divisor;
      *to++ = '0' + place;
      ++result;
    }
    divisor /= 10;
  }
  return result;
}

template<> INLINE s32 string_format<true, u32>( char* to, u32 value )
{
  if ( !value )
  {
    *to = '0';
    return 1;
  }

  s32 result = 0;
  u32 divisor = 1000000000;
  while ( divisor )
  {
    u8 place = u8( value / divisor );
    if ( result || place )
    {
      value %= divisor;
      *to++ = '0' + place;
      ++result;
    }

    divisor /= 10;
  }
  return result;
}

template<> INLINE s32 string_format<true, u16>( char* to, u16 value )
{
  if ( !value )
  {
    *to = '0';
    return 1;
  }
  s32 result = 0;
  u32 divisor = 10000;
  while ( divisor )
  {
    u8 place = u8( value / divisor );
    if ( result || place )
    {
      value %= divisor;
      *to++ = '0' + place;
      ++result;
    }

    divisor /= 10;
  }

  return result;
}


template<> INLINE s32 string_format<true, u8>( char* to, u8 value )
{
  if ( !value )
  {
    *to = '0';
    return 1;
  }

  s32 result = 0;
  u32 divisor = 100;
  while ( divisor )
  {
    u8 place = u8( value / divisor );
    if ( result || place )
    {
      value %= divisor;
      *to++ = '0' + place;
      ++result;
    }

    divisor /= 10;
  }

  return result;
}

template<> INLINE s32 string_format<true, s64>( char* to, s64 value )
{
  s32 result = 0;
  if ( value >> ((sizeof( s64 ) * 8) - 1) )
  {
    *to++ = '-';
    value = -value;
    ++result;
  }
  return result + string_format<true, u64>( to, u64( value ) );
}

template<> INLINE s32 string_format<true, s32>( char* to, s32 value )
{
  s32 result = 0;
  if ( value >> ((sizeof( s32 ) * 8) - 1) )
  {
    *to++ = '-';
    value = -value;
    ++result;
  }
  return result + string_format<true, u32>( to, u32( value ) );
}

template<> INLINE s32 string_format<true, s16>( char* to, s16 value )
{
  s32 result = 0;
  if ( value >> ((sizeof( s16 ) * 8) - 1) )
  {
    *to++ = '-';
    value = -value;
    ++result;
  }
  return result + string_format<true, u16>( to, u16( value ) );
}

template<> INLINE s32 string_format<true, s8>( char* to, s8 value )
{
  s32 result = 0;
  if ( value >> ((sizeof( s8 ) * 8) - 1) )
  {
    *to++ = '-';
    value = -value;
    ++result;
  }
  return result + string_format<true, u8>( to, u8( value ) );
}

template<> INLINE s32 string_format<true, Hex64>( char* to, Hex64 value )
{
  constexpr u64 mask = 0xf000000000000000;
  *to++ = '0';
  *to++ = 'x';
  for ( int i = 0; i < 16; ++i )
  {
    u64 w = value.value & mask;
    w = w >> 60;
    value.value = value.value << 4;

    *to++ = w < 10 ? '0' + (u8) w : 'a' + (u8) w - 10;
  }
  return 18;
}

template<> INLINE s32 string_format<true, Hex32>( char* to, Hex32 value )
{
  constexpr u32 mask = 0xf0000000;
  *to++ = '0';
  *to++ = 'x';
  for ( int i = 0; i < 8; ++i )
  {
    u32 w = value.value & mask;
    w = w >> 28;
    value.value = value.value << 4;

    *to++ = w < 10 ? '0' + (u8) w : 'a' + (u8) w - 10;
  }
  return 10;
}


template<> INLINE s32 string_format<true, Hex16>( char* to, Hex16 value )
{
  constexpr u16 mask = 0xf000;
  *to++ = '0';
  *to++ = 'x';
  for ( int i = 0; i < 4; ++i )
  {
    u16 w = value.value & mask;
    w = w >> 12;
    value.value = value.value << 4;

    *to++ = w < 10 ? '0' + (u8) w : 'a' + (u8) w - 10;
  }
  return 6;
}

template<> INLINE s32 string_format<true, Hex8>( char* to, Hex8 value )
{
  constexpr u8 mask = 0xf0;
  *to++ = '0';
  *to++ = 'x';
  for ( int i = 0; i < 2; ++i )
  {
    u8 w = value.value & mask;
    w = w >> 4;
    value.value = value.value << 4;

    *to++ = w < 10 ? '0' + (u8) w : 'a' + (u8) w - 10;
  }
  return 4;
}

template<> INLINE s32 string_format<true, Binary64>( char* to, Binary64 value )
{
  u64 mask = 0b1000000000000000000000000000000000000000000000000000000000000000;
  *to++ = '0';
  *to++ = 'b';
  *to   = '0';
  s32 result = 2;
  for ( ; mask; mask >>= 1 ) if ( value.value & mask ) break;
  for ( ; mask; mask >>= 1 )
  {
    *to++ = '0' + bool( value.value & mask );
    ++result;
  }
  return max( 3, result );
}

template<> INLINE s32 string_format<true, Binary32>( char* to, Binary32 value )
{
  u32 mask = 0b10000000000000000000000000000000;
  *to++ = '0';
  *to++ = 'b';
  *to   = '0';
  s32 result = 2;
  for ( ; mask; mask >>= 1 )
  {
    *to++ = '0' + bool( value.value & mask );
    ++result;
  }
  return max( 3, result );
}


template<> INLINE s32 string_format<true, Binary16>( char* to, Binary16 value )
{
  u16 mask = 0b1000000000000000;
  *to++ = '0';
  *to++ = 'b';
  *to   = '0';
  s32 result = 2;
  for ( ; mask; mask >>= 1 ) if ( value.value & mask ) break;
  for ( ; mask; mask >>= 1 )
  {
    *to++ = '0' + bool( value.value & mask );
    ++result;
  }
  return max( 3, result );
}

template<> INLINE s32 string_format<true, Binary8>( char* to, Binary8 value )
{
  u8 mask = 0b10000000;
  *to++ = '0';
  *to++ = 'b';
  *to   = '0';
  s32 result = 2;
  for ( ; mask; mask >>= 1 )
  {
    *to++ = '0' + bool( value.value & mask );
    ++result;
  }
  return max( 3, result );
}
