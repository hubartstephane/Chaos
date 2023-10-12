#! /usr/bin/bash

SCRIPT_PATH=$(pwd)/$0
SCRIPT_PATH=$(dirname $SCRIPT_PATH)

#unzip $SCRIPT_PATH/external_zip/FreeImage3180Win32Win64.zip -d $SCRIPT_PATH
#unzip $SCRIPT_PATH/external_zip/glew-2.2.0-win32.zip -d $SCRIPT_PATH
#unzip $SCRIPT_PATH/external_zip/irrKlang-64bit-1.6.0.zip -d $SCRIPT_PATH
#unzip $SCRIPT_PATH/external_zip/lua-5.4.2_Win64_vc16_lib -d $SCRIPT_PATH/lua-5.4.2
#unzip $SCRIPT_PATH/external_zip/ImGui -d $SCRIPT_PATH
#unzip $SCRIPT_PATH/external_zip/MSVC_Redist.zip -d $SCRIPT_PATH

#gunzip --stdout $SCRIPT_PATH/external_zip/zlib-1.3.tar.gz | tar -x
#mv $SCRIPT_PATH/external_zip/zlib-1.3 $SCRIPT_PATH

cd $SCRIPT_PATH/glfw
cmake -S . -B build/ 
cmake --build build/ --config Release
cmake --build build/ --config Debug
