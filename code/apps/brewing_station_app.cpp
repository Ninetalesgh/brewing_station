


#ifdef BS_DEBUG
#include <platform/platform_debug.h>
void platform::debug::register_debug_callbacks( PrmRegisterDebugCallbacks prm )
{
  platform::debug::global::ptr_debug_log = prm.debug_log;
}

#endif



#define BUILD_TESTAPP

#ifdef BUILD_TESTAPP
#include "testapp/testapp_main.cpp"
#else

#endif



