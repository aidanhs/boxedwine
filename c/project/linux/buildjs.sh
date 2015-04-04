#/bin/sh
emcc -Wall -I../../include ../../source/sdl/*.c ../../platform/linux/*.c ../../source/emulation/*.c ../../source/io/*.c ../../source/kernel/*.c ../../source/kernel/devs/*.c ../../source/kernel/proc/*.c ../../source/kernel/loader/*.c ../../source/util/*.c ../../source/test/*.c -o boxedwine.html -O2 -DUNALIGNED_MEMORY=1 --preload-file root@/root -s TOTAL_MEMORY=256000000
