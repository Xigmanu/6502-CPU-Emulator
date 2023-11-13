#pragma once
#ifndef CPU_H
#define CPU_H
#define DEBUG

#ifdef _cplusplus
extern "C" {
#endif // _cplusplus

#include <stdlib.h>

#define MEM_MAX 65536      //(64 * 1024)
#define INS_JSR 0x20       //Jump to Subroutine. Takes 6 cycles.
#define INS_RTS 0x60       //Returns to the calling routine. Takes 6 cycles.
#define INS_LDA_IM 0xA9    //Load accumulator immediate. Takes 2 cycles.
#define INS_LDA_ZP 0xA5    //Zero page. Takes 3 cycles.
#define INS_LDA_ZPX 0xB5   //Zero page X. Takes 4 cycles.
#define INS_LDA_ABS 0xAD   //Absolute. Takes 4 cycles.
#define INS_LDA_ABSX 0xBD  //Absolute X. Takes 4(+1) cycles.
#define INS_LDA_ABSY 0xB9  //Absolute Y. Takes 4(+1) cycles
#define INS_LDA_INDX 0xA1  //Indirect X. Takes 6 cycles.

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

#ifdef __cplusplus
}
#endif
#endif // !CPU_H
