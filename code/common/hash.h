#pragma once
#include "basic_types.h"

struct hash64
{
  hash64() {}
  hash64( u64 value ) : value( value ) {}
  hash64( char const* name )
  {
    char tmp[8] = {};
    char* writer = tmp;
    int  i= 0;
    while ( i++ < 8 && *name != '\0' )
    {
      *writer++ = *name++;
    }
    value = *(u64*) tmp;
  }
  u64 value;

  INLINE u32 operator ==( hash64 const& other ) { return value == other.value; }
  INLINE u32 operator !=( hash64 const& other ) { return value != other.value; }
};

struct hash32
{
  hash32() {}
  hash32( u32 value ) : value( value ) {}
  hash32( char const* name )
  {
    char tmp[4] = {};
    char* writer = tmp;
    int  i= 0;
    while ( i++ < 4 && *name != '\0' )
    {
      *writer++ = *name++;
    }
    value = *(u32*) tmp;
  }
  u64 value;

  INLINE u32 operator ==( hash32 const& other ) { return value == other.value; }
  INLINE u32 operator !=( hash32 const& other ) { return value != other.value; }
};