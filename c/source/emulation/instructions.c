#include "cpu.h"
#include "op.h"

void instruction_daa(struct CPU* cpu) {
    if (((AL & 0x0F)>0x09) || getAF(cpu)) {
		if ((AL > 0x99) || getCF(cpu)) {
			AL+=0x60;
			addFlag(CF);
		} else {
			removeFlag(CF);
		}
		AL+=0x06;
		addFlag(AF);
	} else {
		if ((AL > 0x99) || getCF(cpu)) {
			AL+=0x60;
			addFlag(CF);
		} else {
			removeFlag(CF);
		}
		removeFlag(AF);
	}
	setSF(cpu,(AL & 0x80));
	setZF(cpu,(AL == 0));
	setPF(cpu,parity_lookup[AL]);
	cpu->lazyFlags=FLAGS_NONE;
}

void instruction_das(struct CPU* cpu) {
    U8 osigned=AL & 0x80;
	if (((AL & 0x0f) > 9) || getAF(cpu)) {
		if ((AL>0x99) || getCF(cpu)) {
			AL-=0x60;
			addFlag(CF);
		} else {
			setCF(cpu,(AL<=0x05));
		}
		AL-=6;
		addFlag(AF);
	} else {
		if ((AL>0x99) || getCF(cpu)) {
			AL-=0x60;
			addFlag(CF);
		} else {
			removeFlag(CF);
		}
		removeFlag(AF);
	}
	setOF(cpu,osigned && ((AL & 0x80)==0));
	setSF(cpu,(AL & 0x80));
	setZF(cpu,(AL==0));
	setPF(cpu,parity_lookup[AL]);
	cpu->lazyFlags=FLAGS_NONE;
}

void instruction_aaa(struct CPU* cpu) {
    setSF(cpu,((AL>=0x7a) && (AL<=0xf9)));
	if ((AL & 0xf) > 9) {
		setOF(cpu,(AL & 0xf0)==0x70);
		AX += 0x106;
		addFlag(CF);
		setZF(cpu,(AL == 0));
		addFlag(AF);
	} else if (getAF(cpu)) {
		AX += 0x106;
		removeFlag(OF);
		addFlag(CF);
		removeFlag(ZF);
		addFlag(AF);
	} else {
		removeFlag(OF);
		removeFlag(CF);
		setZF(cpu,(AL == 0));
		removeFlag(AF);
	}
	setPF(cpu,parity_lookup[AL]);
	AL &= 0x0F;
	cpu->lazyFlags=FLAGS_NONE;
}

void instruction_aas(struct CPU* cpu) {
    if ((AL & 0x0f)>9) {
		setSF(cpu,(AL>0x85));
		AX -= 0x106;
		removeFlag(OF);
		addFlag(CF);
		addFlag(AF);
	} else if (getAF(cpu)) {
		setOF(cpu,((AL>=0x80) && (AL<=0x85)));
		setSF(cpu,(AL<0x06) || (AL>0x85));
		AX -= 0x106;
		addFlag(CF);
		addFlag(AF);
	} else {
		setSF(cpu,(AL>=0x80));
		removeFlag(OF);
		removeFlag(CF);
		removeFlag(AF);
	}
	setZF(cpu,(AL == 0));
	setPF(cpu,parity_lookup[AL]);
	AL &= 0x0F;
	cpu->lazyFlags=FLAGS_NONE;
}