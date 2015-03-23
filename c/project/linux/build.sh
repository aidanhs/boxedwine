#!/bin/bash
gcc -Wall -I../../include ../../source/sdl/*.c ../../platform/linux/*.c ../../source/emulation/*.c ../../source/io/*.c ../../source/kernel/*.c ../../source/kernel/devs/*.c ../../source/kernel/loader/*.c ../../source/util/*.c ../../source/test/*.c -o boxedwine -lm `sdl-config --cflags --libs` -O2
