#!/bin/sh
sh buildjsfs_sdl2.sh $1
mkdir build
rm build/*
cp boxedwine.html build/
cp boxedwine.js build/
cp browserfs.min.js build/
cp boxedwine.html.mem build/
cp boxedwine.wasm build/
cp buildfiles/* build/
cd build
DATE=`date +%Y-%m-%d`
zip emscripten$DATE.zip *
