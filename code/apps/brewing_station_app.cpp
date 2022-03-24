#define BUILD_TESTAPP

#ifdef BUILD_TESTAPP
#include "testapp/testapp_main.cpp"
#else

#endif


#include <platform/platform_callbacks_internal.h>
namespace platform
{
  callbackfunctionsignature::debug_log* debug_log;
  callbackfunctionsignature::push_async_task* push_async_task;
  callbackfunctionsignature::push_synced_task* push_synced_task;
  callbackfunctionsignature::complete_synced_tasks* complete_synced_tasks;
  callbackfunctionsignature::get_file_info* get_file_info;
  callbackfunctionsignature::read_file* read_file;
  callbackfunctionsignature::write_file* write_file;
  callbackfunctionsignature::free_file* free_file;
  callbackfunctionsignature::send_udp* send_udp;
  callbackfunctionsignature::send_tcp* send_tcp;

  void register_callbacks( platform::PrmRegisterCallbacks prm )
  {
    debug_log             = prm.debug_log;
    push_async_task       = prm.push_async_task;
    push_synced_task      = prm.push_synced_task;
    complete_synced_tasks = prm.complete_synced_tasks;
    get_file_info         = prm.get_file_info;
    read_file             = prm.read_file;
    write_file            = prm.write_file;
    free_file             = prm.free_file;
    send_udp              = prm.send_udp;
    send_tcp              = prm.send_tcp;
  }
};


