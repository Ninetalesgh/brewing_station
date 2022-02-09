#pragma once

#include "platform_net.h"
#include "platform_input.h"
#include "platform_file.h"

#include <core/memory.h>

namespace bs
{
  struct AppData
  {
    memory::Arena mainArena;

    u64 currentFrameIndex;
    Input input;
    NetworkData network;

    fn_get_file_info* get_file_info;
    fn_read_file* read_file;
    fn_write_file* write_file;
    fn_free_file* free_file;
    fn_send_udp* send_udp;
    fn_send_tcp* send_tcp;
  };

  struct AppSampleSoundParameter
  {
    AppData* appData;
    //SoundBuffer* soundBuffer;
  };
  extern "C" void app_sample_sound( AppSampleSoundParameter& );

  struct AppOnLoadParameter
  {
    //PlatformData const* platformData;
    AppData* appData;
  };
  extern "C" void app_on_load( AppOnLoadParameter& );

  struct AppTickParameter
  {
    //PlatformData const* platformData;
    AppData* appData;
    //BackBuffer* backBuffer;
  };
  extern "C" void app_tick( AppTickParameter& );

  struct AppRenderParameter
  {
    //PlatformData const* platformData;
    AppData* appData;
    //BackBuffer* backBuffer;
  };
  extern "C" void app_render( AppRenderParameter& );

  struct AppReceiveUDPPacketParameter
  {
    //PlatformData const* platformData;
    AppData* appData;
    net::Connection sender;
    char const* packet;
    u32         packetSize;
    u32         id;
  };
  extern "C" void app_receive_udp_packet( AppReceiveUDPPacketParameter& );

};