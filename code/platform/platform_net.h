#pragma once

#define APP_CLIENT_COUNT_MAX 24


#include <core/bsnet.h>

namespace bs
{
  struct NetworkData
  {
    net::Connection self;
    net::Connection server;
    net::Connection connections[APP_CLIENT_COUNT_MAX];
    u32        connectionCount;
  };

  struct TCPSendParameter
  { //currently only file transfer? 
    net::Connection to;
    char const* fileData;
    u32         fileSize;
    char const* filename;
  };

  struct UDPSendParameter
  {
    net::Connection to;
    char const* packet;
    u32         packetSize;
  };

  struct UDPReceiveParameter
  {
    net::Connection sender;
    char const* packet;
    u32         packetSize;
    u32         id;
  };

  namespace platform
  {
    using send_tcp = void( TCPSendParameter const& );
    using send_udp = void( UDPSendParameter const& );
  };

};