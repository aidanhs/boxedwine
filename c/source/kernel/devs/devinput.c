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
#include "kscheduler.h"

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

static U32 freeEventsInitialized;
static struct EventData eventData[MAX_NUMBER_OF_EVENTS];
static struct EventData* freeEvents;

struct InputEventQueue {	
	struct EventData* firstQueuedEvent;
	struct EventData* lastQueuedEvent;
	U32 asyncProcessId;
	U32 asyncProcessFd;
	U16 bustype;
	U16 vendor;
	U16 product;
	U16 version;
	const char* name;
	U32 mask;
};

static struct InputEventQueue touchEvents;
static struct InputEventQueue keyboardEvents;
static struct InputEventQueue mouseEvents;

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

BOOL input_init(struct KProcess* process, struct OpenNode* node) {
	if (!freeEventsInitialized) {
		U32 i;

		for (i=0;i<MAX_NUMBER_OF_EVENTS;i++) {
			eventData[i].next = freeEvents;
			freeEvents = &eventData[i];
		}
		freeEventsInitialized = 1;
	}
	return TRUE;
}

S64 input_length(struct OpenNode* node) {
	return 0;
}

BOOL input_setLength(struct OpenNode* node, S64 len) {
	return FALSE;
}

S64 input_getFilePointer(struct OpenNode* node) {
	return 0;
}

S64 input_seek(struct OpenNode* node, S64 pos) {
	return 0;
}

U32 input_read(struct InputEventQueue* queue, struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	U32 result = 0;

	while (queue->firstQueuedEvent && result+16<=len) {
		struct EventData* e = queue->firstQueuedEvent;

		writed(memory, address, (U32) (e->time / 1000000)); // seconds
        writed(memory, address+4, (U32) (e->time % 1000000)); // microseconds
        writew(memory, address+8, e->type);
        writew(memory, address+10, e->code);
        writed(memory, address+12, e->value);
		result+=16;
		address+=16;
		queue->firstQueuedEvent = e->next;
		freeEventData(e);
	}
	if (!queue->firstQueuedEvent) {
		queue->lastQueuedEvent = 0;
	}
	return result;
}

U32 input_write(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	return len;
}

void input_close(struct InputEventQueue* queue, struct OpenNode* node) {
	struct EventData* e = queue->firstQueuedEvent;

	freeOpenNode(node);
	while (e) {
		struct EventData* next = e->next;
		freeEventData(e);
		e = next;
	}
	queue->firstQueuedEvent = 0;
	queue->lastQueuedEvent = 0;
	queue->asyncProcessId = 0;
	queue->asyncProcessFd = 0;
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

U32 input_ioctl(struct InputEventQueue* queue, struct KThread* thread, struct OpenNode* node, U32 request) {
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
			writew(memory, buffer, queue->bustype);
            writew(memory, buffer+2, queue->vendor);
			writew(memory, buffer+4, queue->product);
			writew(memory, buffer+6, queue->version);
            return 0;
        }
        case 0x4506: { // EVIOCGNAME
            U32 len = (request & 0x1fff0000) >> 16;
            U32 buffer = IOCTL_ARG1;
			return writeNativeString2(memory, buffer, queue->name, len);
        }
        case 0x4520: { // EVIOCGBIT
            U32 len = (request & 0x1fff0000) >> 16;
            U32 buffer = IOCTL_ARG1;
            if (len<4)
                kpanic("Bad length for EVIOCGBIT: %d", len);
            writed(memory, buffer, queue->mask);
            return 4;
        }
        case 0x540B: // TCFLSH
            return 0;		
    }
	return -1;
}

void input_setAsync(struct InputEventQueue* queue, struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
	if (isAsync) {
		if (queue->asyncProcessId && queue->asyncProcessId!=process->id) {
			kpanic("touch_setAsync only supports one process: %d tried to attached but %d already has it", process->id, queue->asyncProcessId);
		} else {
			queue->asyncProcessId = process->id;
			queue->asyncProcessFd = fd;
		}
	} else {
		if (process->id == queue->asyncProcessId) {
			queue->asyncProcessId = 0;
			queue->asyncProcessFd = 0;
		}
	}
}

