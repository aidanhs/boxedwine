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

#include <SDL.h>
#include <string.h>

#define AFMT_QUERY               0x00000000      /* Return current fmt */
#define AFMT_MU_LAW              0x00000001
#define AFMT_A_LAW               0x00000002
#define AFMT_IMA_ADPCM           0x00000004
#define AFMT_U8                  0x00000008
#define AFMT_S16_LE              0x00000010      /* Little endian signed 16*/
#define AFMT_S16_BE              0x00000020      /* Big endian signed 16 */
#define AFMT_S8                  0x00000040
#define AFMT_U16_LE              0x00000080      /* Little endian U16 */
#define AFMT_U16_BE              0x00000100      /* Big endian U16 */
#define AFMT_MPEG                0x00000200      /* MPEG (2) audio */

#define PCM_ENABLE_INPUT         0x00000001
#define PCM_ENABLE_OUTPUT        0x00000002

#define DSP_CAP_REVISION         0x000000ff      /* Bits for revision level (0 to 255) */
#define DSP_CAP_DUPLEX           0x00000100      /* Full duplex record/playback */
#define DSP_CAP_REALTIME         0x00000200      /* Real time capability */
#define DSP_CAP_BATCH            0x00000400      /* Device has some kind of */
                                                 /* internal buffers which may */
                                                 /* cause some delays and */
                                                 /* decrease precision of timing */
#define DSP_CAP_COPROC           0x00000800      /* Has a coprocessor */
                                                 /* Sometimes it's a DSP */
                                                 /* but usually not */
#define DSP_CAP_TRIGGER          0x00001000      /* Supports SETTRIGGER */
#define DSP_CAP_MMAP             0x00002000      /* Supports mmap() */
#define DSP_CAP_MULTI            0x00004000      /* support multiple open */
#define DSP_CAP_BIND             0x00008000      /* channel binding to front/rear/cneter/lfe */

BOOL isDspOpen = 0;
static U32 dspFmt = AFMT_U8;
static U32 sdlFmt = AUDIO_U8;
static U32 dspChannels = 1;
static U32 dspFragSize = 1024;
static U32 dspFreq = 8000;
#define DSP_BUFFER_SIZE 1024*64
static U8 dspBuffer[DSP_BUFFER_SIZE];
static S32 dspBufferLen;
static U32 dspBufferPos;
static struct KThread* dspWaitingToWriteThread;
static U32 pauseAtLen;

#define pauseEnabled() (pauseAtLen!=0xFFFFFFFF)

void closeAudio() {
    if (isDspOpen) {
        SDL_CloseAudio();
        isDspOpen = 0;
    }
}

void audioCallback(void *userdata, U8* stream, S32 len) {
    S32 available;
    S32 result;

    if ( dspBufferLen == 0 )
        return;
    available = DSP_BUFFER_SIZE-dspBufferPos;
    if (available>dspBufferLen)
        available = dspBufferLen;
    if (available > len)
        available = len;
    if (pauseEnabled()  && available > pauseAtLen) {
        available = pauseAtLen;
    }
    memcpy(stream, dspBuffer+dspBufferPos, available);
    result=available;
    dspBufferPos += result;
    dspBufferLen -= result;
    if (pauseEnabled())
        pauseAtLen -= result;
    if (dspBufferPos==DSP_BUFFER_SIZE) {
        dspBufferPos = 0;
        audioCallback(userdata, stream+result, len-result);
        return;
    }
    if (dspWaitingToWriteThread)
        wakeThread(dspWaitingToWriteThread);
    if (result<len) {
        memset(stream+result, 0, len-result);
    }
}

void openAudio() {
    SDL_AudioSpec want;
    SDL_AudioSpec got;

    memset(&want, 0, sizeof(SDL_AudioSpec));
    want.freq = dspFreq;
    want.format = sdlFmt;
    want.channels = dspChannels;
    want.samples = 512;
    want.callback = audioCallback;  // you wrote this function elsewhere.

    if (SDL_OpenAudio(&want, &got) < 0) {
        printf("Failed to open audio: %s\n", SDL_GetError());
    } 
    isDspOpen = 1;
    SDL_PauseAudio(0);
    printf("openAudio: freq=%d(got %d) format=%d(%d/got %d) channels=%d(got %d)\n", dspFreq, got.freq, dspFmt, sdlFmt, got.format, dspChannels, got.channels);
}

