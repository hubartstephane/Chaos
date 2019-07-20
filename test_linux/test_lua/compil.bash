#! /bin/bash

cd $(dirname $0)
TARGET_NAME=$(basename $(pwd))

TARGET_DIR=../../../build/test_linux/$TARGET_NAME
mkdir -p $TARGET_DIR

#lua5.3-deb-multiarch.h
LUA_LIB_PATH=./x86_64-linux-gnu/liblua5.3

#CCFLAGS="$CCFLAGS -L$LUA_LIB_PATH"


CCFLAGS="-ldl"

#LIBS="lua5.3-c++ lua5.3 lua5.3-system"

LIBS="lua5.3"
for LIB in $LIBS; do 
	CCFLAGS="$CCFLAGS $(pkg-config --libs $LIB)"
	CCFLAGS="$CCFLAGS $(pkg-config --cflags $LIB)"
done

echo gcc $CCFLAGS -o $TARGET_DIR/$TARGET_NAME *.cpp
gcc $CCFLAGS -o $TARGET_DIR/$TARGET_NAME *.cpp
