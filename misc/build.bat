@echo off

set build_mode="%1"

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

set codepath=..\..\code

set compiler_options=/I %codepath% /GR- /EHa- /FC /MT /nologo /W4 /wd4068 /wd4100 /wd4201 /wd4701
set linker_options=/link /opt:ref /incremental:no

set app_exports=/EXPORT:app_sample_sound /EXPORT:app_tick /EXPORT:app_receive_udp_packet /EXPORT:app_on_load /EXPORT:app_render

  echo -----------------------------------------
  echo ----------------- debug -----------------
  echo -----------------------------------------
  IF NOT EXIST debug mkdir debug
  pushd debug
  del *.pdb > NUL 2> NUL

  set compiler_options_debug=%compiler_options% /Z7 /Od /DBS_BUILD_DEBUG=1 /DBS_BUILD_DEVELOP=0 /DBS_BUILD_RELEASE=0 
  
  cl     %codepath%\win32_entry_brewing_station.cpp /Fe:brewing_station_raw.exe %compiler_options_debug% %linker_options% 

popd
popd