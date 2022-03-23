#pragma once

#include "platform_net.h"
#include "platform_input.h"
#include "platform_file.h"
#include "platform_thread.h"
#include "platform_debug.h"

#include <core/bsmemory.h>

namespace bs
{
  struct AppData
  {
    memory::Arena mainArena;

    u64 currentFrameIndex;
    Input input;
    NetworkData network;

    //task scheduling
    platform::push_async_task* push_async_task;
    platform::push_synced_task* push_synced_task;
    platform::complete_synced_tasks* complete_synced_tasks;
    //file
    platform::get_file_info* get_file_info;
    platform::read_file* read_file;
    platform::write_file* write_file;
    platform::free_file* free_file;
    //networking
    platform::send_udp* send_udp;
    platform::send_tcp* send_tcp;
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