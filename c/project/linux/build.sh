#!/bin/bash
gcc -m32 -Wall -I../../include ../../source/sdl/*.c ../../platform/linux/*.c ../../source/emulation/*.c ../../source/io/*.c ../../source/kernel/*.c ../../source/kernel/devs/*.c ../../source/kernel/proc/*.c ../../source/kernel/loader/*.c ../../source/util/*.c ../../source/test/*.c -o boxedwine -lm `sdl-config --cflags --libs` -O2
