#define BUILD_TESTAPP



#ifdef BS_DEBUG
#include <app_common/platform_debug.h>
void platform::debug::register_debug_callbacks( PrmRegisterDebugCallbacks prm )
{
  platform::debug::global::ptr_debug_log = prm.debug_log;
}

#endif







#ifdef BUILD_TESTAPP
#include "testapp/testapp_main.cpp"
#else

#endif



