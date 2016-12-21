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
#include "log.h"
#include "kprocess.h"
#include "kscheduler.h"
#include "oss.h"
#include "kalloc.h"

#include <SDL.h>
#include <string.h>

struct DspData {
	BOOL isDspOpen;
	U32 dspFmt;
	U32 sdlFmt;
	U32 dspChannels;
	U32 dspFragSize;
	U32 dspFreq;
	U32 bytesPerSecond;
#define DSP_BUFFER_SIZE 1024*64
	U8 dspBuffer[DSP_BUFFER_SIZE];
	S32 dspBufferLen;
	U32 dspBufferPos;
	struct KThread* dspWaitingToWriteThread;
	U32 pauseAtLen;
};
#define pauseEnabled() (data->pauseAtLen!=0xFFFFFFFF)

void closeAudio(struct DspData* data) {
    if (data->isDspOpen) {
        SDL_CloseAudio();
        data->isDspOpen = 0;
    }
}

void audioCallback(void *userdata, U8* stream, S32 len) {
    S32 available;
    S32 result;
	struct DspData* data = userdata;

	if (data->dspBufferLen == 0) {
		memset(stream, 0, len);
		return;
	}
	available = DSP_BUFFER_SIZE - data->dspBufferPos;
	if (available>data->dspBufferLen)
		available = data->dspBufferLen;
    if (available > len)
        available = len;
	if (pauseEnabled() && available > data->pauseAtLen) {
		available = data->pauseAtLen;
    }
	memcpy(stream, data->dspBuffer + data->dspBufferPos, available);
    result=available;
	data->dspBufferPos += result;
	data->dspBufferLen -= result;
    if (pauseEnabled())
		data->pauseAtLen -= result;
	if (data->dspBufferPos == DSP_BUFFER_SIZE) {
		data->dspBufferPos = 0;
        audioCallback(userdata, stream+result, len-result);
        return;
    }
	if (data->dspWaitingToWriteThread)
		wakeThread(data->dspWaitingToWriteThread);
    if (result<len) {
        memset(stream+result, 0, len-result);
		printf("audio underflow\n");
    }
}

void openAudio(struct DspData* data) {
    SDL_AudioSpec want;
    SDL_AudioSpec got;

    memset(&want, 0, sizeof(SDL_AudioSpec));
	want.freq = data->dspFreq;
	want.format = data->sdlFmt;
	want.channels = data->dspChannels;
    //want.samples = 4096;
    want.callback = audioCallback;  // you wrote this function elsewhere.
	want.userdata = data;

    if (SDL_OpenAudio(&want, &got) < 0) {
        printf("Failed to open audio: %s\n", SDL_GetError());
    } 
	data->isDspOpen = 1;
    SDL_PauseAudio(0);
	data->pauseAtLen = 0xFFFFFFFF;
	data->dspFragSize = got.size;
	data->bytesPerSecond = data->dspFreq * data->dspChannels * ((data->sdlFmt == AUDIO_S16LSB || data->sdlFmt == AUDIO_S16MSB || data->sdlFmt == AUDIO_U16LSB || data->sdlFmt == AUDIO_U16MSB) ? 2 : 1);
	printf("openAudio: freq=%d(got %d) format=%d(%d/got %d) channels=%d(got %d)\n", data->dspFreq, got.freq, data->dspFmt, data->sdlFmt, got.format, data->dspChannels, got.channels);
}

BOOL dsp_init(struct KProcess* process, struct OpenNode* node) {
	struct DspData* data = kalloc(sizeof(struct DspData), 0);
	data->dspFmt = AFMT_U8;
	data->sdlFmt = AUDIO_U8;
	data->dspChannels = 1;
	data->dspFragSize = 5512;
	data->dspFreq = 11025;
	node->data = data;
    return TRUE;
}

S64 dsp_length(struct OpenNode* node) {
    return 0;
}

BOOL dsp_setLength(struct OpenNode* node, S64 len) {
    return FALSE;
}

S64 dsp_getFilePointer(struct OpenNode* node) {
    return 0;
}

S64 dsp_seek(struct OpenNode* node, S64 pos) {
    return 0;
}

U32 dsp_read(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
    return 0;
}

extern struct KThread* currentThread;

