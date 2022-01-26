#pragma once

#define APP_CLIENT_COUNT_MAX 24


#include <common/net.h>

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
using fn_send_udp = void( UDPSendParameter const& );

struct UDPReceiveParameter
{
  net::Connection sender;
  char const* packet;
  u32         packetSize;
  u32         id;
};
using fn_send_tcp = void( TCPSendParameter const& );
