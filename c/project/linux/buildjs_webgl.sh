#/bin/sh
emcc -Wall -I../../include -I../../glshim/include -I../../glshim/src/util ../../source/sdl/*.c ../../platform/linux/*.c ../../source/emulation/*.c ../../source/io/*.c ../../source/kernel/*.c ../../source/kernel/devs/*.c ../../source/kernel/proc/*.c ../../source/kernel/loader/*.c ../../source/util/*.c ../../source/opengl/sdl/*.c ../../source/opengl/*.c ../../glshim/src/gl/*.c ../../glshim/src/util/*.c ../../glshim/src/gl/wrap/*.c ../../glshim/src/util/math/*.c -o boxedwine.html -O2 -DUNALIGNED_MEMORY=1 -DBOXEDWINE_SDL -DGLH=\<GL/gl.h\> --preload-file root@/root -s TOTAL_MEMORY=603979776 --shell-file shell.html
./packager.py
mv package.js boxedwine.js
gzip -9 boxedwine.data
mv boxedwine.data.gz boxedwine.data