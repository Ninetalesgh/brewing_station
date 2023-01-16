#pragma once

#include <platform/bs_platform.h>
#include <common/bsstring.h>
#include <common/bscommon.h>

namespace bs
{
  u64 hash_name64( char const* name )
  {
    u64 result = 0;
    s32 length = bs::string_length( name );

    char const* reader = name;
    char const* end = name + length;

    s32 offsetter = 0;
    while ( reader != end )
    {
      s32 i = 0;
      char buf[8] = {};
      while ( reader != end && i < 8 ) { buf[i++] = *reader++; }
      u64 batch = *(u64 const*) buf;

      result ^= ((batch + (offsetter++ * 37)) * 31);
    }

    return result;
  }

  u32 hash_name32( char const* name )
  {
    u32 result = 0;
    s32 length = bs::string_length( name );

    char const* reader = name;
    char const* end = name + length;

    s32 offsetter = 0;
    while ( reader != end )
    {
      s32 i = 0;
      char buf[4] = {};
      while ( reader != end && i < 4 ) { buf[i++] = *reader++; }
      u32 batch = *(u32 const*) buf;

      result ^= ((batch + (offsetter++ * 37)) * 31);
    }

    return result;
  }

  s32 binary_search( u32 const* hashes, s32 hashCount, u32 hashToFind )
  {
    s32 index = hashCount / 2;
    s32 top = hashCount;
    s32 bot = -1;
    while ( index != top )
    {
      if ( hashes[index] > hashToFind )
      {
        top = index;
        index -= (top - bot) / 2;
      }
      else if ( hashes[index] < hashToFind )
      {
        bot = index;
        index += (top - index + 1) / 2;
      }
      else
      {
        break;
      }
    }

    return index;
  }

  template<class Value> struct HashMap
  {
    HashMap()
    {

    }
    Value find( char const* const& key ) const
    {
      return *values;
    }

    Value& find( char const* const& key )
    {
      return *values;
    }

    void insert( s32 index, char const* name, u32 hash, Value value )
    {
      //
      if ( count < capacity )
      {
        for ( s32 i = count; i > index; --i )
        {
          //TODO split those?
          values[i] = values[i - 1];
          hashes[i] = hashes[i - 1];
          names[i] = names[i - 1];
        }

        values[index] = value;
        //TODO copy string over
        hashes[index] = hash;
        names[index] = name;
        ++count;
      }
    }

    Value  operator []( char const* key ) const { return find( key ); }
    Value& operator []( char const* key ) { return find( key ); }

    u32* hashes;
    char const** names;
    Value* values;
    s32 count;
    s32 capacity;

  };




};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////  cpp  /////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <random>

namespace bs
{

  using hash32_fn = u32( char const* );
  using hash64_fn = u64( char const* );
  void make_name( char* destination, s32 size )
  {
    //static const s32 range = '~' - '0';
    static const s32 range = '{' - '0';

    s32 randomSize = (rand() % (size - 2)) + 3;
    s32 i = 0;
    for ( ; i < randomSize; ++i )
    {
      char c = char( rand() % range ) + '0';
      while ( (c > '9' && c < 'A') || (c > 'Z' && c < 'a') )
      {
        c = char( rand() % range ) + '0';
      }

      destination[i] = c;
    }
    destination[randomSize] = '\0';
  }

  void hash_tester32( hash32_fn* hash_call, s32 const testBatchCount )
  {
    u32 MAX = 100000;
    s32 const max_name_length = 16;
    struct AAA
    {
      u32 hash;
      char name[max_name_length];
    };
    void* allocation = bsp::platform->allocate_to_zero( MAX * sizeof( AAA ) );

    AAA* entries = (AAA*) allocation;

    char buffer[max_name_length + 1] = { };
    char comparand[max_name_length + 1] = {};

    s32 totalCollisions = 0;
    s32 totalBatches = 0;

    srand( (u32) time( 0 ) );

    while ( totalBatches < testBatchCount )
    {
      s32 colCount = 0;
      log_info( "----------------------------------------------------------" );
      bool running = true;
      while ( running )
      {
        make_name( buffer, max_name_length );
        u32 hash = hash_call( buffer );
        u32 num = hash % MAX;
        u32 originalNum = num;

        while ( entries[num].hash )
        {
          if ( entries[num].hash == hash )
          {
            string_copy( comparand, entries[num].name, max_name_length );
            if ( !string_match( buffer, comparand ) )
            {
              log_info( buffer, "\n", comparand );
              log_info( "result in hash: ", hash );
              log_info( "---------------" );
              ++colCount;
            }
          }

          num = (num + 1) % MAX;
          if ( num == originalNum )
          {
            running = false;
            break;
          }
        }

        entries[num].hash = hash;
        string_copy( entries[num].name, buffer, max_name_length );
      }

      memset( allocation, 0, MAX * sizeof( AAA ) );
      totalCollisions +=  colCount;
      ++totalBatches;
    }
    float avgCollisions = (float) totalCollisions / (float) totalBatches;

    log_info( "=========================================================================================" );
    log_info( "=========================================================================================" );

    log_info( "Total Batches: ", totalBatches, " | Batch size: ", MAX );
    log_info( "Total collisions: ", totalCollisions, " | Average collisions per batch: ", avgCollisions );
    log_info( "Collision rate: ", float( totalCollisions ) * 100.0f / float( totalBatches * MAX ), "%%" );

    bsp::platform->free( allocation );
  }

  void hash_tester64( hash64_fn* hash_call, s32 const testBatchCount )
  {
    u32 MAX = 100000;
    s32 const max_name_length = 24;
    struct AAA
    {
      u64 hash;
      char name[max_name_length];
    };
    void* allocation = bsp::platform->allocate_to_zero( MAX * sizeof( AAA ) );

    AAA* entries = (AAA*) allocation;

    char buffer[max_name_length + 1] = { };
    char comparand[max_name_length + 1] = {};

    s32 totalCollisions = 0;
    s32 totalBatches = 0;

    srand( (u32) time( 0 ) );

    while ( totalBatches < testBatchCount )
    {
      s32 colCount = 0;
      log_info( "----------------------------------------------------------" );
      bool running = true;
      while ( running )
      {
        make_name( buffer, max_name_length );
        u64 hash = hash_call( buffer );
        u64 num = hash % MAX;
        u64 originalNum = num;

        while ( entries[num].hash )
        {
          if ( entries[num].hash == hash )
          {
            string_copy( comparand, entries[num].name, max_name_length );
            if ( !string_match( buffer, comparand ) )
            {
              log_info( buffer, "\n", comparand );
              log_info( "result in hash: ", hash );
              log_info( "---------------" );
              ++colCount;
            }
          }

          num = (num + 1) % MAX;
          if ( num == originalNum )
          {
            running = false;
            break;
          }
        }

        entries[num].hash = hash;
        string_copy( entries[num].name, buffer, max_name_length );
      }

      memset( allocation, 0, MAX * sizeof( AAA ) );
      totalCollisions +=  colCount;
      ++totalBatches;
    }
    float avgCollisions = (float) totalCollisions / (float) totalBatches;

    log_info( "=========================================================================================" );
    log_info( "=========================================================================================" );

    log_info( "Total Batches: ", totalBatches, " | Batch size: ", MAX );
    log_info( "Total collisions: ", totalCollisions, " | Average collisions per batch: ", avgCollisions );
    log_info( "Collision rate: ", float( totalCollisions ) * 100.0f / float( totalBatches * MAX ), "%%" );

    bsp::platform->free( allocation );
  }

};