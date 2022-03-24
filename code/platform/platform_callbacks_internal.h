#pragma once

#include "platform_callbacks.h"

namespace platform
{
  struct PrmRegisterCallbacks
  {
    platform::callbackfunctionsignature::debug_log* debug_log;
    platform::callbackfunctionsignature::push_async_task* push_async_task;
    platform::callbackfunctionsignature::push_synced_task* push_synced_task;
    platform::callbackfunctionsignature::complete_synced_tasks* complete_synced_tasks;
    platform::callbackfunctionsignature::get_file_info* get_file_info;
    platform::callbackfunctionsignature::read_file* read_file;
    platform::callbackfunctionsignature::write_file* write_file;
    platform::callbackfunctionsignature::free_file* free_file;
    platform::callbackfunctionsignature::send_udp* send_udp;
    platform::callbackfunctionsignature::send_tcp* send_tcp;
  };
};

extern "C" void register_callbacks( platform::PrmRegisterCallbacks );