BOOL dsp_init(struct KProcess* process, struct OpenNode* node) {
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
    if (len+dspBufferLen>DSP_BUFFER_SIZE) {
        if (dspWaitingToWriteThread) {
            kpanic("%d tried to wait on writing to dsp but %d is already waiting", currentThread->id, dspWaitingToWriteThread);
        }
        dspWaitingToWriteThread = currentThread;
        addClearOnWake(currentThread, &dspWaitingToWriteThread);
        return -K_WAIT;
    }
    if (!isDspOpen)
        openAudio();
    SDL_LockAudio();
    startPos = dspBufferPos+dspBufferLen;
    if (startPos>=DSP_BUFFER_SIZE)
        startPos-=DSP_BUFFER_SIZE;
    available = DSP_BUFFER_SIZE-startPos;
    if (available>len)
        available = len;
    if (available>DSP_BUFFER_SIZE-dspBufferLen)
        available=DSP_BUFFER_SIZE-dspBufferLen;
    memcopyToNative(MMU_PARAM address, (char*)dspBuffer+startPos, available);
    dspBufferLen+=available;
    result = available;	
    if (len!=available && dspBufferLen< DSP_BUFFER_SIZE) {
        len-=available;
        address+=available;
        // we wrapped around the the buffer
        startPos = dspBufferPos+dspBufferLen;
        if (startPos>=DSP_BUFFER_SIZE)
            startPos-=DSP_BUFFER_SIZE;
        if (startPos!=0) {
            kpanic("dsp_write logic error");
        }
        available = DSP_BUFFER_SIZE-startPos;
        if (available>len)
            available = len;
        if (available>DSP_BUFFER_SIZE-dspBufferLen)
            available=DSP_BUFFER_SIZE-dspBufferLen;
        memcopyToNative(MMU_PARAM address, (char*)dspBuffer+startPos, available);
        dspBufferLen+=available;
        result+=available;
    }
    SDL_UnlockAudio();	
    return result;
}

void dsp_close(struct OpenNode* node) {
    if (isDspOpen) {
        closeAudio();
    }
    freeOpenNode(node);
}

