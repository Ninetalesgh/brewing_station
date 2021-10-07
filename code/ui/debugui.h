#pragma once


#include <common/memory.h>

namespace ui
{
  namespace debug
  {
    struct DebugDisplay
    {


      memory::debug::ArenaObserver arenaObserver;
    };

    struct DebugDisplayInitParameter
    {
      memory::Arena* mainArena;
    };
    DebugDisplay* init_debug_display( memory::Arena* arena, DebugDisplayInitParameter* );
  };

};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////



namespace ui
{
  namespace debug
  {
    DebugDisplay* init_debug_display( memory::Arena* arena, DebugDisplayInitParameter* initParameter )
    {
      return nullptr;
      //  DebugDisplay* result = (DebugDisplay*) arena->alloc_set_zero( sizeof( ui::debug::DebugDisplay ) );
        //result->mainArena = initParameter->mainArena;
    }

  };
};