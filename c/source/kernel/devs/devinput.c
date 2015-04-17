#include "platform.h"
#include "nodeaccess.h"
#include "nodetype.h"
#include "filesystem.h"
#include "kerror.h"
#include "log.h"
#include "kthread.h"
#include "kprocess.h"
#include "devinput.h"
#include "ksystem.h"
#include "ksignal.h"

#include <string.h>

extern U32 screenWidth;
extern U32 screenHeight;

static U32 lastX;
static U32 lastY;

struct EventData {
	U64 time;
	U16 type;
	U16 code;
	S32 value;
	struct EventData* next;
};

#define MAX_NUMBER_OF_EVENTS 1024

static struct EventData eventData[MAX_NUMBER_OF_EVENTS];
static struct EventData* freeEvents;
static struct EventData* firstQueuedEvent;
static struct EventData* lastQueuedEvent;
static U32 asyncProcessId;

struct EventData* allocEventData() {
	if (freeEvents) {
		struct EventData* result = freeEvents;
		freeEvents = freeEvents->next;
		return result;
	}
	return 0;
}

void freeEventData(struct EventData* data) {
	data->next = freeEvents;
	freeEvents = data;
}

static const char* name = "BoxedWine touchpad";

BOOL touch_init(struct KProcess* process, struct OpenNode* node) {
	U32 i;

	for (i=0;i<MAX_NUMBER_OF_EVENTS;i++) {
		eventData[i].next = freeEvents;
		freeEvents = &eventData[i];
	}
	return TRUE;
}

S64 touch_length(struct OpenNode* node) {
	return 0;
}

BOOL touch_setLength(struct OpenNode* node, S64 len) {
	return FALSE;
}

S64 touch_getFilePointer(struct OpenNode* node) {
	return 0;
}

S64 touch_seek(struct OpenNode* node, S64 pos) {
	return 0;
}

U32 touch_read(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	U32 result = 0;

	while (firstQueuedEvent && result+16<=len) {
		writed(memory, address, (U32) (firstQueuedEvent->time / 1000000)); // seconds
        writed(memory, address+4, (U32) (firstQueuedEvent->time % 1000000)); // microseconds
        writew(memory, address+8, firstQueuedEvent->type);
        writew(memory, address+10, firstQueuedEvent->code);
        writed(memory, address+12, firstQueuedEvent->value);
		result+=16;
		firstQueuedEvent = firstQueuedEvent->next;
	}
	if (!firstQueuedEvent) {
		lastQueuedEvent = 0;
	}
	return result;
}

U32 touch_write(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	return len;
}

void touch_close(struct OpenNode* node) {
	freeOpenNode(node);
}

void writeBit(struct Memory* memory, U32 address, U32 bit) {
    U32 b = bit/8;
    U32 p = bit % 8;
    U32 value = readb(memory, address+b);
    value|=(1<<p);
    writeb(memory, address+b, value);
}

//    struct input_absinfo {
//        __s32 value;
//        __s32 minimum;
//        __s32 maximum;
//        __s32 fuzz;
//        __s32 flat;
//        __s32 resolution;
//    };
void writeAbs(struct Memory* memory, U32 address, U32 value, U32 min, U32 max) {
    writed(memory, address, value);
    writed(memory, address+4, min);
    writed(memory, address+8, max);
    writed(memory, address+12, 0);
    writed(memory, address+16, 0);
    writed(memory, address+20, 96);
}