U32 dsp_write(MMU_ARG struct OpenNode* node, U32 address, U32 l) {
    S32 len = (S32)l;
    S32 available;
    S32 result;
    U32 startPos;
	struct DspData* data = node->data;

    if (len+data->dspBufferLen>DSP_BUFFER_SIZE) {
		if (data->dspWaitingToWriteThread) {
			kpanic("%d tried to wait on writing to dsp but %d is already waiting", currentThread->id, data->dspWaitingToWriteThread);
        }
		data->dspWaitingToWriteThread = currentThread;
		addClearOnWake(currentThread, &data->dspWaitingToWriteThread);
        return -K_WAIT;
    }
	if (!data->isDspOpen)
        openAudio(data);
    SDL_LockAudio();
	startPos = data->dspBufferPos + data->dspBufferLen;
    if (startPos>=DSP_BUFFER_SIZE)
        startPos-=DSP_BUFFER_SIZE;
    available = DSP_BUFFER_SIZE-startPos;
    if (available>len)
        available = len;
	if (available>DSP_BUFFER_SIZE - data->dspBufferLen)
		available = DSP_BUFFER_SIZE - data->dspBufferLen;
	memcopyToNative(MMU_PARAM address, (char*)data->dspBuffer + startPos, available);
	data->dspBufferLen += available;
    result = available;	
	if (len != available && data->dspBufferLen< DSP_BUFFER_SIZE) {
        len-=available;
        address+=available;
        // we wrapped around the the buffer
		startPos = data->dspBufferPos + data->dspBufferLen;
        if (startPos>=DSP_BUFFER_SIZE)
            startPos-=DSP_BUFFER_SIZE;
        if (startPos!=0) {
            kpanic("dsp_write logic error");
        }
        available = DSP_BUFFER_SIZE-startPos;
        if (available>len)
            available = len;
		if (available>DSP_BUFFER_SIZE - data->dspBufferLen)
			available = DSP_BUFFER_SIZE - data->dspBufferLen;
		memcopyToNative(MMU_PARAM address, (char*)data->dspBuffer + startPos, available);
		data->dspBufferLen += available;
        result+=available;
    }
    SDL_UnlockAudio();	
    return result;
}

void dsp_close(struct OpenNode* node) {
	struct DspData* data = node->data;

    if (data->isDspOpen) {
        closeAudio(data);
    }
	kfree(data, 0);
	node->data = 0;
    freeOpenNode(node);
}

