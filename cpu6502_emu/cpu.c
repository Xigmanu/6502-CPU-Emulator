#include "cpu.h"
#include <stdio.h>
#include <stdbool.h>

#pragma region Memory helpers

byte readByteFromPC(word* pc, const RAM* ram, u32* cycles) {
   byte val = ram->data[*pc];
#ifdef _DEBUG
   printf("DEBUG\t| Read [0x%X] from [0x%X]\n", val, *pc);
#endif //_DEBUG
   (*pc)++;
   (*cycles)++;

   return val;
}

byte readByteFromAddr(word addr, const RAM* ram, u32* cycles) {
   byte val = ram->data[addr];
   (*cycles)++;

#ifdef _DEBUG
   printf("DEBUG\t| Read [0x%X] from [0x%X]\n", val, addr);
#endif //_DEBUG

   return val;
}

word readWordFromPC(word* pc, const RAM* ram, u32* cycles) {
   byte loB = readByteFromPC(pc, ram, cycles);
   byte hiB = readByteFromPC(pc, ram, cycles);

#ifdef _DEBUG
   printf("DEBUG\t| Read word. loB=[0x%X], hiB=[0x%X]\n", loB, hiB);
#endif // _DEBUG

   return (word)(loB | (hiB << 8));
}

word readWordFromAddr(word addr, const RAM* ram, u32* cycles) {
   byte loB = readByteFromAddr(addr, ram, cycles);
   byte hiB = readByteFromAddr(addr + 1, ram, cycles);

#ifdef _DEBUG
   printf("DEBUG\t| Read word. loB=[0x%X], hiB=[0x%X]\n", loB, hiB);
#endif // _DEBUG

   return (word)(loB | (hiB << 8));
}

void writeByteToMemory(byte val, word addr, RAM* ram, u32* cycles) {
   ram->data[addr] = val;
   (*cycles)++;
#ifdef _DEBUG
   printf("DEBUG\t| Wrote [0x%X] to [0x%X]\n", val, addr);
#endif // _DEBUG
}

void pushWordToStack(RAM* ram, byte* sp, word val, u32* cycles) {
   (*sp)--;
   ram->data[*sp] = val & 0xFF;
   (*sp)--;
   ram->data[*sp] = val >> 8;

   (*cycles) += 2;
#ifdef _DEBUG
   printf("DEBUG\t| Pushed [0x%X] to the stack. Current SP: [0x%X]\n", val, *sp);
#endif // _DEBUG
}

word popWordFromStack(const RAM* ram, byte* sp, u32* cycles) {
   byte hB = ram->data[*sp];
   (*sp)++;
   byte lB = ram->data[*sp];
   (*sp)++;
   (*cycles) += 4;

   word val = (word)((hB << 8) | lB);

#ifdef _DEBUG
   printf("DEBUG\t| Popped [0x%X] from the stack. Current SP: [0x%X]\n", val, *sp);
#endif //_DEBUG

   return val;
}

#pragma endregion

#pragma region Instruction helpers
//Improve
void setLDFlags(CPU* cpu, byte reg) {
   cpu->z = (reg == 0);
   cpu->n = (reg & 0x80) > 0;
#ifdef _DEBUG
   printf("DEBUG\t| Set Flags: Z: [0x%X], N: [0x%X]\n", cpu->z, cpu->n);
#endif // _DEBUG
}

void setADCFlags(CPU* cpu, word sum, byte op) {
   setLDFlags(cpu, cpu->a);
   cpu->c = sum > 0xFF;
   cpu->v = ((cpu->a ^ op) & 0x80) && ((cpu->a ^ sum) & 0x80);
#ifdef _DEBUG
   printf("DEBUG\t| Set Flags: C: [0x%X], V: [0x%X]\n", cpu->c, cpu->v);
#endif // _DEBUG
}

byte ldZPHelper(word* pc, const RAM* ram, u32* cycles) {
   byte zpAddr = readByteFromPC(pc, ram, cycles);
   return readByteFromAddr(zpAddr, ram, cycles);
}

