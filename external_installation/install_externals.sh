#! /usr/bin/bash

# This script compiles and installs most libraries into "externals" directory

SCRIPT_PATH=$(pwd)/$0
SCRIPT_PATH=$(dirname $SCRIPT_PATH)

BASE_INSTALL_PATH=$SCRIPT_PATH/../external/

###########################################################################
# Unzip libraries
##########################################################################

unzip $SCRIPT_PATH/external_zip/FreeImage3180Win32Win64.zip -d $BASE_INSTALL_PATH
unzip $SCRIPT_PATH/external_zip/glew-2.2.0-win32.zip -d $BASE_INSTALL_PATH
unzip $SCRIPT_PATH/external_zip/irrKlang-64bit-1.6.0.zip -d $BASE_INSTALL_PATH
unzip $SCRIPT_PATH/external_zip/lua-5.4.2_Win64_vc16_lib -d $BASE_INSTALL_PATH/lua-5.4.2
unzip $SCRIPT_PATH/external_zip/ImGui -d $BASE_INSTALL_PATH
unzip $SCRIPT_PATH/external_zip/MSVC_Redist.zip -d $BASE_INSTALL_PATH

cd $SCRIPT_PATH/external_zip
gunzip --stdout $SCRIPT_PATH/external_zip/zlib-1.3.tar.gz | tar -x
mv $SCRIPT_PATH/external_zip/zlib-1.3 $SCRIPT_PATH

###########################################################################
# Compile libraries (except zlib)
###########################################################################

function BeginBuildLibrary()
{
	LIBRARY=$1
	CONFIG_TYPE=Release
	SRC_PATH=$SCRIPT_PATH/$LIBRARY
	BUILD_PATH=$SRC_PATH/build/$CONFIG_TYPE
	INSTALL_PATH=$BASE_INSTALL_PATH/$LIBRARY
	
	echo "========================================================="
	echo "  -> Building: " $LIBRARY
	echo "       SRC_PATH:     " $SRC_PATH
	echo "       BUILD_PATH:   " $BUILD_PATH
	echo "       INSTALL_PATH: " $INSTALL_PATH
	echo "========================================================="
}

for LIBRARY in assimp freetype2 glm json tinyxml2 glfw
do
	BeginBuildLibrary $LIBRARY
	cmake -S $SRC_PATH -B $BUILD_PATH
	cmake --build $BUILD_PATH --config $CONFIG_TYPE
	cmake --install $BUILD_PATH --prefix $INSTALL_PATH
done

###########################################################################
# zlib-1.3: the --prefix is not work working for this lib. Use -DCMAKE_INSTALL_PREFIX instead
###########################################################################

BeginBuildLibrary "zlib-1.3"
cmake -S $SRC_PATH -B $BUILD_PATH -DCMAKE_INSTALL_PREFIX=$INSTALL_PATH
cmake --build $BUILD_PATH --config $CONFIG_TYPE
cmake --install $BUILD_PATH
