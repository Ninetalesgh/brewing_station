#define UNICODE

void brewing_station_main();

int main( int argc, char** argv )
{
  brewing_station_main();

  return 0;
}

#ifdef _WIN32
#include <platform/win32/win32_brewing_station.cpp>
#endif