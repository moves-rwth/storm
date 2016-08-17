#!/bin/sh

DYLIBBUNDLER_DIR=/Users/chris/work/macdylibbundler/

mkdir -p $1
mkdir -p $1/bin
mkdir -p $1/lib
cp $2 $1/bin
$DYLIBBUNDLER_DIR/dylibbundler -cd -od -b -p @executable_path/../lib -x $1/bin/storm -d $1/lib
python packager.py --bin storm --dir $1