BOOL input_isAsync(struct InputEventQueue* queue, struct OpenNode* node, struct KProcess* process) {
	return queue->asyncProcessId == process->id;
}

BOOL input_isWriteReady(struct InputEventQueue* queue, struct OpenNode* node) {
	return freeEvents!=0;
}

BOOL input_isReadReady(struct InputEventQueue* queue, struct OpenNode* node) {
	return queue->firstQueuedEvent!=0;
}

U32 input_map(struct OpenNode* node, struct Memory* memory, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
	return 0;
}

BOOL input_canMap(struct OpenNode* node) {
	return FALSE;
}

BOOL touch_init(struct KProcess* process, struct OpenNode* node) {
	touchEvents.bustype = 3;
	touchEvents.vendor = 0;
	touchEvents.product = 0;
	touchEvents.version = 0;
	touchEvents.name = "BoxedWine touchpad";
	touchEvents.mask = (1<<K_EV_SYN)|(1<<K_EV_KEY)|(1<<K_EV_ABS);
	return input_init(process, node);
}

U32 touch_read(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	return input_read(&touchEvents, memory, node, address, len);
}

void touch_close(struct OpenNode* node) {
	input_close(&touchEvents, node);
}

BOOL touch_isReadReady(struct OpenNode* node) {
	return input_isReadReady(&touchEvents, node);
}

BOOL touch_isWriteReady(struct OpenNode* node) {
	return input_isWriteReady(&touchEvents, node);
}

U32 touch_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
	struct Memory* memory = thread->process->memory;
	struct CPU* cpu = &thread->cpu;

    switch (request & 0xFFFF) {
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
		default:
			return input_ioctl(&touchEvents, thread, node, request);
    }
	return -1;
}

void touch_setAsync(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
	input_setAsync(&touchEvents, node, process, fd, isAsync);
}

BOOL touch_isAsync(struct OpenNode* node, struct KProcess* process) {
	return input_isAsync(&touchEvents, node, process);
}

struct NodeAccess touchInputAccess = {touch_init, input_length, input_setLength, input_getFilePointer, input_seek, touch_read, input_write, touch_close, input_map, input_canMap, touch_ioctl, touch_setAsync, touch_isAsync, touch_isWriteReady, touch_isReadReady};

BOOL mouse_init(struct KProcess* process, struct OpenNode* node) {
	mouseEvents.bustype = 3;
	mouseEvents.vendor = 0x046d;
	mouseEvents.product = 0xc52b;
	mouseEvents.version = 0x0111;
	mouseEvents.name = "BoxedWine mouse";
	mouseEvents.mask = (1<<K_EV_SYN)|(1<<K_EV_KEY)|(1<<K_EV_REL);
	return input_init(process, node);
}

U32 mouse_read(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	return input_read(&mouseEvents, memory, node, address, len);
}

void mouse_close(struct OpenNode* node) {
	input_close(&mouseEvents, node);
}

BOOL mouse_isReadReady(struct OpenNode* node) {
	return input_isReadReady(&mouseEvents, node);
}

BOOL mouse_isWriteReady(struct OpenNode* node) {
	return input_isWriteReady(&mouseEvents, node);
}

U32 mouse_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
	struct Memory* memory = thread->process->memory;
	struct CPU* cpu = &thread->cpu;

    switch (request & 0xFFFF) {
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
			writeb(memory, buffer, (1 << K_REL_X)|(1 << K_REL_Y));
            return 1;
        }
        case 0x4523: { // EVIOCGBIT, EV_ABS
            U32 len = (request & 0x1fff0000) >> 16;
            U32 buffer = IOCTL_ARG1;
            zeroMemory(memory, buffer, len);
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
            writeAbs(memory, address, 0, 0, 0);
            return 0;
        }
        case 0x4541: { // EVIOCGABS (ABS_Y)
            int len = (request & 0x1fff0000) >> 16;
            int address = IOCTL_ARG1;
            if (len<24)
                kpanic("Bad length for EVIOCGABS (ABS_X)");
            writeAbs(memory, address, 0, 0, 0);
            return 0;
        }
		default:
			return input_ioctl(&mouseEvents, thread, node, request);
    }
	return -1;
}

