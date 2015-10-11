#/bin/sh
emcc -Wall -I../../include -I../../mesa/include -I../../mesa/src/mesa -I../../mesa/src/mesa/main -I../../mesa/src/mesa/glapi -I../../mesa/src/mesa/swrast -I../../mesa/src/mesa/shader ../../source/sdl/*.c ../../platform/linux/*.c ../../source/emulation/*.c ../../source/io/*.c ../../source/kernel/*.c ../../source/kernel/devs/*.c ../../source/kernel/proc/*.c ../../source/kernel/loader/*.c ../../source/util/*.c ../../source/opengl/mesa/*.c ../../source/opengl/*.c ../../mesa/src/glsl/cl/*.c ../../mesa/src/glsl/pp/*.c ../../mesa/src/mesa/drivers/common/*.c ../../mesa/src/mesa/drivers/osmesa/*.c ../../mesa/src/mesa/glapi/*.c ../../mesa/src/mesa/main/*.c ../../mesa/src/mesa/math/*.c ../../mesa/src/mesa/shader/slang/*.c ../../mesa/src/mesa/shader/*.c ../../mesa/src/mesa/swrast/*.c ../../mesa/src/mesa/swrast_setup/*.c ../../mesa/src/mesa/tnl/*.c ../../mesa/src/mesa/vbo/*.c -o boxedwine.html -O2 -DUNALIGNED_MEMORY=1 -DMESA -DGLH=\"../../mesa/include/GL/osmesa.h\" --preload-file root@/root -s TOTAL_MEMORY=503979776  --shell-file shell.html
./packager.py
mv package.js boxedwine.js
gzip -9 boxedwine.data
mv boxedwine.data.gz boxedwine.data
