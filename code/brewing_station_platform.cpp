#define UNICODE


#include "common/bscommon.h"

void brewing_station_main();

int main( int argc, char** argv )
{
  brewing_station_main();

  return 0;
}

#ifdef _WIN32
#include "win32/win32_brewing_station.cpp"
#endif