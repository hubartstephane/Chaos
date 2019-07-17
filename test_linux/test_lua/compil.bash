#! /bin/bash

cd $(dirname $0)
TARGET_NAME=$(basename $(pwd))

TARGET_DIR=../../../build/test_linux/$TARGET_NAME
mkdir -p $TARGET_DIR


LIBS="lua-5.3"
for LIB in $LIBS; do 
	CCFLAGS="$CCFLAGS $(pkg-config --libs $LIB)"
	CCFLAGS="$CCFLAGS $(pkg-config --cflags $LIB)"
done

gcc $CCFLAGS -o $TARGET_DIR/$TARGET_NAME *.cpp
