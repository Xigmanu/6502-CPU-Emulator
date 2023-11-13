#pragma once
#ifndef CPU_H
#define CPU_H
#define DEBUG

#include <stdlib.h>

#define MEM_MAX 65536   //(64 * 1024)
#define INS_JSR 0x20    //Jump to Subroutine. Takes 6 cycles.
#define INS_RTS 0x60    //Return to the calling routine. Takes 6 cycles.
#define INS_LDA_IM 0xA9 //Immediate. Takes 2 cycles.
#define INS_LDA_ZP

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int u32;

struct Mem {
   byte data[MEM_MAX];
};

struct CPU {
   word pc;
   byte sp;

   byte a;
   byte x;
   byte y;
   byte ps;

   byte c : 1;
   byte z : 1;
   byte i : 1;
   byte d : 1;
   byte b : 1;
   byte v : 1;
   byte n : 1;
};

typedef struct Mem Mem;
typedef struct CPU CPU;

void reset(CPU* cpu, Mem* mem);
void initMem(Mem* mem);
void exec(CPU* cpu, Mem* mem, u32 cycles);

#endif // !CPU_H