U32 touch_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
	struct Memory* memory = thread->process->memory;
	struct CPU* cpu = &thread->cpu;

    switch (request & 0xFFFF) {
        case 0x4502: { // EVIOCGID
//            struct input_id {
//                __u16 bustype;
//                __u16 vendor;
//                __u16 product;
//                __u16 version;
//                };
            U32 len = (request & 0x1fff0000) >> 16;
			U32 buffer = IOCTL_ARG1;
            if (len!=8)
                kpanic("Bad length for EVIOCGID: %d",len);
            writew(memory, buffer, 3);
            writew(memory, buffer+2, 0x046d);
            writew(memory, buffer+4, 0xc52b);
            writew(memory, buffer+6, 0x0111);
            return 0;
        }
        case 0x4506: { // EVIOCGNAME
            U32 len = (request & 0x1fff0000) >> 16;
            U32 buffer = IOCTL_ARG1;
			writeNativeString(memory, buffer, name);
            return strlen(name);
        }
        case 0x4520: { // EVIOCGBIT
            U32 len = (request & 0x1fff0000) >> 16;
            U32 buffer = IOCTL_ARG1;
            if (len<4)
                kpanic("Bad length for EVIOCGBIT");
            writed(memory, buffer, (1<<K_EV_SYN)|(1<<K_EV_KEY)|(1<<K_EV_ABS));
            return 4;
        }
        case 0x540B: // TCFLSH
            return 0;

		case 0x4521: { // EVIOCGBIT, EV_KEY
            U32 len = (request & 0x1fff0000) >> 16;
            U32 buffer = IOCTL_ARG1;
            U32 result = K_BTN_MIDDLE;
            result = (result+7)/8;
			zeroMemory(memory, buffer, len);
            writeBit(memory, buffer, K_BTN_LEFT);
            writeBit(memory, buffer, K_BTN_MIDDLE);
            writeBit(memory, buffer, K_BTN_RIGHT);
            return result;
        }
        case 0x4522: { // EVIOCGBIT, EV_REL
            U32 len = (request & 0x1fff0000) >> 16;
            U32 buffer = IOCTL_ARG1;
			zeroMemory(memory, buffer, len);
            return 1;
        }
        case 0x4523: { // EVIOCGBIT, EV_ABS
            U32 len = (request & 0x1fff0000) >> 16;
            U32 buffer = IOCTL_ARG1;
            zeroMemory(memory, buffer, len);
            writeb(memory, buffer, (1 << K_ABS_X)|(1 << K_ABS_Y));
            return 1;
        }
        case 0x4531: { // EVIOCGBIT, EV_LED
            U32 len = (request & 0x1fff0000) >> 16;
            U32 buffer = IOCTL_ARG1;
            zeroMemory(memory, buffer, len);
            return 1;
        }
        case 0x4540: { // EVIOCGABS (ABS_X)
            U32 len = (request & 0x1fff0000) >> 16;
            U32 address = IOCTL_ARG1;
            if (len<24)
                kpanic("Bad length for EVIOCGABS (ABS_X)");
            writeAbs(memory, address, lastX, 0, screenWidth);
            return 0;
        }
        case 0x4541: { // EVIOCGABS (ABS_Y)
            int len = (request & 0x1fff0000) >> 16;
            int address = IOCTL_ARG1;
            if (len<24)
                kpanic("Bad length for EVIOCGABS (ABS_X)");
            writeAbs(memory, address, lastY, 0, screenHeight);
            return 0;
        }
    }
	return -1;
}

void touch_setAsync(struct OpenNode* node, struct KProcess* process, BOOL isAsync) {
	if (isAsync) {
		if (asyncProcessId && asyncProcessId!=process->id) {
			kwarn("touch_setAsync only supports one process: %d tried to attached but %d already has it", process->id, asyncProcessId);
		} else {
			asyncProcessId = process->id;
		}
	} else {
		if (process->id == asyncProcessId) {
			asyncProcessId = 0;
		}
	}
}

BOOL touch_isAsync(struct OpenNode* node, struct KProcess* process) {
	return 0;
}

BOOL touch_isWriteReady(struct OpenNode* node) {
	return freeEvents!=0;
}

BOOL touch_isReadReady(struct OpenNode* node) {
	return firstQueuedEvent!=0;
}

U32 touch_map(struct OpenNode* node, struct Memory* memory, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
	return 0;
}

BOOL touch_canMap(struct OpenNode* node) {
	return FALSE;
}

struct NodeAccess touchInputAccess = {touch_init, touch_length, touch_setLength, touch_getFilePointer, touch_seek, touch_read, touch_write, touch_close, touch_map, touch_canMap, touch_ioctl, touch_setAsync, touch_isAsync, touch_isWriteReady, touch_isReadReady};

void queueEvent(U32 type, U32 code, U32 value) {
	struct EventData* data = allocEventData();
	if (data) {
		data->time = getSystemTimeAsMicroSeconds();
		data->type = type;
		data->code = code;
		data->value = value;
		if (lastQueuedEvent) {
			lastQueuedEvent->next = data;
		}
		lastQueuedEvent = data;
	}
}

void onMouseMove(U32 x, U32 y) {
	U32 send = 0;

    if (x!=lastX) {
        lastX = x;
        queueEvent(K_EV_ABS, K_ABS_X, x);
        send = 1;
    }
    if (y!=lastY) {
        lastY = y;
        queueEvent(K_EV_ABS, K_ABS_Y, y);
        send = 1;
    }
    if (send) {
        queueEvent(K_EV_SYN, K_SYN_REPORT, 0);
        if (asyncProcessId) {
			struct KProcess* process = getProcessById(asyncProcessId);
			if (process)
				signalProcess(process, K_SIGIO);
        }
    }
}