byte ldZPRegHelper(word* pc, const RAM* ram, u32* cycles, byte reg) {
   byte zpAddr = readByteFromPC(pc, ram, cycles);
   zpAddr += reg;
   (*cycles)++;
   
   return readByteFromAddr(zpAddr, ram, cycles);
}

byte ldAbsHelper(word* pc, const RAM* ram, u32* cycles) {
   word absAddr = readWordFromPC(pc, ram, cycles);
   return readByteFromAddr(absAddr, ram, cycles);
}

byte ldAbsRegHelper(word* pc, const RAM* ram, u32* cycles, byte reg) {
   word absAddr = readWordFromPC(pc, ram, cycles);
   word absAddrReg = absAddr + reg;
   if ((absAddr & 0xFF00) != (absAddrReg & 0xFF00)) {
      (*cycles)++;
   }
   
   return readByteFromAddr(absAddrReg, ram, cycles);
}

#pragma endregion

#pragma region Instruction handlers

void jsr(CPU* cpu, RAM* ram) {
   word addr = readWordFromPC(&cpu->pc, ram, &cpu->cycles);
   pushWordToStack(ram, &cpu->sp, cpu->pc - 1, &cpu->cycles);

   cpu->pc = addr;
   cpu->cycles++;
}

void rts(CPU* cpu, const RAM* ram) {
   word addr = popWordFromStack(ram, &cpu->sp, &cpu->cycles);
   cpu->pc = addr + 1; //Not sure if correct but this is the only way program flow does not break.
   cpu->cycles++;
}

void adcImmediate(CPU* cpu, const RAM* ram) {
   byte val = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   word sum = cpu->a + val + cpu->c;
   cpu->a = (byte)sum;
   setADCFlags(cpu, sum, val);
}

void ldaImmediate(CPU* cpu, const RAM* ram) {
   byte val = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   cpu->a = val;
   setLDFlags(cpu, cpu->a);
}

void ldaZeroPage(CPU* cpu, const RAM* ram) {
   cpu->a = ldZPHelper(&cpu->pc, ram, &cpu->cycles);
   setLDFlags(cpu, cpu->a);
}

void ldaZeroPageX(CPU* cpu, const RAM* ram) {
   cpu->a = ldZPRegHelper(&cpu->pc, ram, &cpu->cycles, cpu->x);
   setLDFlags(cpu, cpu->a);
}

void ldaAbsolute(CPU* cpu, const RAM* ram) {
   cpu->a = ldAbsHelper(&cpu->pc, ram, &cpu->cycles);
   setLDFlags(cpu, cpu->a);
}

void ldaAbsoluteX(CPU* cpu, const RAM* ram) {
   cpu->a = ldAbsRegHelper(&cpu->pc, ram, &cpu->cycles, cpu->x);
   setLDFlags(cpu, cpu->a);
}

void ldaAbsoluteY(CPU* cpu, const RAM* ram) {
   cpu->a = ldAbsRegHelper(&cpu->pc, ram, &cpu->cycles, cpu->y);
   setLDFlags(cpu, cpu->a);
}


void ldaIndirectX(CPU* cpu, const RAM* ram) {
   byte addrZp = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   addrZp += cpu->x;
   cpu->cycles++;
   word addr = readWordFromAddr(addrZp, ram, &cpu->cycles);

   cpu->a = readByteFromAddr(addr, ram, &cpu->cycles);
   setLDFlags(cpu, cpu->a);
}

void ldaIndirectY(CPU* cpu, const RAM* ram) {
   byte addrZp = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   word addr = readWordFromAddr(addrZp, ram, &cpu->cycles);
   word addrY = addr + cpu->y;
   if ((addr & 0xFF00) != (addrY & 0xFF00)) {
      cpu->cycles++;
   }

   cpu->a = readByteFromAddr(addrY, ram, &cpu->cycles);
   setLDFlags(cpu, cpu->a);
}

void ldxImmediate(CPU* cpu, const RAM* ram) {
   byte val = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   cpu->x = val;
   setLDFlags(cpu, cpu->x);
}

