#include "cpu.h"
#include <stdio.h>
#include <stdbool.h>

#define EMPTY_ADDR 0x0

#pragma region Memory helpers

static byte readByteFromAddr(word addr, const RAM* ram, u32* cycles) {
   byte val = ram->data[addr];
   (*cycles)++;

#ifdef _DEBUG
   printf("DEBUG\t| Read [0x%X] from [0x%X]\n", val, addr);
#endif //_DEBUG

   return val;
}

static word readWordFromAddr(word addr, const RAM* ram, u32* cycles) {
   byte loB = readByteFromAddr(addr, ram, cycles);
   byte hiB = readByteFromAddr(addr + 1, ram, cycles);

#ifdef _DEBUG
   printf("DEBUG\t| Read word. loB=[0x%X], hiB=[0x%X]\n", loB, hiB);
#endif // _DEBUG

   return (word)(loB | (hiB << 8));
}

static byte readByteFromPC(word* pc, const RAM* ram, u32* cycles) {
   byte val = readByteFromAddr(*pc, ram, cycles);
   (*pc)++;
   return val;
}

static word readWordFromPC(word* pc, const RAM* ram, u32* cycles) {
   word val = readWordFromAddr(*pc, ram, cycles);
   (*pc)+=2;
   return val;
}

static void writeByteToMemory(byte val, word addr, RAM* ram, u32* cycles) {
   ram->data[addr] = val;
   (*cycles)++;
#ifdef _DEBUG
   printf("DEBUG\t| Wrote [0x%X] to [0x%X]\n", val, addr);
#endif // _DEBUG
}

static void pushByteToStack(RAM* ram, word* sp, byte val, u32* cycles) {
   (*sp)--;
   ram->data[*sp] = val;
   (*cycles)++;
#ifdef _DEBUG
   printf("DEBUG\t| Pushed [0x%X] to the stack. Current SP: [0x%X]\n", val, *sp);
#endif // _DEBUG
}

static byte popByteFromStack(const RAM* ram, word* sp, u32* cycles) {
   byte val = ram->data[*sp];
   (*sp)++;
   (*cycles)++;
#ifdef _DEBUG
   printf("DEBUG\t| Popped [0x%X] from the stack. Current SP: [0x%X]\n", val, *sp);
#endif //_DEBUG
   return val;
}

static void pushWordToStack(RAM* ram, word* sp, word val, u32* cycles) {
   pushByteToStack(ram, sp, (val >> 8), cycles);
   pushByteToStack(ram, sp, (val & 0xFF), cycles);
}

static word popWordFromStack(const RAM* ram, word* sp, u32* cycles) {
   byte lB = popByteFromStack(ram, sp, cycles);
   byte hB = popByteFromStack(ram, sp, cycles);
   (*cycles)++;
   return (word)((hB << 8) | lB);
}

#pragma endregion

#pragma region Addressing mode helpers

typedef enum {
   NONE,
   X,
   Y
} AddrModeReg;

static byte zpAddr(CPU* cpu, const RAM* ram, AddrModeReg reg) {
   byte zpAddr = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   byte regVal;

   switch (reg)
   {
   case X: regVal = cpu->x; break;
   case Y: regVal = cpu->y; break;
   default: return zpAddr;
   }

   zpAddr += regVal;
   cpu->cycles++;

   return zpAddr;
}

static word absAddr(CPU* cpu, const RAM* ram, AddrModeReg reg, bool canPageCross) {
   word absAddr = readWordFromPC(&cpu->pc, ram, &cpu->cycles);
   byte regVal;

   switch (reg)
   {
   case X: regVal = cpu->x; break;
   case Y: regVal = cpu->y; break;
   default: return absAddr;
   }

   word absAddrReg = absAddr + regVal;
   if (canPageCross) {
      cpu->cycles += ((absAddr & 0xFF00) != (absAddrReg & 0xFF00)) ? 1 : 0;
   }
   else {
      cpu->cycles++;
   }

   return absAddrReg;
}

static word indAddr(CPU* cpu, const RAM* ram, AddrModeReg reg, bool canPageCross) {
   byte zpAddr = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   word baseAddr;

   switch (reg)
   {
   case X: {
      zpAddr += cpu->x;
      cpu->cycles++;

      baseAddr = readWordFromAddr(zpAddr, ram, &cpu->cycles);
      return baseAddr;
   }
   case Y: {
      baseAddr = readWordFromAddr(zpAddr, ram, &cpu->cycles);
      word addrY = baseAddr + cpu->y;
      if (canPageCross) {
         cpu->cycles += ((baseAddr & 0xFF00) != (addrY & 0xFF00)) ? 1 : 0;
      }
      else {
         cpu->cycles++;
      }

      return addrY;
   }
   default: return 0x0;
   }
}

