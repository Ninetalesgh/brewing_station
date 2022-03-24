#pragma once

#include "platform_callbacks.h"
#include "platform_input.h"

#include <core/bsnet.h>
#include <core/bsmemory.h>

namespace bs
{
  struct AppData
  {
    memory::Arena mainArena;

    u64 currentFrameIndex;
    Input input;
    net::NetworkData network;
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

  struct PrmAppRender
  {
    //PlatformData const* platformData;
    AppData* appData;
    //BackBuffer* backBuffer;
  };
  extern "C" void app_render( PrmAppRender );

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

};