#! /bin/bash


PATH=$PATH:/usr/lib/x86_64-linux-gnu

cd $(dirname $0)
TARGET_NAME=$(basename $(pwd))

TARGET_DIR=../../../build/test_linux/$TARGET_NAME
mkdir -p $TARGET_DIR

#lua5.3-deb-multiarch.h
LUA_LIB_PATH=./x86_64-linux-gnu/liblua5.3

#CCFLAGS="$CCFLAGS -L$LUA_LIB_PATH"


CCFLAGS="-std=c++14 -v -march=x86-64"
LIBS="lua5.3 glfw3"
for LIB in $LIBS; do 
	CCFLAGS="$CCFLAGS $(pkg-config --libs $LIB)"
	CCFLAGS="$CCFLAGS $(pkg-config --cflags $LIB)"
done
COMPILER=g++-7

echo $COMPILER $CCFLAGS -o $TARGET_DIR/$TARGET_NAME *.cpp
$COMPILER $CCFLAGS -o $TARGET_DIR/$TARGET_NAME *.cpp
