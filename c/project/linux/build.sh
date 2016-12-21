#!/bin/bash
gcc -m32 -Wall -I../../include ../../source/sdl/*.c ../../platform/linux/*.c ../../source/emulation/*.c ../../source/io/*.c ../../source/kernel/*.c ../../source/kernel/devs/*.c ../../source/kernel/proc/*.c ../../source/kernel/loader/*.c ../../source/util/*.c ../../source/util/pbl/*.c ../../source/opengl/sdl/*.c ../../source/opengl/*.c ../../source/test/*.c -o boxedwine -lm -DUNALIGNED_MEMORY=1 -DSDL2=1 -DUSE_MMU `sdl2-config --cflags --libs` -O2
