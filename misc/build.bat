@echo off

set build_mode="%1"

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

set codepath=..\..\code

set compiler_options=/I %codepath% /GR- /EHa- /FC /MT /nologo /volatile:iso /W4 /wd4068 /wd4100 /wd4201 /wd4701
set linker_options=/link /opt:ref /incremental:no

set app_exports=/EXPORT:app_sample_sound /EXPORT:app_tick /EXPORT:app_receive_udp_packet /EXPORT:app_on_load /EXPORT:register_callbacks

IF %build_mode%=="develop" goto build_develop
IF %build_mode%=="release" goto build_release

:build_develop
  echo ------------------------------------
  echo ------------- develop --------------
  echo ------------------------------------
  IF NOT EXIST dev mkdir dev
  pushd dev
  del *.pdb > NUL 2> NUL

  set compiler_options_dev=%compiler_options% /Z7 /Od /DBS_DEBUG

  cl /LD %codepath%\apps\brewing_station_app.cpp /I %codepath%\apps %compiler_options_dev% /Fe:brewing_station_app.dll /Fmbrewing_station_app.map %linker_options% %app_exports% /PDB:bs_app_%random%.pdb 
  cl     %codepath%\brewing_station_main.cpp /Fe:brewing_station.exe %compiler_options_dev% %linker_options% 

  echo ------------------------------------
  popd
 IF %build_mode%=="develop" goto end


:build_release
  echo ------------- release --------------
  echo ------------------------------------
  IF NOT EXIST release mkdir release
  pushd release

  set compiler_options_release=%compiler_options% /Ox /DBS_RELEASE_BUILD /DBS_DEBUG
  
  cl %codepath%\brewing_station_main.cpp /Fe:brewing_station.exe %compiler_options_release% %linker_options% 
  echo ------------------------------------

  del *.obj > NUL 2> NUL
  popd
  IF %build_mode%=="release" goto end


:end
popd