void mouse_setAsync(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
	input_setAsync(&mouseEvents, node, process, fd, isAsync);
}

BOOL mouse_isAsync(struct OpenNode* node, struct KProcess* process) {
	return input_isAsync(&mouseEvents, node, process);
}

struct NodeAccess mouseInputAccess = {mouse_init, input_length, input_setLength, input_getFilePointer, input_seek, mouse_read, input_write, mouse_close, input_map, input_canMap, mouse_ioctl, mouse_setAsync, mouse_isAsync, mouse_isWriteReady, mouse_isReadReady};

BOOL keyboard_init(struct KProcess* process, struct OpenNode* node) {
	keyboardEvents.bustype = 0x11;
	keyboardEvents.vendor = 1;
	keyboardEvents.product = 1;
	keyboardEvents.version = 0xab41;
	keyboardEvents.name = "BoxedWine Keyboard";
	keyboardEvents.mask = (1<<K_EV_SYN)|(1<<K_EV_KEY);
	return input_init(process, node);
}

U32 keyboard_read(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	return input_read(&keyboardEvents, memory, node, address, len);
}

void keyboard_close(struct OpenNode* node) {
	input_close(&keyboardEvents, node);
}

BOOL keyboard_isReadReady(struct OpenNode* node) {
	return input_isReadReady(&keyboardEvents, node);
}

BOOL keyboard_isWriteReady(struct OpenNode* node) {
	return input_isWriteReady(&keyboardEvents, node);
}

