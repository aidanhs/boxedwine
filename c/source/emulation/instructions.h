#ifndef __INSTRUCTIONS_H__
#define __INSTRUCTIONS_H__

void instruction_daa(struct CPU* cpu);
void instruction_das(struct CPU* cpu);
void instruction_aaa(struct CPU* cpu);
void instruction_aas(struct CPU* cpu);
void instruction_aad(struct CPU* cpu, U32 value);
void instruction_aam(struct CPU* cpu, U32 value);

#endif