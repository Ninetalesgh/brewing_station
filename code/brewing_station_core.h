
#include "core/bs_filesystem.h"

#include "platform/bs_platform.h"

//defaults and ease of use core systems

namespace bs
{
  [[nodiscard]]
  File* load_file( char const* path );
  bool unload_file( File* );
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//////////////////cpp//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


namespace bs
{
  File* load_file( char const* path ) { return load_file( bsp::platform->default.fileSystem, path ); }
  bool unload_file( File* file ) { return unload_file( bsp::platform->default.fileSystem, file ); }
};