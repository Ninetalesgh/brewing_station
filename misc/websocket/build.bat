@echo off

set build_mode="%1"

set root_dir=..\..\

IF NOT EXIST %root_dir%build mkdir %root_dir%build
pushd %root_dir%build

set codepath=%root_dir%code\apps_standalone\websocket\

set compiler_options=/I %codepath% /GR- /EHa- /FC /MT /nologo /volatile:iso /W4 /wd4068 /wd4100 /wd4201 /wd4701
set linker_options=/link /opt:ref /incremental:no

IF %build_mode%=="develop" goto build_develop

:build_develop
  echo ------------------------------------
  echo ------------- develop --------------
  echo ------------------------------------
  IF NOT EXIST dev mkdir dev
  pushd dev
  del *.pdb > NUL 2> NUL

  set compiler_options_dev=%compiler_options% /wd4189 /wd4505 /Z7 /Od /DBS_DEBUG

 cl     %codepath%win32_websocket_main.cpp /I %root_dir%code /Fe:bs_develop.exe %compiler_options_dev% %linker_options% 

  echo ------------------------------------
  
  popd
popd