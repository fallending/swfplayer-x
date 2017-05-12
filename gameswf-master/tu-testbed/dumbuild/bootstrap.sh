#!/bin/sh

# Simple build script to bootstrap a build of dmb.exe
#
# This script should work on Linux and other posix systems using gcc.
#
# It should also work on cygwin if you have MSVC set up in your path.

sysname=`uname`

sources="../../compile_util.cpp \
    ../../config.cpp \
    ../../context.cpp \
    ../../dumbuild.cpp \
    ../../eval.cpp \
    ../../external_lib_target.cpp \
    ../../exe_target.cpp \
    ../../file_deps.cpp \
    ../../hash.cpp \
    ../../hash_util.cpp \
    ../../lib_target.cpp \
    ../../object.cpp \
    ../../object_store.cpp \
    ../../os.cpp \
    ../../path.cpp \
    ../../res.cpp \
    ../../sha1.cpp \
    ../../target.cpp \
    ../../test.cpp \
    ../../util.cpp"

# cygwin with MSVC:
if echo "$sysname" | grep -q 'CYGWIN' ; then
  mkdir -p dmb-out/bootstrap
  cd dmb-out/bootstrap

  # Make lib_json.lib
  cl -c -GX ../../jsoncpp/src/lib_json/*.cpp -I../../jsoncpp/include
  lib /OUT:lib_json.lib json_reader.obj json_value.obj json_writer.obj

  # Make dmb.exe
  cl -Fedmb.exe ${sources} \
    -Zi -GX -I../../jsoncpp/include -link lib_json.lib -subsystem:console

  cd ../..

else
  # Non-windows.  Assume gcc is going to work.

  mkdir -p dmb-out/bootstrap
  cd dmb-out/bootstrap

  # Make lib_json.a
  echo Compiling lib_json...
  gcc -c -g ../../jsoncpp/src/lib_json/*.cpp -I../../jsoncpp/include
  echo ar lib_json...
  ar rc lib_json.a json_reader.o json_value.o json_writer.o

  # Make dmb executable
  echo Compiling and linking...
  gcc -o dmb ${sources} \
    -g -Wall -I../../jsoncpp/include lib_json.a -lstdc++

fi
