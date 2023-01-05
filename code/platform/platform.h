DEPRECATED

#pragma once

#include "bs_platform.h"

#include <core/bsinput.h>
#include <core/bsnet.h>


#define DEFAULT_WINDOW_SIZE int2{ 1024, 780 }

namespace bs
{




  struct AppData
  {
    u64 currentFrameIndex;
    float deltaTime;
    input::State input;
    net::NetworkData network;
    void* userData;
  };

  struct PrmAppSampleSound
  {
    AppData* appData;
    //SoundBuffer* soundBuffer;
  };
  extern "C" void app_sample_sound( PrmAppSampleSound );

  struct PrmAppOnLoad
  {
    //PlatformData const* platformData;
    AppData* appData;
  };
  extern "C" void app_on_load( PrmAppOnLoad );

  struct PrmAppTick
  {
    //PlatformData const* platformData;
    AppData* appData;
    //BackBuffer* backBuffer;
  };
  extern "C" void app_tick( PrmAppTick );

  struct PrmAppReceiveUDPPacket
  {
    //PlatformData const* platformData;
    AppData* appData;
    net::Connection sender;
    char const* packet;
    u32         packetSize;
    u32         id;
  };
  extern "C" void app_receive_udp_packet( PrmAppReceiveUDPPacket );




  extern "C" void register_callbacks( bs::Platform* );

};