U32 keyboard_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
	struct Memory* memory = thread->process->memory;
	struct CPU* cpu = &thread->cpu;

    switch (request & 0xFFFF) {
        case 0x4521: { // EVIOCGBIT, EV_KEY
            U32 len = (request & 0x1fff0000) >> 16;
            U32 buffer = IOCTL_ARG1;
            zeroMemory(memory, buffer, len);
            writeBit(memory, buffer, K_KEY_ESC);
            writeBit(memory, buffer, K_KEY_1);
            writeBit(memory, buffer, K_KEY_2);
            writeBit(memory, buffer, K_KEY_3);
            writeBit(memory, buffer, K_KEY_4);
            writeBit(memory, buffer, K_KEY_5);
            writeBit(memory, buffer, K_KEY_6);
            writeBit(memory, buffer, K_KEY_7);
            writeBit(memory, buffer, K_KEY_8);
            writeBit(memory, buffer, K_KEY_9);
            writeBit(memory, buffer, K_KEY_0);
            writeBit(memory, buffer, K_KEY_MINUS);
            writeBit(memory, buffer, K_KEY_EQUAL);
            writeBit(memory, buffer, K_KEY_BACKSPACE);
            writeBit(memory, buffer, K_KEY_TAB);
            writeBit(memory, buffer, K_KEY_Q);
            writeBit(memory, buffer, K_KEY_W);
            writeBit(memory, buffer, K_KEY_E);
            writeBit(memory, buffer, K_KEY_R);
            writeBit(memory, buffer, K_KEY_T);
            writeBit(memory, buffer, K_KEY_Y);
            writeBit(memory, buffer, K_KEY_U);
            writeBit(memory, buffer, K_KEY_I);
            writeBit(memory, buffer, K_KEY_O);
            writeBit(memory, buffer, K_KEY_P);
            writeBit(memory, buffer, K_KEY_LEFTBRACE);
            writeBit(memory, buffer, K_KEY_RIGHTBRACE);
            writeBit(memory, buffer, K_KEY_ENTER);
            writeBit(memory, buffer, K_KEY_LEFTCTRL);
            writeBit(memory, buffer, K_KEY_A);
            writeBit(memory, buffer, K_KEY_S);
            writeBit(memory, buffer, K_KEY_D);
            writeBit(memory, buffer, K_KEY_F);
            writeBit(memory, buffer, K_KEY_G);
            writeBit(memory, buffer, K_KEY_H);
            writeBit(memory, buffer, K_KEY_J);
            writeBit(memory, buffer, K_KEY_K);
            writeBit(memory, buffer, K_KEY_L);
            writeBit(memory, buffer, K_KEY_SEMICOLON);
            writeBit(memory, buffer, K_KEY_APOSTROPHE);
            writeBit(memory, buffer, K_KEY_GRAVE);
            writeBit(memory, buffer, K_KEY_LEFTSHIFT);
            writeBit(memory, buffer, K_KEY_BACKSLASH);
            writeBit(memory, buffer, K_KEY_Z);
            writeBit(memory, buffer, K_KEY_X);
            writeBit(memory, buffer, K_KEY_C);
            writeBit(memory, buffer, K_KEY_V);
            writeBit(memory, buffer, K_KEY_B);
            writeBit(memory, buffer, K_KEY_N);
            writeBit(memory, buffer, K_KEY_M);
            writeBit(memory, buffer, K_KEY_COMMA);
            writeBit(memory, buffer, K_KEY_DOT);
            writeBit(memory, buffer, K_KEY_SLASH);
            writeBit(memory, buffer, K_KEY_RIGHTSHIFT);
            writeBit(memory, buffer, K_KEY_LEFTALT);
            writeBit(memory, buffer, K_KEY_SPACE);
            writeBit(memory, buffer, K_KEY_CAPSLOCK);
            writeBit(memory, buffer, K_KEY_F1);
            writeBit(memory, buffer, K_KEY_F2);
            writeBit(memory, buffer, K_KEY_F3);
            writeBit(memory, buffer, K_KEY_F4);
            writeBit(memory, buffer, K_KEY_F5);
            writeBit(memory, buffer, K_KEY_F6);
            writeBit(memory, buffer, K_KEY_F7);
            writeBit(memory, buffer, K_KEY_F8);
            writeBit(memory, buffer, K_KEY_F9);
            writeBit(memory, buffer, K_KEY_F10);
            writeBit(memory, buffer, K_KEY_NUMLOCK);
            writeBit(memory, buffer, K_KEY_SCROLLLOCK);

            writeBit(memory, buffer, K_KEY_F11);
            writeBit(memory, buffer, K_KEY_F12);
            writeBit(memory, buffer, K_KEY_RIGHTCTRL);
            writeBit(memory, buffer, K_KEY_RIGHTALT);
            writeBit(memory, buffer, K_KEY_HOME);
            writeBit(memory, buffer, K_KEY_UP);
            writeBit(memory, buffer, K_KEY_PAGEUP);
            writeBit(memory, buffer, K_KEY_LEFT);
            writeBit(memory, buffer, K_KEY_RIGHT);
            writeBit(memory, buffer, K_KEY_END);
            writeBit(memory, buffer, K_KEY_DOWN);
            writeBit(memory, buffer, K_KEY_PAGEDOWN);
            writeBit(memory, buffer, K_KEY_INSERT);
            writeBit(memory, buffer, K_KEY_DELETE);
            writeBit(memory, buffer, K_KEY_PAUSE);
            return (K_KEY_PAUSE+7)/8;
        }
        case 0x4522: { // EVIOCGBIT, EV_REL
            U32 len = (request & 0x1fff0000) >> 16;
            U32 buffer = IOCTL_ARG1;
            zeroMemory(memory, buffer, len);
            writeb(memory, buffer, 0);
            return 1;
        }
        case 0x4523: { // EVIOCGBIT, EV_ABS
            U32 len = (request & 0x1fff0000) >> 16;
            U32 buffer = IOCTL_ARG1;
            zeroMemory(memory, buffer, len);
            writeb(memory, buffer, 0);
            return 1;
        }
        case 0x4531: { // EVIOCGBIT, EV_LED
            int len = (request & 0x1fff0000) >> 16;
            U32 buffer = IOCTL_ARG1;
            zeroMemory(memory, buffer, len);
            return 1;
        }
		default:
			return input_ioctl(&keyboardEvents, thread, node, request);
    }
	return -1;
}

