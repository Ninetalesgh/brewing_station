#define BUILD_TESTAPP

#ifdef BUILD_TESTAPP
#include "testapp/testapp_main.cpp"
#else

#endif




#include <core/internal/bsfile.cpp>
#include <core/internal/bstask.cpp>
#include <core/internal/bsnet.cpp>


#include <platform/platform_callbacks_internal.h>
namespace platform
{
  callbackfunctionsignature::debug_log* ptr_debug_log;
  callbackfunctionsignature::push_async_task* ptr_push_async_task;
  callbackfunctionsignature::push_synced_task* ptr_push_synced_task;
  callbackfunctionsignature::complete_synced_tasks* ptr_complete_synced_tasks;
  callbackfunctionsignature::get_file_info* ptr_get_file_info;
  callbackfunctionsignature::read_file* ptr_read_file;
  callbackfunctionsignature::write_file* ptr_write_file;
  callbackfunctionsignature::free_file* ptr_free_file;
  callbackfunctionsignature::send_udp* ptr_send_udp;
  callbackfunctionsignature::send_tcp* ptr_send_tcp;
  bs::memory::Arena* mainArena;

  void register_callbacks( platform::PrmRegisterCallbacks prm )
  {
    ptr_debug_log             = prm.debug_log;
    ptr_push_async_task       = prm.push_async_task;
    ptr_push_synced_task      = prm.push_synced_task;
    ptr_complete_synced_tasks = prm.complete_synced_tasks;
    ptr_get_file_info         = prm.get_file_info;
    ptr_read_file             = prm.read_file;
    ptr_write_file            = prm.write_file;
    ptr_free_file             = prm.free_file;
    ptr_send_udp              = prm.send_udp;
    ptr_send_tcp              = prm.send_tcp;
    mainArena                 = prm.mainArena;
  }
};