void ldxZeroPage(CPU* cpu, const RAM* ram) {
   cpu->x = ldZPHelper(&cpu->pc, ram, &cpu->cycles);
   setLDFlags(cpu, cpu->x);
}

void ldxZeroPageY(CPU* cpu, const RAM* ram) {
   cpu->x = ldZPRegHelper(&cpu->pc, ram, &cpu->cycles, cpu->y);
   setLDFlags(cpu, cpu->x);
}

void ldxAbsolute(CPU* cpu, const RAM* ram) {
   cpu->x = ldAbsHelper(&cpu->pc, ram, &cpu->cycles);
   setLDFlags(cpu, cpu->x);
}

void ldxAbsoluteY(CPU* cpu, const RAM* ram) {
   cpu->x = ldAbsRegHelper(&cpu->pc, ram, &cpu->cycles, cpu->y);
   setLDFlags(cpu, cpu->x);
}

void ldyImmediate(CPU* cpu, const RAM* ram) {
   byte val = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   cpu->y = val;
   setLDFlags(cpu, cpu->y);
}

void ldyZeroPage(CPU* cpu, const RAM* ram) {
   cpu->y = ldZPHelper(&cpu->pc, ram, &cpu->cycles);
   setLDFlags(cpu, cpu->y);
}

void ldyZeroPageX(CPU* cpu, const RAM* ram) {
   cpu->y = ldZPRegHelper(&cpu->pc, ram, &cpu->cycles, cpu->x);
   setLDFlags(cpu, cpu->y);
}

void ldyAbsolute(CPU* cpu, const RAM* ram) {
   cpu->y = ldAbsHelper(&cpu->pc, ram, &cpu->cycles);
   setLDFlags(cpu, cpu->y);
}

void ldyAbsoluteX(CPU* cpu, const RAM* ram) {
   cpu->y = ldAbsRegHelper(&cpu->pc, ram, &cpu->cycles, cpu->x);
   setLDFlags(cpu, cpu->y);
}

void staZeroPage(CPU* cpu, RAM* ram) {
   byte zpAddr = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   writeByteToMemory(cpu->a, zpAddr, ram, &cpu->cycles);
}

void staZeroPageX(CPU* cpu, RAM* ram) {
   byte zpAddr = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   byte effAddr = (zpAddr + cpu->x) & 0xFF;
   cpu->cycles++;
   writeByteToMemory(cpu->a, effAddr, ram, &cpu->cycles);
}

void staAbsolute(CPU* cpu, RAM* ram) {
   word addr = readWordFromPC(&cpu->pc, ram, &cpu->cycles);
   writeByteToMemory(cpu->a, addr, ram, &cpu->cycles);
}

void staAbsoluteX(CPU* cpu, RAM* ram) {
   word addr = readWordFromPC(&cpu->pc, ram, &cpu->cycles);
   word addrX = addr + cpu->x;
   if ((addrX & 0xFF00) != (addr & 0xFF)) {
      cpu->cycles++;
   }
   writeByteToMemory(cpu->a, addrX, ram, &cpu->cycles);
}

void staAbsoluteY(CPU* cpu, RAM* ram) {
   word addr = readWordFromPC(&cpu->pc, ram, &cpu->cycles);
   word effAddr = addr + cpu->y;
   if ((addr & 0xFF00) != (addr & 0xFF)) {
      cpu->cycles++;
   }
   writeByteToMemory(cpu->a, effAddr, ram, &cpu->cycles);
}

void staIndirectX(CPU* cpu, RAM* ram) {
   byte zpAddr = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   zpAddr += cpu->x;
   word effAddr = readWordFromAddr(zpAddr, ram, &cpu->cycles);
   writeByteToMemory(cpu->a, effAddr, ram, &cpu->cycles);
}

void staIndirectY(CPU* cpu, RAM* ram) {
   byte zpAddr = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   word addr = readWordFromAddr(zpAddr, ram, &cpu->cycles);
   addr += cpu->y;
   writeByteToMemory(cpu->a, addr, ram, &cpu->cycles);
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
   cpu->sp = 0xFF;

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