void keyboard_setAsync(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
	input_setAsync(&keyboardEvents, node, process, fd, isAsync);
}

BOOL keyboard_isAsync(struct OpenNode* node, struct KProcess* process) {
	return input_isAsync(&keyboardEvents, node, process);
}

struct NodeAccess keyboardInputAccess = {keyboard_init, input_length, input_setLength, input_getFilePointer, input_seek, keyboard_read, input_write, keyboard_close, input_map, input_canMap, keyboard_ioctl, keyboard_setAsync, keyboard_isAsync, keyboard_isWriteReady, keyboard_isReadReady};

void queueEvent(struct InputEventQueue* queue, U32 type, U32 code, U32 value, U64 time) {
	struct EventData* data = allocEventData();
	if (data) {
		data->time = time;
		data->type = type;
		data->code = code;
		data->value = value;
		data->next = 0;
		if (queue->lastQueuedEvent) {
			queue->lastQueuedEvent->next = data;
		}
		if (!queue->firstQueuedEvent) {
			queue->firstQueuedEvent = data;
		}
		queue->lastQueuedEvent = data;
	}
}

/*
void onMouseMove(U32 x, U32 y) {
	U32 send = 0;

    if (x!=lastX) {
        queueEvent(&mouseEvents, K_EV_REL, K_REL_X, x-lastX);
        lastX = x;
        send = 1;
    }
    if (y!=lastY) {
        queueEvent(&mouseEvents, K_EV_REL, K_REL_Y, y-lastY);
        lastY = y;
        send = 1;
    }
    if (send) {
        queueEvent(&mouseEvents, K_EV_SYN, K_SYN_REPORT, 0);
        if (mouseEvents.asyncProcessId) {
			struct KProcess* process = getProcessById(mouseEvents.asyncProcessId);
			if (process)
				signalProcess(process, K_SIGIO);
        }
    }
}

void onMouseButtonUp(U32 button) {
	if (button == 0)
        queueEvent(&mouseEvents, K_EV_KEY, K_BTN_LEFT, 0);
    else if (button == 2)
        queueEvent(&mouseEvents, K_EV_KEY, K_BTN_MIDDLE, 0);
    else if (button == 1)
        queueEvent(&mouseEvents, K_EV_KEY, K_BTN_RIGHT, 0);
    else
        return;
    queueEvent(&mouseEvents, K_EV_SYN, K_SYN_REPORT, 0);
    if (mouseEvents.asyncProcessId) {
		struct KProcess* process = getProcessById(mouseEvents.asyncProcessId);
		if (process)
			signalProcess(process, K_SIGIO);
    }
}

void onMouseButtonDown(U32 button) {
	if (button == 0)
        queueEvent(&mouseEvents, K_EV_KEY, K_BTN_LEFT, 1);
    else if (button == 2)
        queueEvent(&mouseEvents, K_EV_KEY, K_BTN_MIDDLE, 1);
    else if (button == 1)
        queueEvent(&mouseEvents, K_EV_KEY, K_BTN_RIGHT, 1);
    else
        return;
    queueEvent(&mouseEvents, K_EV_SYN, K_SYN_REPORT, 0);
    if (mouseEvents.asyncProcessId) {
		struct KProcess* process = getProcessById(mouseEvents.asyncProcessId);
		if (process)
			signalProcess(process, K_SIGIO);
    }
}
*/

