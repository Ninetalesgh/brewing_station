#pragma once

#include "platform_callbacks.h"

#include <core/bsinput.h>
#include <core/bsnet.h>

namespace bs
{
  struct AppData
  {
    u64 currentFrameIndex;
    input::State input;
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