U32 dsp_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
    U32 len = (request >> 16) & 0x3FFF;
    struct CPU* cpu = &thread->cpu;
    //BOOL read = request & 0x40000000;
    BOOL write = request & 0x80000000;

    switch (request & 0xFFFF) {
    case 0x5000: // SNDCTL_DSP_RESET
        return 0;
    case 0x5002:  // SNDCTL_DSP_SPEED 
        if (len!=4) {
            kpanic("SNDCTL_DSP_SPEED was expecting a len of 4");
        }
        dspFreq = readd(MMU_PARAM_THREAD IOCTL_ARG1);
        return 0;
    case 0x5003: { // SNDCTL_DSP_STEREO
        U32 fmt;

        if (len!=4) {
            kpanic("SNDCTL_DSP_STEREO was expecting a len of 4");
        }
        fmt = readd(MMU_PARAM_THREAD IOCTL_ARG1);
        if (fmt!=dspChannels-1) {
            closeAudio();
        }
        if (fmt == 0) {
            dspChannels = 1;
        } else if (fmt == 1) {
            dspChannels = 2;
        } else {
            kpanic("SNDCTL_DSP_STEREO wasn't expecting a value of %d", fmt);
        }
        if (write)
            writed(MMU_PARAM_THREAD IOCTL_ARG1, dspChannels - 1);
        return 0;
    }
    case 0x5005: { // SNDCTL_DSP_SETFMT 
        U32 fmt;

        if (len!=4) {
            kpanic("SNDCTL_DSP_SETFMT was expecting a len of 4");
        }
        fmt = readd(MMU_PARAM_THREAD IOCTL_ARG1);
        if (fmt!=AFMT_QUERY && fmt!=dspFmt) {
            closeAudio();
        }
        switch (fmt) {
        case AFMT_QUERY:
            break;
        case AFMT_MU_LAW:
        case AFMT_A_LAW:
        case AFMT_IMA_ADPCM:
            dspFmt = AFMT_U8;
            sdlFmt = AUDIO_U8;
            break;
        case AFMT_U8:
            dspFmt = AFMT_U8;
            sdlFmt = AUDIO_U8;
            break;
        case AFMT_S16_LE:
            dspFmt = AFMT_S16_LE;
            sdlFmt = AUDIO_S16LSB;
            break;
        case AFMT_S16_BE:
            dspFmt = AFMT_S16_BE;
            sdlFmt = AUDIO_S16MSB;
            break;
        case AFMT_S8:
            dspFmt = AFMT_S8;
            sdlFmt = AUDIO_S8;
            break;
        case AFMT_U16_LE:
            dspFmt = AFMT_U16_LE;
            sdlFmt = AUDIO_U16LSB;
            break;
        case AFMT_U16_BE:
            dspFmt = AFMT_U16_BE;
            sdlFmt = AUDIO_U16MSB;
            break;
        case AFMT_MPEG:
            dspFmt = AFMT_U8;
            sdlFmt = AUDIO_U8;
            break;
        }
        if (write)
            writed(MMU_PARAM_THREAD IOCTL_ARG1, dspFmt);
        else if (dspFmt!=fmt) {
            kpanic("SNDCTL_DSP_SETFMT dspFmt!=fmt and can't write result");
        }
        return 0;
        }
    case 0x5006: {// SOUND_PCM_WRITE_CHANNELS
        U32 channels = readd(MMU_PARAM_THREAD IOCTL_ARG1);
        if (channels!=dspChannels) {
            closeAudio();
        }
        if (channels==1) {
            dspChannels = 1;
        } else if (channels == 2) {
            dspChannels = 2;
        } else {
            dspChannels = 2;
        }
        if (write)
            writed(MMU_PARAM_THREAD IOCTL_ARG1, dspChannels);
        return 0;
        }
    case 0x500A: // SNDCTL_DSP_SETFRAGMENT
        dspFragSize = 1 << (readd(MMU_PARAM_THREAD IOCTL_ARG1) & 0xFFFF);
        return 0;
    case 0x500B: // SNDCTL_DSP_GETFMTS
        writed(MMU_PARAM_THREAD IOCTL_ARG1, AFMT_U8 | AFMT_S16_LE | AFMT_S16_BE | AFMT_S8 | AFMT_U16_BE);
        return 0;
    case 0x500C: // SNDCTL_DSP_GETOSPACE
        writed(MMU_PARAM_THREAD IOCTL_ARG1, (DSP_BUFFER_SIZE - dspBufferLen) / dspFragSize); // fragments
        writed(MMU_PARAM_THREAD IOCTL_ARG1 + 4, DSP_BUFFER_SIZE / dspFragSize);
        writed(MMU_PARAM_THREAD IOCTL_ARG1 + 8, dspFragSize);
        writed(MMU_PARAM_THREAD IOCTL_ARG1 + 12, DSP_BUFFER_SIZE - dspBufferLen);
        return 0;
    case 0x500F: // SNDCTL_DSP_GETCAPS
        writed(MMU_PARAM_THREAD IOCTL_ARG1, DSP_CAP_TRIGGER);
        return 0;
    case 0x5010: // SNDCTL_DSP_SETTRIGGER
        if (readd(MMU_PARAM_THREAD IOCTL_ARG1) & PCM_ENABLE_OUTPUT) {
            SDL_PauseAudio(0);
            pauseAtLen = 0xFFFFFFFF;
        } else {            
            pauseAtLen = dspBufferLen;
            if (pauseAtLen==0) {
                SDL_PauseAudio(0);
            }
        }
        return 0;
    case 0x5012: // SNDCTL_DSP_GETOPTR
        writed(MMU_PARAM_THREAD IOCTL_ARG1, 0); // Total # of bytes processed
        writed(MMU_PARAM_THREAD IOCTL_ARG1 + 4, 0); // # of fragment transitions since last time
        if (pauseEnabled()) {
            writed(MMU_PARAM_THREAD IOCTL_ARG1 + 8, pauseAtLen); // Current DMA pointer value
            if (pauseAtLen==0) {
                SDL_PauseAudio(0);
            }
        } else {
            writed(MMU_PARAM_THREAD IOCTL_ARG1 + 8, dspBufferLen); // Current DMA pointer value
        }
        return 0;
    case 0x5016: // SNDCTL_DSP_SETDUPLEX
        return -K_EINVAL;
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
    if (events & K_POLLOUT) {
        if (dspWaitingToWriteThread)
            kpanic("%d tried to wait on a input read, but %d is already waiting.", thread->id, dspWaitingToWriteThread->id);
        dspWaitingToWriteThread = thread;
        addClearOnWake(thread, &dspWaitingToWriteThread);
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
