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

#include "platform.h"
#include "nodeaccess.h"
#include "nodetype.h"
#include "filesystem.h"
#include "kerror.h"
#include "kprocess.h"
#include "oss.h"

BOOL mixer_init(struct KProcess* process, struct OpenNode* node) {
    return TRUE;
}

S64 mixer_length(struct OpenNode* node) {
    return 0;
}

BOOL mixer_setLength(struct OpenNode* node, S64 len) {
    return FALSE;
}

S64 mixer_getFilePointer(struct OpenNode* node) {
    return 0;
}

S64 mixer_seek(struct OpenNode* node, S64 pos) {
    return 0;
}

U32 mixer_read(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
    return 0;
}

extern struct KThread* currentThread;

U32 mixer_write(MMU_ARG struct OpenNode* node, U32 address, U32 l) {
    return 0;
}

void mixer_close(struct OpenNode* node) {    
    freeOpenNode(node);
}

U32 mixer_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
    U32 len = (request >> 16) & 0x3FFF;
    struct CPU* cpu = &thread->cpu;
    BOOL read = request & 0x40000000;
    BOOL write = request & 0x80000000;
    int i;

    switch (request & 0xFFFF) {
    case 0x5801: // SNDCTL_SYSINFO
        if (write) {
            U32 p = IOCTL_ARG1;
            for (i=0;i<len/4;i++)
                writed(MMU_PARAM_THREAD p+i*4, 2000+i);
            writeNativeString(MMU_PARAM_THREAD p, "OSS/Linux"); p+=32; // char product[32];		/* For example OSS/Free, OSS/Linux or OSS/Solaris */
            writeNativeString(MMU_PARAM_THREAD p, "4.0.0a"); p+=32; // char version[32];		/* For example 4.0a */
            writed(MMU_PARAM_THREAD p, 0x040000); p+=4; // int versionnum;		/* See OSS_GETVERSION */

            for (i=0;i<128;i++) {
                writeb(MMU_PARAM_THREAD p, i+100); p+=1; // char options[128];		/* Reserved */
            }

            writed(MMU_PARAM_THREAD p, 1); p+=4; // offset 196 int numaudios;		/* # of audio/dsp devices */
            for (i=0;i<8;i++) {
                writed(MMU_PARAM_THREAD p, 200+i); p+=4; // int openedaudio[8];		/* Bit mask telling which audio devices are busy */
            }

            writed(MMU_PARAM_THREAD p, 1); p+=4; // int numsynths;		/* # of availavle synth devices */
            writed(MMU_PARAM_THREAD p, 1); p+=4; // int nummidis;			/* # of available MIDI ports */
            writed(MMU_PARAM_THREAD p, 1); p+=4; // int numtimers;		/* # of available timer devices */
            writed(MMU_PARAM_THREAD p, 1); p+=4; // offset 244 int nummixers;		/* # of mixer devices */

            for (i=0;i<8;i++) {
                writed(MMU_PARAM_THREAD p, 0); p+=4; // int openedmidi[8];		/* Bit mask telling which midi devices are busy */
            }
            writed(MMU_PARAM_THREAD p, 1); p+=4; // offset 280 int numcards;			/* Number of sound cards in the system */
            writed(MMU_PARAM_THREAD p, 1); p+=4; // offset 284 int numaudioengines;		/* Number of audio engines in the system */
            writeNativeString(MMU_PARAM_THREAD p, "GPL"); p+=16; // char license[16];		/* For example "GPL" or "CDDL" */
            return 0;
        }
        break;
    case 0x5807: // SNDCTL_AUDIOINFO	
        if (write) {
            U32 p = IOCTL_ARG1;
            p+=4; // int dev; /* Audio device number */
            writeNativeString(MMU_PARAM_THREAD p, "BoxedWine mixer"); p+=64; // oss_devname_t name;
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int busy; /* 0, OPEN_READ, OPEN_WRITE or OPEN_READWRITE */
            writed(MMU_PARAM_THREAD p, -1); p+=4; // int pid;
            writed(MMU_PARAM_THREAD p, PCM_CAP_OUTPUT); p+=4; // int caps;			/* PCM_CAP_INPUT, PCM_CAP_OUTPUT */
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int iformats
            writed(MMU_PARAM_THREAD p, AFMT_U8 | AFMT_S16_LE | AFMT_S16_BE | AFMT_S8 | AFMT_U16_BE); p+=4; // int oformats;
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int magic;			/* Reserved for internal use */
            writeNativeString(MMU_PARAM_THREAD p, ""); p+=64; // oss_cmd_t cmd;		/* Command using the device (if known) */
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int card_number;
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int port_number;
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int mixer_dev;
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int legacy_device;		/* Obsolete field. Replaced by devnode */
            writed(MMU_PARAM_THREAD p, 1); p+=4; // int enabled;			/* 1=enabled, 0=device not ready at this moment */
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int flags;			/* For internal use only - no practical meaning */
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int min_rate
            writed(MMU_PARAM_THREAD p, 0); p+=4; // max_rate;	/* Sample rate limits */
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int min_channels
            writed(MMU_PARAM_THREAD p, 0); p+=4; // max_channels;	/* Number of channels supported */
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int binding;			/* DSP_BIND_FRONT, etc. 0 means undefined */
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int rate_source;
            writeNativeString(MMU_PARAM_THREAD p, ""); p+=64; // oss_handle_t handle;
            writed(MMU_PARAM_THREAD p, 0); p+=4; // unsigned int nrates
            for (i=0;i<20;i++) {
                writed(MMU_PARAM_THREAD p, 0); p+=4; // rates[20];	/* Please read the manual before using these */
            }
            writeNativeString(MMU_PARAM_THREAD p, ""); p+=32; // oss_longname_t song_name;	/* Song name (if given) */
            writeNativeString(MMU_PARAM_THREAD p, ""); p+=16; // oss_label_t label;		/* Device label (if given) */
            writed(MMU_PARAM_THREAD p, -1); p+=4; // int latency;			/* In usecs, -1=unknown */
            writeNativeString(MMU_PARAM_THREAD p, "/dev/dsp"); p+=16; // oss_devnode_t devnode;	/* Device special file name (absolute path) */
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int next_play_engine;		/* Read the documentation for more info */
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int next_rec_engine;		/* Read the documentation for more info */
            return 0;
        }        
    }
    return -K_ENODEV;
}

void mixer_setAsync(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
    if (isAsync)
        kwarn("mixer_setAsync not implemented");
}

BOOL mixer_isAsync(struct OpenNode* node, struct KProcess* process) {
    return 0;
}

void mixer_waitForEvents(struct OpenNode* node, struct KThread* thread, U32 events) {
}

BOOL mixer_isWriteReady(struct OpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_RDONLY;
}

BOOL mixer_isReadReady(struct OpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

U32 mixer_map(MMU_ARG struct OpenNode* node,  U32 address, U32 len, S32 prot, S32 flags, U64 off) {
    return 0;
}

BOOL mixer_canMap(struct OpenNode* node) {
    return FALSE;
}

struct NodeAccess mixerAccess = {mixer_init, mixer_length, mixer_setLength, mixer_getFilePointer, mixer_seek, mixer_read, mixer_write, mixer_close, mixer_map, mixer_canMap, mixer_ioctl, mixer_setAsync, mixer_isAsync, mixer_waitForEvents, mixer_isWriteReady, mixer_isReadReady};