#pragma endregion

#pragma region Flag helpers

static void updatePS(CPU* cpu) {
   byte ps = 0x0;

   ps |= (cpu->n << 7);
   ps |= (cpu->v << 6);
   ps |= (cpu->b << 4);
   ps |= (cpu->d << 3);
   ps |= (cpu->i << 2);
   ps |= (cpu->z << 1);
   ps |= cpu->c;

   cpu->ps = ps;

#ifdef _DEBUG
   printf("DEBUG\t| Set Processor Status: PS: [0x%X]\n", ps);
#endif // _DEBUG
}

static void setZNFlags(CPU* cpu, byte reg) {
   cpu->z = (reg == 0);
   cpu->n = (reg & 0x80) > 0;
#ifdef _DEBUG
   printf("DEBUG\t| Set Flags: Z: [0x%X], N: [0x%X]\n", cpu->z, cpu->n);
#endif // _DEBUG
   updatePS(cpu);
}

static void setCVFlags(CPU* cpu, word sum, byte op) {
   setZNFlags(cpu, cpu->a);
   cpu->c = sum > 0xFF;
   cpu->v = ((cpu->a ^ op) & 0x80) && ((cpu->a ^ sum) & 0x80);
#ifdef _DEBUG
   printf("DEBUG\t| Set Flags: C: [0x%X], V: [0x%X]\n", cpu->c, cpu->v);
#endif // _DEBUG
   updatePS(cpu);
}

#pragma endregion

#pragma region Instruction helpers

static void ldIns(CPU* cpu, const RAM* ram, byte* reg, word valAddr) {
   byte val = EMPTY_ADDR == valAddr 
      ? readByteFromPC(&cpu->pc, ram, &cpu->cycles) 
      : readByteFromAddr(valAddr, ram, &cpu->cycles);
   (*reg) = val;
   setZNFlags(cpu, *reg);
}

static void andIns(CPU* cpu, const RAM* ram, word valAddr) {
   byte val = EMPTY_ADDR == valAddr
      ? readByteFromPC(&cpu->pc, ram, &cpu->cycles)
      : readByteFromAddr(valAddr, ram, &cpu->cycles);
   cpu->a &= val;
   setZNFlags(cpu, cpu->a);
}

#pragma endregion

#pragma region Instruction handlers

static void jsr(CPU* cpu, RAM* ram) { //takes 1 cycle to fetch op code
   word addr = readWordFromPC(&cpu->pc, ram, &cpu->cycles); // takes 2 cycles
   pushWordToStack(ram, &cpu->sp, cpu->pc - 1, &cpu->cycles); // takes 4 cycles

   cpu->pc = addr;
   cpu->cycles++;
}

static void rts(CPU* cpu, const RAM* ram) {
   word addr = popWordFromStack(ram, &cpu->sp, &cpu->cycles);
   cpu->pc = addr + 1; //Not sure if correct but this is the only way program flow does not break.
   cpu->cycles += 2;
}

static void adcImmediate(CPU* cpu, const RAM* ram) {
   byte val = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   word sum = cpu->a + val + cpu->c;
   cpu->a = (byte)sum;
   setCVFlags(cpu, sum, val);
}

static void ldaImmediate(CPU* cpu, const RAM* ram) {
   ldIns(cpu, ram, &cpu->a, EMPTY_ADDR);
}

static void ldaZeroPage(CPU* cpu, const RAM* ram) {
   byte addr = zpAddr(cpu, ram, NONE);
   ldIns(cpu, ram, &cpu->a, addr);
}

static void ldaZeroPageX(CPU* cpu, const RAM* ram) {
   byte addr = zpAddr(cpu, ram, X);
   ldIns(cpu, ram, &cpu->a, addr);
}

static void ldaAbsolute(CPU* cpu, const RAM* ram) {
   word addr = absAddr(cpu, ram, NONE, false);
   ldIns(cpu, ram, &cpu->a, addr);
}

static void ldaAbsoluteX(CPU* cpu, const RAM* ram) {
   word addr = absAddr(cpu, ram, X, true);
   ldIns(cpu, ram, &cpu->a, addr);
}

static void ldaAbsoluteY(CPU* cpu, const RAM* ram) {
   word addr = absAddr(cpu, ram, Y, true);
   ldIns(cpu, ram, &cpu->a, addr);
}

