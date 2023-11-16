#pragma once
#ifndef CPU_H
#define CPU_H

#include <stdlib.h>

#define MEM_MAX 65536      //(64 * 1024)
#define INS_JSR 0x20       //Jump to Subroutine. Takes 6 cycles.
#define INS_RTS 0x60       //Returns to the calling routine. Takes 6 cycles.
#pragma region ADC
#define INS_ADC_IM 0x69    //ADC Immediate. Takes 2 cycles.
#define INS_ADC_ZP 0x65    //ADC Zero Page. Takes 3 cycles
#define INS_ADC_ZPX 0x75   //ADC Zero Page X. Takes 4 cycles.
#define INS_ADC_ABS 0xAD   //ADC Absolute. Takes 4 cycles.
#define INS_ADC_ABSX 0xBD  //ADC Absolute X. Takes 4(+1) cycles.
#define INS_ADC_ABSY 0xB9  //ADC Absolute Y. Takes 4(+1) cycles
#define INS_ADC_INDX 0xA1  //ADC Indirect X. Takes 6 cycles.
#define INS_ADC_INDY 0xB1  //ADC Indirect Y. Takes 5(+1) cycles.
#pragma endregion
#pragma region LDA
#define INS_LDA_IM 0xA9    //LDA immediate. Takes 2 cycles.
#define INS_LDA_ZP 0xA5    //LDA Zero page. Takes 3 cycles.
#define INS_LDA_ZPX 0xB5   //LDA Zero page X. Takes 4 cycles.
#define INS_LDA_ABS 0xAD   //LDA Absolute. Takes 4 cycles.
#define INS_LDA_ABSX 0xBD  //LDA Absolute X. Takes 4(+1) cycles.
#define INS_LDA_ABSY 0xB9  //LDA Absolute Y. Takes 4(+1) cycles
#define INS_LDA_INDX 0xA1  //LDA Indirect X. Takes 6 cycles.
#define INS_LDA_INDY 0xB1  //LDA Indirect Y. Takes 5(+1) cycles.
#pragma endregion
#pragma region LDX
#define INS_LDX_IM 0xA2    //LDX immediate. Takes 2 cycles.
#define INS_LDX_ZP 0xA6    //LDX Zero page. Takes 3 cycles.
#define INS_LDX_ZPY 0xB6   //LDX Zero page Y. Takes 4 cycles.
#define INS_LDX_ABS 0xAE   //LDX Absolute. Takes 4 cycles.
#define INS_LDX_ABSY 0xBE  //LDX Absolute Y. Takes 4(+1) cycles   
#pragma endregion
#pragma region LDY
#define INS_LDY_IM 0xA0    //LDY immediate. Takes 2 cycles.
#define INS_LDY_ZP 0xA4    //LDY Zero page. Takes 3 cycles.
#define INS_LDY_ZPX 0xB4   //LDY Zero page X. Takes 4 cycles.
#define INS_LDY_ABS 0xAC   //LDY Absolute. Takes 4 cycles.
#define INS_LDY_ABSX 0xBC  //LDY Absolute X. Takes 4(+1) cycles 
#pragma endregion


typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int u32;

struct RAM {
   byte* data;
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

   u32 cycles;
};

typedef struct RAM RAM;
typedef struct CPU CPU;

void resetCPU(CPU* cpu, word sPC);
RAM* initRAM();
void freeRAM(RAM* ram);
int exec(CPU* cpu, RAM* ram, u32 insCount);

#endif // !CPU_H
