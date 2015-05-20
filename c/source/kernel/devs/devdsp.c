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

BOOL isDspOpen = 0;
U32 dspFmt = AFMT_U8;
U32 sdlFmt = AUDIO_U8;
U32 dspChannels = 1;
U32 dspFreq = 8000;
#define DSP_BUFFER_SIZE 1024*64
U8 dspBuffer[DSP_BUFFER_SIZE];
S32 dspBufferLen;
U32 dspBufferPos;

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
	memcpy(stream, dspBuffer+dspBufferPos, available);
    result=available;
    dspBufferPos += available;
    dspBufferLen -= available;
	if (dspBufferPos==DSP_BUFFER_SIZE) {
		dspBufferPos = 0;
		if (dspBufferLen) {
			len-=available;
			available = dspBufferLen;
			if (available>len) {
				available = len;
			}
			memcpy(stream+result, dspBuffer+dspBufferPos, available);
			result+=available;
			dspBufferPos += available;
			dspBufferLen -= available;
		}
	}
	wakeThreads(WAIT_DSP);
	if (result<len) {
		memset(stream+result, 0, len-result);
	}
}

void openAudio() {
	SDL_AudioSpec want;

	memset(&want, 0, sizeof(SDL_AudioSpec));
	want.freq = dspFreq;
	want.format = sdlFmt;
	want.channels = dspChannels;
	want.samples = 4096;
	want.callback = audioCallback;  // you wrote this function elsewhere.

	if (SDL_OpenAudio(&want, 0) < 0) {
		printf("Failed to open audio: %s\n", SDL_GetError());
	} 
	isDspOpen = 1;
	SDL_PauseAudio(0);
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

U32 dsp_read(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	return 0;
}

extern struct KThread* currentThread;

U32 dsp_write(struct Memory* memory, struct OpenNode* node, U32 address, U32 l) {
	S32 len = (S32)l;
	S32 available;
	S32 result;
	U32 startPos;
	if (len+dspBufferLen>DSP_BUFFER_SIZE) {
		currentThread->waitType = WAIT_DSP;
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
	memcopyToNative(memory, address, (char*)dspBuffer+startPos, available);
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
		memcopyToNative(memory, address, (char*)dspBuffer+startPos, available);
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
	struct Memory* memory = thread->process->memory;
	struct CPU* cpu = &thread->cpu;
	//BOOL read = request & 0x40000000;
	BOOL write = request & 0x80000000;

	switch (request & 0xFFFF) {
	case 0x5000: // SNDCTL_DSP_RESET
		return 0;
	case 0x5002:  // SNDCTL_DSP_SPEED 
		if (len!=4) {
			kpanic("SNDCTL_DSP_STEREO was expecting a len of 4");
		}
		dspFreq = readd(memory, IOCTL_ARG1);
		return 0;
	case 0x5003: { // SNDCTL_DSP_STEREO
		U32 fmt;

		if (len!=4) {
			kpanic("SNDCTL_DSP_STEREO was expecting a len of 4");
		}
		fmt = readd(memory, IOCTL_ARG1);
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
			writed(memory, IOCTL_ARG1, dspChannels-1);
		return 0;
	}
	case 0x5005: { // SNDCTL_DSP_SETFMT 
		U32 fmt;

		if (len!=4) {
			kpanic("SNDCTL_DSP_SETFMT was expecting a len of 4");
		}
		fmt = readd(memory, IOCTL_ARG1);
		if (fmt!=0 && fmt!=dspFmt) {
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
			writed(memory, IOCTL_ARG1, dspFmt);
		else if (dspFmt!=fmt) {
			kpanic("SNDCTL_DSP_SETFMT dspFmt!=fmt and can't write result");
		}
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

BOOL dsp_isWriteReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)==K_O_RDONLY;
}

BOOL dsp_isReadReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

U32 dsp_map(struct OpenNode* node, struct Memory* memory, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
	return 0;
}

BOOL dsp_canMap(struct OpenNode* node) {
	return FALSE;
}

struct NodeAccess dspAccess = {dsp_init, dsp_length, dsp_setLength, dsp_getFilePointer, dsp_seek, dsp_read, dsp_write, dsp_close, dsp_map, dsp_canMap, dsp_ioctl, dsp_setAsync, dsp_isAsync, dsp_isWriteReady, dsp_isReadReady};
