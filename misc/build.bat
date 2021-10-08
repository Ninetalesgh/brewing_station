@echo off

set build_mode="%1"

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build


set codepath=..\..\code

set compiler_options=/I %codepath% /GR- /EHa- /FC /MT /nologo /W4 /wd4068 /wd4100 /wd4201 /wd4701
set linker_options=/link /opt:ref /incremental:no

set app_exports=/EXPORT:app_sample_sound /EXPORT:app_tick /EXPORT:app_receive_udp_packet /EXPORT:app_on_load /EXPORT:app_render


IF %build_mode%=="debug" goto build_debug
IF %build_mode%=="develop" goto build_develop
IF %build_mode%=="release" goto build_release

:build_debug
  echo -----------------------------------------
  echo ----------------- debug -----------------
  echo -----------------------------------------
  IF NOT EXIST debug mkdir debug
  pushd debug
  del *.pdb > NUL 2> NUL

  set compiler_options_debug=%compiler_options% /Z7 /Od /DBS_BUILD_DEBUG=1 /DBS_BUILD_DEVELOP=0 /DBS_BUILD_RELEASE=0 
  
  cl /LD %codepath%\apps\scifi.cpp %compiler_options_debug% /Fmscifi.map %linker_options% %app_exports% /PDB:scifi_%random%.pdb 
  cl     %codepath%\win32_main.cpp  /Fe:scifi_debug.exe %compiler_options_debug% %linker_options% 
  cl     %codepath%\win32_server.cpp /Fe:server_debug.exe %compiler_options_debug% %linker_options% 

  popd
  IF %build_mode%=="debug" goto end

:build_develop
  echo -----------------------------------------
  echo ---------------- develop ----------------
  echo -----------------------------------------
  IF NOT EXIST develop mkdir develop
  pushd develop

  set compiler_options_develop=%compiler_options% /Ox /DBS_BUILD_DEBUG=0 /DBS_BUILD_DEVELOP=1 /DBS_BUILD_RELEASE=0

  cl /LD %codepath%\apps\scifi.cpp %compiler_options_develop% %linker_options% %app_exports%
  cl     %codepath%\win32_main.cpp  /Fe:scifi_develop.exe %compiler_options_develop% %linker_options% 
  cl     %codepath%\win32_server.cpp /Fe:server_develop.exe %compiler_options_develop% %linker_options% 

  del *.obj
  del *.exp
  del *.lib
  popd
  IF %build_mode%=="develop" goto end

:build_release
  echo -----------------------------------------
  echo ---------------- release ----------------
  echo -----------------------------------------
  IF NOT EXIST release mkdir release
  pushd release

  set compiler_options_release=%compiler_options% /Ox /DBS_BUILD_DEBUG=0 /DBS_BUILD_DEVELOP=0 /DBS_BUILD_RELEASE=1
  
  cl %codepath%\win32_main.cpp  /Fe:scifi.exe %compiler_options_release% %linker_options% 
  cl %codepath%\win32_server.cpp /Fe:server.exe %compiler_options_release% %linker_options% 

  del *.obj
  popd
  IF %build_mode%=="release" goto end

:end
popd