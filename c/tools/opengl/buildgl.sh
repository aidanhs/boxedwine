#!/bin/bash
gcc -c -Wall -Werror -Wno-return-type -fpic gl.c
gcc -Wl,-soname,libGL.so.1 -shared -o libGL.so gl.o
