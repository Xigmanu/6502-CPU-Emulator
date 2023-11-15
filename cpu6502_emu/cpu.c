#include "cpu.h"
#include <stdio.h>
#include <stdbool.h>

#pragma region Helpers

byte readByteFromPC(word* pc, const RAM* ram, u32* cycles) {
   byte val = ram->data[*pc];
#ifdef _DEBUG
   printf("DEBUG\t| Read byte [0x%X] from RAM. PC: [0x%X]\n", val, *pc);
#endif //_DEBUG
   (*pc)++;
   (*cycles)++;

   return val;
}

byte readByteFromAddr(word addr, const RAM* ram, u32* cycles) {
   byte val = ram->data[addr];
   (*cycles)++;

#ifdef _DEBUG
   printf("DEBUG\t| Read byte [0x%X] from RAM. ADDR: [0x%X]\n", val, addr);
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
   printf("DEBUG\t| Read word from [0x%X+0x%X]. loB=[0x%X], hiB=[0x%X]\n", addr, addr + 1, loB, hiB);
#endif // _DEBUG

   return (word)(loB | (hiB << 8));
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

void setLDAFlags(CPU* cpu) {
   cpu->z = (cpu->a == 0);
   cpu->n = (cpu->a & 0x80) > 0;
#ifdef _DEBUG
   printf("DEBUG\t| Set Flags: Z: [0x%X], N: [0x%X]\n", cpu->z, cpu->n);
#endif // _DEBUG
}

void setADCFlags(CPU* cpu, word sum, byte op) {
   setLDAFlags(cpu);
   cpu->c = sum > 0xFF;
   cpu->v = ((cpu->a ^ op) & 0x80) && ((cpu->a ^ sum) & 0x80);
#ifdef _DEBUG
   printf("DEBUG\t| Set Flags: C: [0x%X], V: [0x%X]\n", cpu->c, cpu->v);
#endif // _DEBUG
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
   setLDAFlags(cpu);
}

void ldaZeroPage(CPU* cpu, const RAM* ram) {
   byte zpAddr = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   cpu->a = readByteFromAddr(zpAddr, ram, &cpu->cycles);
   setLDAFlags(cpu);
}

void ldaZeroPageX(CPU* cpu, const RAM* ram) {
   byte zpAddr = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   zpAddr += cpu->x;
   cpu->cycles++;

   cpu->a = readByteFromAddr(zpAddr, ram, &cpu->cycles);
   setLDAFlags(cpu);
}

void ldaAbsolute(CPU* cpu, const RAM* ram) {
   word absAddr = readWordFromPC(&cpu->pc, ram, &cpu->cycles);

   cpu->a = readByteFromAddr(absAddr, ram, &cpu->cycles);
   setLDAFlags(cpu);
}

void ldaAbsoluteX(CPU* cpu, const RAM* ram) {
   word absAddr = readWordFromPC(&cpu->pc, ram, &cpu->cycles);
   word absAddrX = absAddr + cpu->x;
   const bool crossed = (absAddr ^ absAddrX) >> 8;
   if (crossed) {
      cpu->cycles++;
   }

   cpu->a = readByteFromAddr(absAddrX, ram, &cpu->cycles);
   setLDAFlags(cpu);
}

void ldaAbsoluteY(CPU* cpu, const RAM* ram) {
   word absAddr = readWordFromPC(&cpu->pc, ram, &cpu->cycles);
   word absAddrY = absAddr + cpu->y;
   const bool crossed = (absAddr ^ absAddrY) >> 8;
   if (crossed) {
      cpu->cycles++;
   }

   cpu->a = readByteFromAddr(absAddrY, ram, &cpu->cycles);
   setLDAFlags(cpu);
}

void ldaIndirectX(CPU* cpu, const RAM* ram) {
   byte addrZp = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   addrZp += cpu->x;
   cpu->cycles++;
   word addr = readWordFromAddr(addrZp, ram, &cpu->cycles);

   cpu->a = readByteFromAddr(addr, ram, &cpu->cycles);
   setLDAFlags(cpu);
}

void ldaIndirectY(CPU* cpu, const RAM* ram) {
   byte addrZp = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
   word addr = readWordFromAddr(addrZp, ram, &cpu->cycles);
   word addrY = addr + cpu->y;
   const bool crossed = (addr ^ addrY) >> 8;
   if (crossed) {
      cpu->cycles++;
   }

   cpu->a = readByteFromAddr(addrY, ram, &cpu->cycles);
   setLDAFlags(cpu);
}

#pragma endregion

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
   printf("DEBUG\t| Allocated [%llu B] to RAM\n", MEM_MAX * sizeof(byte));
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

void exec(CPU* cpu, RAM* ram, u32 insCount) {
   for (u32 i = insCount; i > 0; i--) {
      byte opCode = readByteFromPC(&cpu->pc, ram, &cpu->cycles);
      
      if (NULL != insTable[opCode]) {
         insTable[opCode](cpu, ram);
      }
   }
}
