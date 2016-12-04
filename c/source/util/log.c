/*
 *  Copyright (C) 2016  The BoxedWine Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdarg.h>

FILE* logFile;

void kpanic(const char* msg, ...) {
    int j = 0;
    int i;

    va_list argptr;
    va_start(argptr, msg);
    vfprintf(stderr, msg, argptr);
    if (logFile) {
        vfprintf(logFile, msg, argptr);
    }
    va_end(argptr);
    fprintf(stderr, "\n");
    if (logFile) {
        fprintf(logFile, "\n");
        fflush(logFile);
        fclose(logFile);
    }
    i=1/j;
}

void kwarn(const char* msg, ...) {
#ifdef _DEBUG
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(stderr, msg, argptr);
    if (logFile) {
        vfprintf(logFile, msg, argptr);
    }
    va_end(argptr);
    fprintf(stderr, "\n");
    if (logFile) {
        fprintf(logFile, "\n");
    }
#endif
}

void klog(const char* msg, ...) {
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(stdout, msg, argptr);
    if (logFile) {
        vfprintf(logFile, msg, argptr);
    }
    va_end(argptr);
    fprintf(stderr, "\n");
    if (logFile) {
        fprintf(logFile, "\n");
    }
}
