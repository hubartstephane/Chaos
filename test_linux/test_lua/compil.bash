#! /bin/bash

cd $(dirname $0)
TARGET_NAME=$(basename $(pwd))

LIBS="lua-5.3"
for LIB in $LIBS; do 
	CCFLAGS="$CCFLAGS $(pkg-config --libs $LIB)"
	CCFLAGS="$CCFLAGS $(pkg-config --cflags $LIB)"
done

gcc $CCFLAGS -o $TARGET_NAME *.cpp
