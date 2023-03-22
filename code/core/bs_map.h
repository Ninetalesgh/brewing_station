#pragma once

#include <module/bs_allocator.h>
#include <common/bs_string.h>
#include <common/bs_common.h>

namespace bs
{
  u32 hash_name32( char const* name );
  u64 hash_name64( char const* name );

  //returns the index of the slot this hash is in or should be in
  s32 binary_search( u32 const* hashes, s32 hashCount, u32 hashToFind );


  //allocate, deallocate, construct, destroy


  template <class ValueType> class HashTable32
  {



  };

  template<class Value> struct HashMap32
  {
    HashMap32()
      : hashes( nullptr )
      , values( nullptr )
      , count( 0 )
      , capacity( 0 )
    {}

    bool contains_key( char const* name );
    bool contains_key( u32 hash );

    Value& add_key_value_pair( char const* name, Value value );
    Value& add_key_value_pair( u32 hash, Value value );

    Value& fetch_or_add_entry( u32 hash );

    void insert( s32 index, char const* name, u32 hash, Value value );

    void allocate_containers();

    Value& operator []( char const* key );

    u32* hashes;
    Value* values;
    s32 count;
    s32 capacity;
  };


  //TODO permanent string allocator
  //TODO multi-frame 






  struct HashRegister64
  {

  };

  struct HashRegister32
  {
    u32* hashes;
    s32* nameIndices;
    char* names;
    s32 count;
    s32 capacity;
    s32 nameCapacity;
  };

  [[nodiscard]]
  HashRegister32* create_hash_register_32( s32 startCapacity, s32 avgNameLengthEstimate = 5 )
  {
    HashRegister32* result = nullptr;
    s32 nameCapacity = startCapacity * avgNameLengthEstimate;
    s32 allocationSize = sizeof( HashRegister32 ) + (sizeof( u32 ) + sizeof( s32 )) * startCapacity + nameCapacity;
    char* allocation = (char*) bsm::allocate( bsp::platform->default.allocator, (s64) allocationSize );
    if ( allocation )
    {
      result               = (HashRegister32*) result;
      result->hashes       = (u32*) (allocation + sizeof( HashRegister32 ));
      result->nameIndices  = (s32*) (allocation + sizeof( HashRegister32 ) + sizeof( u32 ) * startCapacity);
      result->names        = (char*) (allocation + sizeof( HashRegister32 ) + (sizeof( u32 ) + sizeof( s32 )) * startCapacity);
      result->capacity     = startCapacity;
      result->nameCapacity = nameCapacity;
    }
    else
    {
      BREAK;
    }

    return result;
  }


};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////  inl  /////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace bs
{

  template<class Value> bool HashMap32<Value>::contains_key( char const* name )
  {
    return contains_key( hash_name32( name ) );
  }

  template<class Value> bool HashMap32<Value>::contains_key( u32 hash )
  {
    return hash == binary_search( hashes, count, hashToFind );
  }

  template<class Value> Value& HashMap32<Value>::add_key_value_pair( char const* name, Value value )
  {
    return add_key_value_pair( hash_name32( name ) );
  }

  template<class Value> Value& HashMap32<Value>::add_key_value_pair( u32 hash, Value value )
  {
    s32 bestIndex = binary_search( hashes, count, hashToFind );
  }

  template<class Value> void HashMap32<Value>::insert( s32 index, char const* name, u32 hash, Value value )
  {
    if ( count < capacity )
    {
      for ( s32 i = count; i > index; --i )
      {
        values[i] = values[i - 1];
      }

      for ( s32 i = count; i > index; --i )
      {
        hashes[i] = hashes[i - 1];
      }

      values[index] = value;
      hashes[index] = hash;
      ++count;
    }
    else
    {
      BREAK; //TODO
    }
  }

  template<class Value> void HashMap32<Value>::allocate_containers()
  {
    int i = 0;
    i = 0;
  }

  template<class Value> Value& HashMap32<Value>::fetch_or_add_entry( u32 hash )
  {
    s32 newHash = hash_name32( key );
    s32 bestIndex = binary_search( hashes, count, hashToFind );

    if ( )
      return values[binary_search( hashes, count, hashToFind )];

  }

  template<class Value> Value& HashMap32<Value>::operator[]( char const* key )
  {

    return values[binary_search( hashes, count, hashToFind )];
  }


};




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////  cpp  /////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <random>

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
    void* allocation = bsm::allocate_to_zero( bsp::platform->default.allocator, MAX * sizeof( AAA ) );

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

    bsm::free( bsp::platform->default.allocator, allocation );
  }

  void hash_tester64( hash64_fn* hash_call, s32 const testBatchCount )
  {
    u32 MAX = 100000;
    s32 const max_name_length = 40;
    struct AAA
    {
      u64 hash;
      char name[max_name_length];
    };
    void* allocation = bsm::allocate_to_zero( bsp::platform->default.allocator, MAX * sizeof( AAA ) );

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

    bsm::free( bsp::platform->default.allocator, allocation );
  }

};