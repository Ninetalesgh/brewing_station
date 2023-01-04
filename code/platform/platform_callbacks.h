#pragma once

DEPRECATED

#include <core/bsdebuglog.h>
#include <core/bsgraphics.h>
#include <core/bsnet.h>
#include <core/bsfile.h>
#include <core/bstask.h>

namespace platform
{
  //debug log callback. use defines in bsdebuglog for ease of use
  void debug_log( bs::debug::DebugLogFlags, char const* string, s32 size );

  ///////////////////////////////////////////////////////////////////////////////
  //networking
  ///////////////////////////////////////////////////////////////////////////////

  //TODO//////
  void send_tcp( bs::net::TCPSendParameter const& );
  void send_udp( bs::net::UDPSendParameter const& );

  ///////////////////////////////////////////////////////////////////////////////
  //file io
  ///////////////////////////////////////////////////////////////////////////////

  //returns 0 on fail
  u32 get_file_info( char const* filePath, bs::file::Info* out_fileInfo );

  // returns 0 on fail
  // the allocated memory has to be managed manually
  u32 load_file_into_memory( char const* filePath, bs::file::Data* out_loadedFileData );

  // returns 0 on fail
  u32 write_file( char const* filePath, void const* data, u32 size );

  ///////////////////////////////////////////////////////////////////////////////
  //threading
  ///////////////////////////////////////////////////////////////////////////////

  //returns 0 on fail
  //pushes a task onto the asynchronous task queue, if you want to keep track of it, use the task state 
  u32 push_async_task( bs::Task const&, bs::TaskState volatile* out_taskState = nullptr );

  //returns 0 on fail
  //pushes a task onto the synchronous task queue, which can be collectively waited for with complete_synced_tasks();
  u32 push_synced_task( bs::Task const&, bs::TaskState volatile* out_taskState = nullptr );

  //wait for all tasks in the synchronous task queue to finish. 
  void complete_synced_tasks();

  ///////////////////////////////////////////////////////////////////////////////
  //memory
  ///////////////////////////////////////////////////////////////////////////////

  //TODO alternative allocators from the mainArena?
  void* allocate( s64 size );
  void* allocate_to_zero( s64 size );
  void free( void* ptr );

  ///////////////////////////////////////////////////////////////////////////////
  //graphics
  ///////////////////////////////////////////////////////////////////////////////

  bs::graphics::TextureID allocate_texture( bs::graphics::TextureData const* );
  void free_texture( bs::graphics::TextureID );

  bs::graphics::Mesh allocate_mesh( bs::graphics::MeshData const* );
  void free_mesh( bs::graphics::Mesh );

  void render( bs::graphics::RenderTarget*, bs::graphics::RenderGroup*, bs::graphics::Camera* );

};