static void ldaIndirectX(CPU* cpu, const RAM* ram) {
   word addr = indAddr(cpu, ram, X, false);   
   ldIns(cpu, ram, &cpu->a, addr);
}

static void ldaIndirectY(CPU* cpu, const RAM* ram) {
   word addr = indAddr(cpu, ram, Y, true);
   ldIns(cpu, ram, &cpu->a, addr);
}

static void ldxImmediate(CPU* cpu, const RAM* ram) {
   ldIns(cpu, ram, &cpu->x, EMPTY_ADDR);
}

static void ldxZeroPage(CPU* cpu, const RAM* ram) {
   byte addr = zpAddr(cpu, ram, NONE);
   ldIns(cpu, ram, &cpu->x, addr);
}

static void ldxZeroPageY(CPU* cpu, const RAM* ram) {
   byte addr = zpAddr(cpu, ram, Y);
   ldIns(cpu, ram, &cpu->x, addr);
}

static void ldxAbsolute(CPU* cpu, const RAM* ram) {
   word addr = absAddr(cpu, ram, NONE, false);
   ldIns(cpu, ram, &cpu->x, addr);
}

static void ldxAbsoluteY(CPU* cpu, const RAM* ram) {
   word addr = absAddr(cpu, ram, Y, true);
   ldIns(cpu, ram, &cpu->x, addr);
}

static void ldyImmediate(CPU* cpu, const RAM* ram) {
   ldIns(cpu, ram, &cpu->y, EMPTY_ADDR);
}

static void ldyZeroPage(CPU* cpu, const RAM* ram) {
   byte addr = zpAddr(cpu, ram, NONE);
   ldIns(cpu, ram, &cpu->y, addr);
}

static void ldyZeroPageX(CPU* cpu, const RAM* ram) {
   byte addr = zpAddr(cpu, ram, X);
   ldIns(cpu, ram, &cpu->y, addr);
}

static void ldyAbsolute(CPU* cpu, const RAM* ram) {
   word addr = absAddr(cpu, ram, NONE, false);
   ldIns(cpu, ram, &cpu->y, addr);
}

static void ldyAbsoluteX(CPU* cpu, const RAM* ram) {
   word addr = absAddr(cpu, ram, X, true);
   ldIns(cpu, ram, &cpu->y, addr);
}

static void staZeroPage(CPU* cpu, RAM* ram) {
   byte addr = zpAddr(cpu, ram, NONE);
   writeByteToMemory(cpu->a, addr, ram, &cpu->cycles);
}

static void staZeroPageX(CPU* cpu, RAM* ram) {
   word addr = zpAddr(cpu, ram, X);
   writeByteToMemory(cpu->a, addr, ram, &cpu->cycles);
}

static void staAbsolute(CPU* cpu, RAM* ram) {
   word addr = absAddr(cpu, ram, NONE, false);
   writeByteToMemory(cpu->a, addr, ram, &cpu->cycles);
}

static void staAbsoluteX(CPU* cpu, RAM* ram) {
   word addr = absAddr(cpu, ram, X, false);
   writeByteToMemory(cpu->a, addr, ram, &cpu->cycles);
}

static void staAbsoluteY(CPU* cpu, RAM* ram) {
   word addr = absAddr(cpu, ram, Y, false);
   writeByteToMemory(cpu->a, addr, ram, &cpu->cycles);
}

static void staIndirectX(CPU* cpu, RAM* ram) {
   word addr = indAddr(cpu, ram, X, false);
   writeByteToMemory(cpu->a, addr, ram, &cpu->cycles);
}

static void staIndirectY(CPU* cpu, RAM* ram) {
   word addr = indAddr(cpu, ram, Y, false);
   writeByteToMemory(cpu->a, addr, ram, &cpu->cycles);
}

static void stxZeroPage(CPU* cpu, RAM* ram) {
   byte addr = zpAddr(cpu, ram, NONE);
   writeByteToMemory(cpu->x, addr, ram, &cpu->cycles);
}

static void stxZeroPageY(CPU* cpu, RAM* ram) {
   byte addr = zpAddr(cpu, ram, Y);
   writeByteToMemory(cpu->x, addr, ram, &cpu->cycles);
}

static void stxAbsolute(CPU* cpu, RAM* ram) {
   word addr = absAddr(cpu, ram, NONE, false);
   writeByteToMemory(cpu->x, addr, ram, &cpu->cycles);
}

