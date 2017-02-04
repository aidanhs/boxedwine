#/bin/sh
emcc -Wall -Wno-unused-variable -Wno-parentheses-equality -Wno-unused-function -I../../include ../../lib/zlib/contrib/minizip/unzip.c ../../lib/zlib/contrib/minizip/ioapi.c ../../source/sdl/*.c ../../platform/linux/*.c ../../source/emulation/*.c ../../source/io/*.c ../../source/kernel/*.c ../../source/kernel/devs/*.c ../../source/kernel/proc/*.c ../../source/kernel/loader/*.c ../../source/util/*.c ../../source/util/pbl/*.c ../../source/opengl/sdl/*.c ../../source/opengl/*.c -o boxedwine.html -O2 -DUNALIGNED_MEMORY=1 -DUSE_MMU -s USE_SDL=2 -s USE_ZLIB=1 -DSDL2 -s TOTAL_MEMORY=400000000  --shell-file shellfs.html
zip -r9qdgds 10m boxedwine.zip root

if [ "$1" = "remote" ] ; then
    zip -r9qdgds 10m boxedwine.zip root -x root/usr/lib/i386-linux-gnu/wine/wine/**\*
    if [ -d "dlls" ]; then
        STR="{\"dlls\": {"
        for file in `find dlls -name "*.*"`; do
        STR="${STR}\"${file#*/}\": null  ,"
        done
        STR="${STR},,"
        echo ${STR/,,,/'}}'} > dlls.json

    fi
else
	zip -r9qdgds 10m boxedwine.zip root
fi
