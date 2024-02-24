#pragma once
#ifndef CPU_H
#define CPU_H

#include "ins.h"
#include <stdlib.h>
#include <stdint.h>

#define MEM_MAX 65536   //(64 * 1024)

typedef uint8_t byte;
typedef uint16_t word;

struct RAM {
   byte* data;
};

struct CPU {
   word pc;
   word sp;

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

   uint32_t cycles;
};

void resetCPU(struct CPU* cpu, word sPC);
struct RAM* initRAM();
void freeRAM(struct RAM* ram);
int exec(struct CPU* cpu, struct RAM* ram, uint32_t insCount);

#endif // CPU_H