static void styZeroPage(CPU* cpu, RAM* ram) {
   byte addr = zpAddr(cpu, ram, NONE);
   writeByteToMemory(cpu->y, addr, ram, &cpu->cycles);
}

static void styZeroPageX(CPU* cpu, RAM* ram) {
   byte addr = zpAddr(cpu, ram, X);
   writeByteToMemory(cpu->y, addr, ram, &cpu->cycles);
}

static void styAbsolute(CPU* cpu, RAM* ram) {
   word addr = absAddr(cpu, ram, NONE, false);
   writeByteToMemory(cpu->y, addr, ram, &cpu->cycles);
}

static void tax(CPU* cpu, const RAM* ram) {
   (void)ram; //unused

   cpu->x = cpu->a;
   cpu->cycles++;
   setZNFlags(cpu, cpu->x);
}

static void txa(CPU* cpu, const RAM* ram) {
   (void)ram; //unused

   cpu->a = cpu->x;
   cpu->cycles++;
   setZNFlags(cpu, cpu->a);
}

static void tay(CPU* cpu, const RAM* ram) {
   (void)ram; //unused

   cpu->y = cpu->a;
   cpu->cycles++;
   setZNFlags(cpu, cpu->y);
}

static void tya(CPU* cpu, const RAM* ram) {
   (void)ram; //unused

   cpu->a = cpu->y;
   cpu->cycles++;
   setZNFlags(cpu, cpu->a);
}

static void tsx(CPU* cpu, const RAM* ram) {
   (void)ram;

   cpu->x = (cpu->sp & 0x00FF);
   cpu->cycles++;
   setZNFlags(cpu, cpu->x);
}

static void txs(CPU* cpu, const RAM* ram) {
   (void)ram;

   cpu->sp = 0x0100 | cpu->x;
   cpu->cycles++;
}

static void pha(CPU* cpu, RAM* ram) {
   pushByteToStack(ram, &cpu->sp, cpu->a, &cpu->cycles);
   cpu->cycles++;
}

static void php(CPU* cpu, RAM* ram) {
   pushByteToStack(ram, &cpu->sp, cpu->ps, &cpu->cycles);
   cpu->cycles++;
}

static void pla(CPU* cpu, const RAM* ram) {
   byte val = popByteFromStack(ram, &cpu->sp, &cpu->cycles);
   cpu->a = val;
   cpu->cycles += 2;
   setZNFlags(cpu, cpu->a);
}

static void plp(CPU* cpu, const RAM* ram) {
   byte val = popByteFromStack(ram, &cpu->sp, &cpu->cycles);
   cpu->ps = val;
   cpu->cycles++;

   cpu->c = (val & 0b00000001) != 0;
   cpu->n = (val & 0b10000000) != 0;
   cpu->v = (val & 0b01000000) != 0;
   cpu->i = (val & 0b00010000) != 0;
   cpu->d = (val & 0b00001000) != 0;
   cpu->b = (val & 0b00000100) != 0;
   cpu->v = (val & 0b00000010) != 0;
   cpu->cycles++;
}

static void andImmediate(CPU* cpu, const RAM* ram) {
   andIns(cpu, ram, EMPTY_ADDR);
}

static void andZeroPage(CPU* cpu, const RAM* ram) {
   byte addr = zpAddr(cpu, ram, NONE);
   andIns(cpu, ram, addr);
}

static void andZeroPageX(CPU* cpu, const RAM* ram) {
   byte addr = zpAddr(cpu, ram, X);
   andIns(cpu, ram, addr);
}

static void andAbsolute(CPU* cpu, const RAM* ram) {
   word addr = absAddr(cpu, ram, NONE, false);
   andIns(cpu, ram, addr);
}

static void andAbsoluteX(CPU* cpu, const RAM* ram) {
   word addr = absAddr(cpu, ram, X, true);
   andIns(cpu, ram, addr);
}

static void andAbsoluteY(CPU* cpu, const RAM* ram) {
   word addr = absAddr(cpu, ram, Y, true);
   andIns(cpu, ram, addr);
}

static void andIndirectX(CPU* cpu, const RAM* ram) {
   word addr = indAddr(cpu, ram, X, false);
   andIns(cpu, ram, addr);
}

static void andIndirectY(CPU* cpu, const RAM* ram) {
   word addr = indAddr(cpu, ram, Y, true);
   andIns(cpu, ram, addr);
}

#pragma endregion

