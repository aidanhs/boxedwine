#ifndef __REG_H__
#define __REG_H__

#include "platform.h"

struct Reg {
	union {
		U32 u32;
		union {
			union {
				U16 u16;
				struct {
					U8 u8;
					U8 h8;
				};
			};
			U16 h16;
		};
	};
};

#endif