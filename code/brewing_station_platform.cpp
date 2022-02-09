#define UNICODE


#include "common/bscommon.h"

s32 brewing_station_init();
void brewing_station_run();


int main( int argc, char** argv )
{
  if ( brewing_station_init() )
  {
    brewing_station_run();
  }

  return 0;
}

#ifdef _WIN32
#include "win32/win32_brewing_station.cpp"
#endif