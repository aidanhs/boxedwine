#/bin/sh
emcc -Wall -Wno-unused-variable -Wno-parentheses-equality -Wno-unused-function -I../../include ../../source/sdl/*.c ../../platform/linux/*.c ../../source/emulation/*.c ../../source/io/*.c ../../source/kernel/*.c ../../source/kernel/devs/*.c ../../source/kernel/proc/*.c ../../source/kernel/loader/*.c ../../source/util/*.c ../../source/util/pbl/*.c ../../source/opengl/sdl/*.c ../../source/opengl/*.c -o boxedwine.html -O3 -DUNALIGNED_MEMORY=1 -DUSE_MMU -s USE_SDL=2 -DSDL2 --preload-file root@/root -s TOTAL_MEMORY=400000000 --shell-file shell.html
./packager.py
mv package.js boxedwine.js
gzip -9 boxedwine.data
mv boxedwine.data.gz boxedwine.data
