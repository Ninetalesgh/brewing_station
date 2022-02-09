@echo off

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

set codepath=..\..\code

set compiler_options=/I %codepath% /GR- /EHa- /FC /MT /nologo /W4 /wd4068 /wd4100 /wd4201 /wd4701
set linker_options=/link /opt:ref /incremental:no

set app_exports=/EXPORT:app_sample_sound /EXPORT:app_tick /EXPORT:app_receive_udp_packet /EXPORT:app_on_load /EXPORT:app_render

  echo -------------- debug --------------
  echo -----------------------------------
  IF NOT EXIST debug mkdir debug
  pushd debug
  del *.pdb > NUL 2> NUL

  set compiler_options_debug=%compiler_options% /Z7 /Od /DBS_DEBUG=1

  echo -----------------------------------
  cl /LD %codepath%\apps\brewing_station_app.cpp /I %codepath%\apps %compiler_options_debug% /Fe:brewing_station_app.dll /Fmbrewing_station_app.map %linker_options% %app_exports% /PDB:bs_app_%random%.pdb 
 
  echo -----------------------------------
  cl     %codepath%\brewing_station_platform.cpp /Fe:brewing_station.exe %compiler_options_debug% %linker_options% 

  echo -----------------------------------
popd
popd