U32 dsp_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
    U32 len = (request >> 16) & 0x3FFF;
    struct CPU* cpu = &thread->cpu;
    //BOOL read = request & 0x40000000;
    BOOL write = request & 0x80000000;
    int i;
	struct DspData* data = node->data;

    switch (request & 0xFFFF) {
    case 0x5000: // SNDCTL_DSP_RESET
        return 0;
    case 0x5002:  // SNDCTL_DSP_SPEED 
        if (len!=4) {
            kpanic("SNDCTL_DSP_SPEED was expecting a len of 4");
        }
		data->dspFreq = readd(MMU_PARAM_THREAD IOCTL_ARG1);
		if (write)
			writed(MMU_PARAM_THREAD IOCTL_ARG1, data->dspFreq);
        return 0;
    case 0x5003: { // SNDCTL_DSP_STEREO
        U32 fmt;

        if (len!=4) {
            kpanic("SNDCTL_DSP_STEREO was expecting a len of 4");
        }
        fmt = readd(MMU_PARAM_THREAD IOCTL_ARG1);
		if (fmt != data->dspChannels - 1) {
            closeAudio(data);
        }
        if (fmt == 0) {
			data->dspChannels = 1;
        } else if (fmt == 1) {
			data->dspChannels = 2;
        } else {
            kpanic("SNDCTL_DSP_STEREO wasn't expecting a value of %d", fmt);
        }
        if (write)
			writed(MMU_PARAM_THREAD IOCTL_ARG1, data->dspChannels - 1);
        return 0;
    }
    case 0x5005: { // SNDCTL_DSP_SETFMT 
        U32 fmt;

        if (len!=4) {
            kpanic("SNDCTL_DSP_SETFMT was expecting a len of 4");
        }
        fmt = readd(MMU_PARAM_THREAD IOCTL_ARG1);
		if (fmt != AFMT_QUERY && fmt != data->dspFmt) {
            closeAudio(data);
        }
        switch (fmt) {
        case AFMT_QUERY:
            break;
        case AFMT_MU_LAW:
        case AFMT_A_LAW:
        case AFMT_IMA_ADPCM:
			data->dspFmt = AFMT_U8;
			data->sdlFmt = AUDIO_U8;
            break;
        case AFMT_U8:
			data->dspFmt = AFMT_U8;
			data->sdlFmt = AUDIO_U8;
            break;
        case AFMT_S16_LE:
			data->dspFmt = AFMT_S16_LE;
			data->sdlFmt = AUDIO_S16LSB;
            break;
        case AFMT_S16_BE:
			data->dspFmt = AFMT_S16_BE;
			data->sdlFmt = AUDIO_S16MSB;
            break;
        case AFMT_S8:
			data->dspFmt = AFMT_S8;
			data->sdlFmt = AUDIO_S8;
            break;
        case AFMT_U16_LE:
			data->dspFmt = AFMT_U16_LE;
			data->sdlFmt = AUDIO_U16LSB;
            break;
        case AFMT_U16_BE:
			data->dspFmt = AFMT_U16_BE;
			data->sdlFmt = AUDIO_U16MSB;
            break;
        case AFMT_MPEG:
			data->dspFmt = AFMT_U8;
			data->sdlFmt = AUDIO_U8;
            break;
        }
        if (write)
			writed(MMU_PARAM_THREAD IOCTL_ARG1, data->dspFmt);
		else if (data->dspFmt != fmt) {
            kpanic("SNDCTL_DSP_SETFMT dspFmt!=fmt and can't write result");
        }
        return 0;
        }
    case 0x5006: {// SOUND_PCM_WRITE_CHANNELS
        U32 channels = readd(MMU_PARAM_THREAD IOCTL_ARG1);
		if (channels != data->dspChannels) {
            closeAudio(data);
        }
        if (channels==1) {
			data->dspChannels = 1;
        } else if (channels == 2) {
			data->dspChannels = 2;
        } else {
			data->dspChannels = 2;
        }
        if (write)
			writed(MMU_PARAM_THREAD IOCTL_ARG1, data->dspChannels);
        return 0;
        }
    case 0x500A: // SNDCTL_DSP_SETFRAGMENT
		data->dspFragSize = 1 << (readd(MMU_PARAM_THREAD IOCTL_ARG1) & 0xFFFF);
        return 0;
    case 0x500B: // SNDCTL_DSP_GETFMTS
        writed(MMU_PARAM_THREAD IOCTL_ARG1, AFMT_U8 | AFMT_S16_LE | AFMT_S16_BE | AFMT_S8 | AFMT_U16_BE);
        return 0;

		//typedef struct audio_buf_info {
		//	int fragments;     /* # of available fragments (partially usend ones not counted) */
		//	int fragstotal;    /* Total # of fragments allocated */
		//	int fragsize;      /* Size of a fragment in bytes */
		//
		//	int bytes;         /* Available space in bytes (includes partially used fragments) */
		//	/* Note! 'bytes' could be more than fragments*fragsize */
		//} audio_buf_info;

    case 0x500C: // SNDCTL_DSP_GETOSPACE
		writed(MMU_PARAM_THREAD IOCTL_ARG1, (DSP_BUFFER_SIZE - data->dspBufferLen) / data->dspFragSize); // fragments
		writed(MMU_PARAM_THREAD IOCTL_ARG1 + 4, DSP_BUFFER_SIZE / data->dspFragSize);
		writed(MMU_PARAM_THREAD IOCTL_ARG1 + 8, data->dspFragSize);
		writed(MMU_PARAM_THREAD IOCTL_ARG1 + 12, DSP_BUFFER_SIZE - data->dspBufferLen);
        return 0;
    case 0x500F: // SNDCTL_DSP_GETCAPS
        writed(MMU_PARAM_THREAD IOCTL_ARG1, DSP_CAP_TRIGGER);
        return 0;
    case 0x5010: // SNDCTL_DSP_SETTRIGGER
        if (readd(MMU_PARAM_THREAD IOCTL_ARG1) & PCM_ENABLE_OUTPUT) {
            SDL_PauseAudio(0);
			data->pauseAtLen = 0xFFFFFFFF;
        } else {            
			data->pauseAtLen = data->dspBufferLen;
			if (data->pauseAtLen == 0) {
                SDL_PauseAudio(0);
            }
        }
        return 0;
    case 0x5012: // SNDCTL_DSP_GETOPTR
        writed(MMU_PARAM_THREAD IOCTL_ARG1, 0); // Total # of bytes processed
        writed(MMU_PARAM_THREAD IOCTL_ARG1 + 4, 0); // # of fragment transitions since last time
        if (pauseEnabled()) {
			writed(MMU_PARAM_THREAD IOCTL_ARG1 + 8, data->pauseAtLen); // Current DMA pointer value
			if (data->pauseAtLen == 0) {
                SDL_PauseAudio(0);
            }
        } else {
			writed(MMU_PARAM_THREAD IOCTL_ARG1 + 8, data->dspBufferLen); // Current DMA pointer value
        }
        return 0;
    case 0x5016: // SNDCTL_DSP_SETDUPLEX
        return -K_EINVAL;
    case 0x5017: // SNDCTL_DSP_GETODELAY 
        if (write) {
			writed(MMU_PARAM_THREAD IOCTL_ARG1, data->dspBufferLen);
            return 0;
        }
    case 0x580C: // SNDCTL_ENGINEINFO
        if (write) {
            U32 p = IOCTL_ARG1;
            p+=4; // int dev; /* Audio device number */
            writeNativeString(MMU_PARAM_THREAD p, "BoxedWine audio"); p+=64; // oss_devname_t name;
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
			writed(MMU_PARAM_THREAD p, 11025); p += 4; // int min_rate
            writed(MMU_PARAM_THREAD p, 44100); p+=4; // max_rate;	/* Sample rate limits */
            writed(MMU_PARAM_THREAD p, 1); p+=4; // int min_channels
            writed(MMU_PARAM_THREAD p, 2); p+=4; // max_channels;	/* Number of channels supported */
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int binding;			/* DSP_BIND_FRONT, etc. 0 means undefined */
            writed(MMU_PARAM_THREAD p, 0); p+=4; // int rate_source;
            writeNativeString(MMU_PARAM_THREAD p, ""); p+=32; // oss_handle_t handle;
            writed(MMU_PARAM_THREAD p, 0); p+=4; // unsigned int nrates
            for (i=0;i<20;i++) {
                writed(MMU_PARAM_THREAD p, 0); p+=4; // rates[20];	/* Please read the manual before using these */
            }
            writeNativeString(MMU_PARAM_THREAD p, ""); p+=64; // oss_longname_t song_name;	/* Song name (if given) */
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

void dsp_setAsync(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
    if (isAsync)
        kwarn("dsp_setAsync not implemented");
}

BOOL dsp_isAsync(struct OpenNode* node, struct KProcess* process) {
    return 0;
}

void dsp_waitForEvents(struct OpenNode* node, struct KThread* thread, U32 events) {
	struct DspData* data = node->data;

    if (events & K_POLLOUT) {
		if (data->dspWaitingToWriteThread)
			kpanic("%d tried to wait on a input read, but %d is already waiting.", thread->id, data->dspWaitingToWriteThread->id);
		data->dspWaitingToWriteThread = thread;
		addClearOnWake(thread, &data->dspWaitingToWriteThread);
    }
}

BOOL dsp_isWriteReady(struct OpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_RDONLY;
}

BOOL dsp_isReadReady(struct OpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

U32 dsp_map(MMU_ARG struct OpenNode* node,  U32 address, U32 len, S32 prot, S32 flags, U64 off) {
    return 0;
}

BOOL dsp_canMap(struct OpenNode* node) {
    return FALSE;
}

struct NodeAccess dspAccess = {dsp_init, dsp_length, dsp_setLength, dsp_getFilePointer, dsp_seek, dsp_read, dsp_write, dsp_close, dsp_map, dsp_canMap, dsp_ioctl, dsp_setAsync, dsp_isAsync, dsp_waitForEvents, dsp_isWriteReady, dsp_isReadReady};
