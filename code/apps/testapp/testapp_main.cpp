#include <platform/platform.h>

namespace bs
{
  void app_sample_sound( PrmAppSampleSound prm )
  {
    BREAK;
  }

  void app_on_load( PrmAppOnLoad prm )
  {
    //bs::AppData* appData = prm.appData;

    char test2[64] = {};
    char* test = (char*) memory::allocate( 64 );
    memory::set( test, 1, 64 );
    memory::copy( test, test2, 64 );
    log_info( "test", string_length( "hello" ), string_length( "" ) );

    bs::file::Data ttf;
    if ( !platform::load_file_into_memory( "w:/data/bs.ttf", &ttf ) )
    {
      BREAK;
    }

    //u32 get_file_info( char const* filename, bs::FileInfo* out_FileInfo );
   // bs::ReadFileResult read_file( char const* filename, u32 maxSize, void* out_data );
   // void free_file( void* filename );
   // u32 write_file( bs::WriteFileParameter const& );

  //    platform::read_file( "w:/data/Inconsolata.ttf" );

  }

  void app_tick( PrmAppTick prm )
  {

    log_info( "tkkkest", "lol" );

    // BREAK;

  }
  void app_render( PrmAppRender prm )
  {
    BREAK;
  }
  void app_receive_udp_packet( PrmAppReceiveUDPPacket prm )
  {
    BREAK;
  }
};

