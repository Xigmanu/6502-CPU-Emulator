#pragma once
#ifndef CPU_H
#define CPU_H

#include <stdlib.h>

#define MEM_MAX 65536   //(64 * 1024)
#define JSR 0x20        //Jump to Subroutine. Takes 6 cycles.
#define RTS 0x60        //Returns to the calling routine. Takes 6 cycles.
#pragma region ADC
#define ADC_IM 0x69     //ADC Immediate. Takes 2 cycles.
#define ADC_ZP 0x65     //ADC Zero Page. Takes 3 cycles
#define ADC_ZPX 0x75    //ADC Zero Page X. Takes 4 cycles.
#define ADC_ABS 0xAD    //ADC Absolute. Takes 4 cycles.
#define ADC_ABSX 0xBD   //ADC Absolute X. Takes 4(+1) cycles.
#define ADC_ABSY 0xB9   //ADC Absolute Y. Takes 4(+1) cycles
#define ADC_INDX 0xA1   //ADC Indirect X. Takes 6 cycles.
#define ADC_INDY 0xB1   //ADC Indirect Y. Takes 5(+1) cycles.
#pragma endregion
#pragma region LD_OPCODES
#define LDA_IM 0xA9     //LDA Immediate. Takes 2 cycles.
#define LDA_ZP 0xA5     //LDA Zero page. Takes 3 cycles.
#define LDA_ZPX 0xB5    //LDA Zero page X. Takes 4 cycles.
#define LDA_ABS 0xAD    //LDA Absolute. Takes 4 cycles.
#define LDA_ABSX 0xBD   //LDA Absolute X. Takes 4(+1) cycles.
#define LDA_ABSY 0xB9   //LDA Absolute Y. Takes 4(+1) cycles
#define LDA_INDX 0xA1   //LDA Indirect X. Takes 6 cycles.
#define LDA_INDY 0xB1   //LDA Indirect Y. Takes 5(+1) cycles.

#define LDX_IM 0xA2     //LDX Immediate. Takes 2 cycles.
#define LDX_ZP 0xA6     //LDX Zero page. Takes 3 cycles.
#define LDX_ZPY 0xB6    //LDX Zero page Y. Takes 4 cycles.
#define LDX_ABS 0xAE    //LDX Absolute. Takes 4 cycles.
#define LDX_ABSY 0xBE   //LDX Absolute Y. Takes 4(+1) cycles   

#define LDY_IM 0xA0     //LDY Immediate. Takes 2 cycles.
#define LDY_ZP 0xA4     //LDY Zero page. Takes 3 cycles.
#define LDY_ZPX 0xB4    //LDY Zero page X. Takes 4 cycles.
#define LDY_ABS 0xAC    //LDY Absolute. Takes 4 cycles.
#define LDY_ABSX 0xBC   //LDY Absolute X. Takes 4(+1) cycles 

#define STA_ZP 0x85     //STA Zero page. Takes 3 cycles.
#define STA_ZPX 0x95    //STA Zero page X. Takes 4 cycles.
#define STA_ABS 0x8D    //STA Absolute. Takes 4 cycles.
#define STA_ABSX 0x9D   //STA Absolute X. Takes 5 cycles.
#define STA_ABSY 0x99   //STA Absolute Y. Takes 5 cycles.
#define STA_INDX 0x81   //STA Indirect X. Takes 6 cycles.
#define STA_INDY 0x91   //STA Indirect Y. Takes 6 cycles.

#define STX_ZP 0x86     //STX Zero page. Takes 3 cycles.
#define STX_ZPY 0x96    //STX Zero page Y. Takes 4 cycles.
#define STX_ABS 0x8E    //STX Absolute. Takes 4 cycles.

#define STY_ZP 0x84     //STY Zero page. Takes 3 cycles.
#define STY_ZPX 0x94    //STY Zero page Y. Takes 4 cycles.
#define STY_ABS 0x8C    //STY Absolute. Takes 4 cycles.

#pragma endregion
#pragma region REG_TRANSFERS

#define TAX 0xAA        //Transfer accumulator to X. Takes 2 cycles.
#define TXA 0x8A        //Transfer accumulator to Y. Takes 2 cycles.
#define TAY 0xA8        //Transfer X to accumulator. Takes 2 cycles.
#define TYA 0x98        //Transfer Y to accumulator. Takes 2 cycles.

#pragma endregion
#pragma region STACK_OPS

#define TSX 0xBA        //Transfer SP to X. Takes 2 cycles.
#define TXS 0x9A        //Transfer X to SP. Takes 2 cycles.
#define PHA 0x48        //Push Accumulator. Takes 3 cycles.
#define PHP 0x08        //Push Processor Status. Takes 3 cycles.
#define PLA 0x68        //Pull Accumulator. Takes 4 cycles.
#define PLP 0x28        //Pull Processor Status. Takes 4 cycles.

#pragma endregion
#pragma region LOGICAL

#define AND_IM 0x29     //AND Immediate. Takes 2 cycles.
#define AND_ZP 0x25     //AND Zero Page. Takes 3 cycles.
#define AND_ZPX 0x35    //AND Zero Page X. Takes 4 cycles.
#define AND_ABS 0x2D    //AND Absolute. Takes 4 cycles.
#define AND_ABSX 0x3D   //AND Absolute X. Takes 4(+1) cycles.
#define AND_ABSY 0x39   //AND Absolute Y. Takes 4(+1) cycles.
#define AND_INDX 0x21   //AND Indirect X. Takes 6 cycles.
#define AND_INDY 0x31   //AND Indirect Y. Takes 5(+1) cycles.

#define EOR_IM 0x49     //EOR Immediate. Takes 2 cycles.
#define EOR_ZP 0x45     //EOR Zero page. Takes 3 cycles.
#define EOR_ZPX 0x55    //EOR Zero Page X. Takes 4 cycles.
#define EOR_ABS 0x4D    //EOR Absolute. Takes 4 cycles.
#define EOR_ABSX 0x5D   //EOR Absolute X. Takes 4(+1) cycles.
#define EOR_ABSY 0x59   //EOR Absolute Y. Takes 4(+1) cycles.
#define EOR_INDX 0x41   //EOR Indirect X. Takes 6 cycles.
#define EOR_INDY 0x51   //EOR Indirect Y. Takes 5(+1) cycles.

#pragma endregion

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int u32;

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

   u32 cycles;
};

typedef struct RAM RAM;
typedef struct CPU CPU;

void resetCPU(CPU* cpu, word sPC);
RAM* initRAM();
void freeRAM(RAM* ram);
int exec(CPU* cpu, RAM* ram, u32 insCount);

#endif // !CPU_H
