@echo off
rem Simple Windows batch file to bootstrap a build of dmb.exe

rem If you don't have MSVC set up in your path (i.e. with PATH, LIB
rem and INCLUDE set appropriately), then it does some simple
rem probing for MSVC and SDK locations.

setlocal

rem Some heuristics for finding MSVC
rem
if defined INCLUDE goto :done_setup_vc
  set vc_90=c:/Program Files/Microsoft Visual Studio 9.0
  set vc_7=c:/Program Files/Microsoft Visual Studio .NET 2003
  rem TODO add locations of other versions of vc here.

  set vcbase=
  set vc_cdir=
  if exist "%vc_90%" (
    set vcbase=%vc_90%
    set vc_cdir=%vc_90%/VC
    goto :done_checking_vc
  )
  if exist "%vc_7%" (
    set vcbase=%vc_7%
    set vc_cdir=%vc_7%/Vc7
    goto :done_checking_vc
  )
:done_checking_vc

  if not defined vcbase (
    echo Warning: couldn't find MSVC
    goto :done_setup_vc
  )
  echo MSVC detected at %vcbase%

  set sdk_base=
  call :check_sdk "c:/Program Files/Microsoft Platform SDK"
  call :check_sdk "c:/Program Files/Microsoft SDKs/Windows/v6.0A"
  call :check_sdk "c:/Program Files/Microsoft SDKs/Windows/v6.1"
  goto :end_check_sdk
:check_sdk
    if exist "%1" (
      set sdk_base=%~1
    )
    goto :eof
:end_check_sdk

  if not defined sdk_base (
    echo Warning: can't find Windows SDK
    goto :done_setup_vc
  )
:done_checking_sdk
  echo Windows SDK detected at %sdk_base%

  rem Set the environment variables for invoking MSVC
  set PATH=%PATH%;%vcbase%/Common7/IDE;%vc_cdir%/bin
  set INCLUDE=%vc_cdir%/include;%sdk_base%/include
  set LIB=%vc_cdir%/lib;%sdk_base%/lib

  rem echo PATH = %PATH%
  rem echo INCLUDE = %INCLUDE%
  rem echo LIB = %LIB%

:end_setup_vc


pushd %~dp0

mkdir dmb-out
mkdir dmb-out\bootstrap
cd dmb-out\bootstrap

rem Make lib_json.lib
cl -nologo -D_HAS_EXCEPTIONS=0 -c ../../jsoncpp/src/lib_json/*.cpp -I../../jsoncpp/include
lib -nologo -OUT:lib_json.lib json_reader.obj json_value.obj json_writer.obj

rem Make dmb.exe
set srcs=^
 ../../compile_util.cpp ^
 ../../config.cpp ^
 ../../context.cpp ^
 ../../dumbuild.cpp ^
 ../../eval.cpp ^
 ../../external_lib_target.cpp ^
 ../../exe_target.cpp ^
 ../../file_deps.cpp ^
 ../../hash.cpp ^
 ../../hash_util.cpp ^
 ../../lib_target.cpp ^
 ../../object.cpp ^
 ../../object_store.cpp ^
 ../../os.cpp ^
 ../../path.cpp ^
 ../../res.cpp ^
 ../../sha1.cpp ^
 ../../target.cpp ^
 ../../test.cpp ^
 ../../util.cpp 
cl -nologo %srcs% -Fedmb.exe -Zi -D_HAS_EXCEPTIONS=0 -I../../jsoncpp/include -link lib_json.lib -subsystem:console

popd