//Instruction map.
void(*insTable[256])(CPU* cpu, RAM* ram) = {
   [INS_JSR] = &jsr,
   [INS_RTS] = &rts,
   [INS_ADC_IM] = &adcImmediate,
   [INS_LDA_IM] = &ldaImmediate,
   [INS_LDA_ZP] = &ldaZeroPage,
   [INS_LDA_ZPX] = &ldaZeroPageX,
   [INS_LDA_ABS] = &ldaAbsolute,
   [INS_LDA_ABSX] = &ldaAbsoluteX,
   [INS_LDA_ABSY] = &ldaAbsoluteY,
   [INS_LDA_INDX] = &ldaIndirectX,
   [INS_LDA_INDY] = &ldaIndirectY,
   [INS_LDX_IM] = &ldxImmediate,
   [INS_LDX_ZP] = &ldxZeroPage,
   [INS_LDX_ZPY] = &ldxZeroPageY,
   [INS_LDX_ABS] = &ldxAbsolute,
   [INS_LDX_ABSY] = &ldxAbsoluteY,
   [INS_LDY_IM] = &ldyImmediate,
   [INS_LDY_ZP] = &ldyZeroPage,
   [INS_LDY_ZPX] = &ldyZeroPageX,
   [INS_LDY_ABS] = &ldyAbsolute,
   [INS_LDY_ABSX] = &ldyAbsoluteX,
   [INS_STA_ZP] = &staZeroPage,
   [INS_STA_ZPX] = &staZeroPageX,
   [INS_STA_ABS] = &staAbsolute,
   [INS_STA_ABSX] = &staAbsoluteX,
   [INS_STA_ABSY] = &staAbsoluteY,
   [INS_STA_INDX] = &staIndirectX,
   [INS_STA_INDY] = &staIndirectY,
   [INS_STX_ZP] = &stxZeroPage,
   [INS_STX_ZPY] = &stxZeroPageY,
   [INS_STX_ABS] = &stxAbsolute,
   [INS_STY_ZP] = &styZeroPage,
   [INS_STY_ZPX] = &styZeroPageX,
   [INS_STY_ABS] = &styAbsolute,
   [INS_TAX] = &tax,
   [INS_TXA] = &txa,
   [INS_TAY] = &tay,
   [INS_TYA] = &tya,
   [INS_TSX] = &tsx,
   [INS_TXS] = &txs,
   [INS_PHA] = &pha,
   [INS_PHP] = &php,
   [INS_PLA] = &pla,
   [INS_PLP] = &plp,
   [INS_AND_IM] = &andImmediate,
   [INS_AND_ZP] = &andZeroPage,
   [INS_AND_ZPX] = &andZeroPageX,
   [INS_AND_ABS] = &andAbsolute,
   [INS_AND_ABSX] = &andAbsoluteX,
   [INS_AND_ABSY] = &andAbsoluteY,
   [INS_AND_INDX] = &andIndirectX,
   [INS_AND_INDY] = &andIndirectY,
};

RAM* initRAM() {
   RAM* ram = malloc(sizeof(RAM));
   if (NULL == ram) {
      printf("Allocation error.");
      return NULL;
   }

   ram->data = (byte*)calloc(MEM_MAX, sizeof(byte)); //Allocate MEM_MAX*1B on the heap.
   if (NULL == ram->data) {
      printf("Allocation error");
      free(ram);
      return NULL;
   }

#ifdef _DEBUG
   printf("DEBUG\t| Initialized RAM\n");
#endif // _DEBUG

   return ram;
}

//Does not set RAM ptr to NULL.
void freeRAM(RAM* ram) {
   free(ram->data);
   free(ram);

#ifdef _DEBUG
   printf("DEBUG\t| Freed allocated memory\n");
#endif //_DEBUG
}

void resetCPU(CPU* cpu, word sPC) {
   cpu->pc = sPC;
   cpu->sp = 0x01FF;

   cpu->c = 0;
   cpu->z = 0;
   cpu->i = 0;
   cpu->d = 0;
   cpu->b = 0;
   cpu->v = 0;
   cpu->n = 0;

   cpu->a = 0;
   cpu->x = 0;
   cpu->y = 0;

   cpu->cycles = 0;

#ifdef _DEBUG
   printf("DEBUG\t| Reset CPU\n");
#endif // _DEBUG
}

int exec(CPU* cpu, RAM* ram, u32 insCount) {
   for (u32 i = insCount; i > 0; i--) {
      byte opCode = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
      
      if (NULL != insTable[opCode]) {
         insTable[opCode](cpu, ram);
      }
      else {
         return 1;
      }
   }

   return 0;
}
