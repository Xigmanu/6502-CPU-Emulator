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

#define LDA_IM 0xA9     //Load accumulator (Immediate). Takes 2 cycles.
#define LDA_ZP 0xA5     //Load accumulator (Zero page). Takes 3 cycles.
#define LDA_ZPX 0xB5    //Load accumulator (Zero page X). Takes 4 cycles.
#define LDA_ABS 0xAD    //Load accumulator (Absolute). Takes 4 cycles.
#define LDA_ABSX 0xBD   //Load accumulator (Absolute X). Takes 4(+1) cycles.
#define LDA_ABSY 0xB9   //Load accumulator (Absolute Y). Takes 4(+1) cycles
#define LDA_INDX 0xA1   //Load accumulator (Indirect X). Takes 6 cycles.
#define LDA_INDY 0xB1   //Load accumulator (Indirect Y). Takes 5(+1) cycles.

#define LDX_IM 0xA2     //Load X (Immediate). Takes 2 cycles.
#define LDX_ZP 0xA6     //Load X (Zero page). Takes 3 cycles.
#define LDX_ZPY 0xB6    //Load X (Zero page Y). Takes 4 cycles.
#define LDX_ABS 0xAE    //Load X (Absolute). Takes 4 cycles.
#define LDX_ABSY 0xBE   //Load X (Absolute Y). Takes 4(+1) cycles   

#define LDY_IM 0xA0     //Load Y (Immediate). Takes 2 cycles.
#define LDY_ZP 0xA4     //Load Y (Zero page). Takes 3 cycles.
#define LDY_ZPX 0xB4    //Load Y (Zero page X). Takes 4 cycles.
#define LDY_ABS 0xAC    //Load Y (Absolute). Takes 4 cycles.
#define LDY_ABSX 0xBC   //Load Y (Absolute X). Takes 4(+1) cycles 

#define STA_ZP 0x85     //Store accumulator (Zero page). Takes 3 cycles.
#define STA_ZPX 0x95    //Store accumulator (Zero page X). Takes 4 cycles.
#define STA_ABS 0x8D    //Store accumulator (Absolute). Takes 4 cycles.
#define STA_ABSX 0x9D   //Store accumulator (Absolute X). Takes 5 cycles.
#define STA_ABSY 0x99   //Store accumulator (Absolute Y). Takes 5 cycles.
#define STA_INDX 0x81   //Store accumulator (Indirect X). Takes 6 cycles.
#define STA_INDY 0x91   //Store accumulator (Indirect Y). Takes 6 cycles.

#define STX_ZP 0x86     //Store X (Zero page). Takes 3 cycles.
#define STX_ZPY 0x96    //Store X (Zero page Y). Takes 4 cycles.
#define STX_ABS 0x8E    //Store X (Absolute). Takes 4 cycles.

#define STY_ZP 0x84     //Store Y (Zero page). Takes 3 cycles.
#define STY_ZPX 0x94    //Store Y (Zero page Y). Takes 4 cycles.
#define STY_ABS 0x8C    //Store Y (Absolute). Takes 4 cycles.

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

#define AND_IM 0x29     //AND (Immediate). Takes 2 cycles.
#define AND_ZP 0x25     //AND (Zero Page). Takes 3 cycles.
#define AND_ZPX 0x35    //AND (Zero Page X). Takes 4 cycles.
#define AND_ABS 0x2D    //AND (Absolute). Takes 4 cycles.
#define AND_ABSX 0x3D   //AND (Absolute X). Takes 4(+1) cycles.
#define AND_ABSY 0x39   //AND (Absolute Y). Takes 4(+1) cycles.
#define AND_INDX 0x21   //AND (Indirect X). Takes 6 cycles.
#define AND_INDY 0x31   //AND (Indirect Y). Takes 5(+1) cycles.

#define EOR_IM 0x49     //Exclusive OR (Immediate). Takes 2 cycles.
#define EOR_ZP 0x45     //Exclusive OR (Zero page). Takes 3 cycles.
#define EOR_ZPX 0x55    //Exclusive OR (Zero Page X). Takes 4 cycles.
#define EOR_ABS 0x4D    //Exclusive OR (Absolute). Takes 4 cycles.
#define EOR_ABSX 0x5D   //Exclusive OR (Absolute X). Takes 4(+1) cycles.
#define EOR_ABSY 0x59   //Exclusive OR (Absolute Y). Takes 4(+1) cycles.
#define EOR_INDX 0x41   //Exclusive OR (Indirect X). Takes 6 cycles.
#define EOR_INDY 0x51   //Exclusive OR (Indirect Y). Takes 5(+1) cycles.

#define ORA_IM 0x09     //Inclusive OR (Immediate). Takes 2 cycles.
#define ORA_ZP 0x05     //Inclusive OR (Zero page). Takes 3 cycles.
#define ORA_ZPX 0x15    //Inclusive OR (Zero Page X). Takes 4 cycles.
#define ORA_ABS 0x0D    //Inclusive OR (Absolute). Takes 4 cycles.
#define ORA_ABSX 0x1D   //Inclusive OR (Absolute X). Takes 4(+1) cycles.
#define ORA_ABSY 0x19   //Inclusive OR (Absolute Y). Takes 4(+1) cycles.
#define ORA_INDX 0x01   //Inclusive OR (Indirect X). Takes 6 cycles.
#define ORA_INDY 0x11   //Inclusive OR (Indirect Y). Takes 5(+1) cycles.

#define BIT_ZP 0x24     //Bit test (Zero page). Takes 3 cycles.
#define BIT_ABS 0x2C    //Bit test (Absolute). Takes 4 cycles.

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

void resetCPU(struct CPU* cpu, word sPC);
struct RAM* initRAM();
void freeRAM(struct RAM* ram);
int exec(struct CPU* cpu, struct RAM* ram, u32 insCount);

#endif // !CPU_H