void onMouseButtonUp(U32 button) {
	U64 time = getSystemTimeAsMicroSeconds();

	if (button == 0)
        queueEvent(&touchEvents, K_EV_KEY, K_BTN_LEFT, 0, time);
    else if (button == 2)
        queueEvent(&touchEvents, K_EV_KEY, K_BTN_MIDDLE, 0, time);
    else if (button == 1)
        queueEvent(&touchEvents, K_EV_KEY, K_BTN_RIGHT, 0, time);
    else
        return;
    queueEvent(&touchEvents, K_EV_SYN, K_SYN_REPORT, 0, time);
    if (touchEvents.asyncProcessId) {
		struct KProcess* process = getProcessById(touchEvents.asyncProcessId);
		if (process)
			signalIO(process, K_POLL_IN, 0, touchEvents.asyncProcessFd);		
    }
	wakeThreads(WAIT_FD);
}

void onMouseButtonDown(U32 button) {
	U64 time = getSystemTimeAsMicroSeconds();

	if (button == 0)
        queueEvent(&touchEvents, K_EV_KEY, K_BTN_LEFT, 1, time);
    else if (button == 2)
        queueEvent(&touchEvents, K_EV_KEY, K_BTN_MIDDLE, 1, time);
    else if (button == 1)
        queueEvent(&touchEvents, K_EV_KEY, K_BTN_RIGHT, 1, time);
    else
        return;
    queueEvent(&touchEvents, K_EV_SYN, K_SYN_REPORT, 0, time);
    if (touchEvents.asyncProcessId) {
		struct KProcess* process = getProcessById(touchEvents.asyncProcessId);
		if (process)
			signalIO(process, K_POLL_IN, 0, touchEvents.asyncProcessFd);		
    }
	wakeThreads(WAIT_FD);
}

void onMouseMove(U32 x, U32 y) {
	U32 send = 0;
	U64 time = getSystemTimeAsMicroSeconds();

    if (x!=lastX) {
        lastX = x;
        queueEvent(&touchEvents, K_EV_ABS, K_ABS_X, x, time);
        send = 1;
    }
    if (y!=lastY) {
        lastY = y;
        queueEvent(&touchEvents, K_EV_ABS, K_ABS_Y, y, time);
        send = 1;
    }
    if (send) {
        queueEvent(&touchEvents, K_EV_SYN, K_SYN_REPORT, 0, time);
        if (touchEvents.asyncProcessId) {
			struct KProcess* process = getProcessById(touchEvents.asyncProcessId);
			if (process)
				signalIO(process, K_POLL_IN, 0, touchEvents.asyncProcessFd);		
        }
		wakeThreads(WAIT_FD);
    }
}

void onKeyDown(U32 code) {
	U64 time = getSystemTimeAsMicroSeconds();

    if (code == 0)
        return;
    queueEvent(&keyboardEvents, K_EV_KEY, code, 1, time);
    queueEvent(&keyboardEvents, K_EV_SYN, K_SYN_REPORT, 0, time);
    if (keyboardEvents.asyncProcessId) {
		struct KProcess* process = getProcessById(keyboardEvents.asyncProcessId);
		if (process)
			signalIO(process, K_POLL_IN, 0, keyboardEvents.asyncProcessFd);		
    }
	wakeThreads(WAIT_FD);
}

void onKeyUp(U32 code) {
	U64 time = getSystemTimeAsMicroSeconds();

    if (code == 0)
        return;
    queueEvent(&keyboardEvents, K_EV_KEY, code, 0, time);
    queueEvent(&keyboardEvents, K_EV_SYN, K_SYN_REPORT, 0, time);
    if (keyboardEvents.asyncProcessId) {
		struct KProcess* process = getProcessById(keyboardEvents.asyncProcessId);
		if (process)
			signalIO(process, K_POLL_IN, 0, keyboardEvents.asyncProcessFd);		
    }
	wakeThreads(WAIT_FD);
}
