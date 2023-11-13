#include "cpu.h"
#include <stdio.h>
#include <stdbool.h>

byte readByteFromPC(word* pc, const Mem* mem, u32* cycles) {
   byte data = mem->data[*pc];
   (*pc)++;
   (*cycles)--;

   return data;
}

byte readByteFromAddr(word addr, const Mem* mem, u32* cycles) {
   byte data = mem->data[addr];
   (*cycles)--;

   return data;
}

word readWordFromPC(word* pc, const Mem* mem, u32* cycles) {
   byte loB = readByteFromPC(pc, mem, cycles);
   byte hiB = readByteFromPC(pc, mem, cycles);

#ifdef DEBUG
   printf("\tRead word. loB=[0x%X], hiB=[0x%X]\n", loB, hiB);
#endif // DEBUG

   return (word)(loB | (hiB << 8));
}

word readWordFromAddr(word addr, const Mem* mem, u32* cycles) {
   byte loB = readByteFromAddr(addr, mem, cycles);
   byte hiB = readByteFromAddr(addr + 1, mem, cycles);

#ifdef DEBUG
   printf("\tRead word from [0x%X+0x%X]. loB=[0x%X], hiB=[0x%X]\n", addr, addr + 1, loB, hiB);
#endif // DEBUG

   return (word)(loB | (hiB << 8));
}

void pushWordToStack(Mem* mem, byte* sp, word val, u32* cycles) {
   (*sp)--;
   mem->data[*sp] = val & 0xFF;
   (*sp)--;
   mem->data[*sp] = val >> 8;

   (*cycles) -= 2;

#ifdef DEBUG
   printf("\tPushed [0x%X] to the stack. Current sp: [0x%X]\n", val, *sp);
#endif // DEBUG

}

word popWordFromStack(const Mem* mem, byte* sp, u32* cycles) {
   byte hB = mem->data[*sp];
   (*sp)++;
   byte lB = mem->data[*sp];
   (*sp)++;
   (*cycles) -= 4;

   word data = (word)((hB << 8) | lB);
   return data;
}

void setNAndZFlags(CPU* cpu) {
   cpu->z = (cpu->a == 0);
   cpu->n = (cpu->a & 0b10000000) > 0;
}

void reset(CPU* cpu, Mem* mem) {
   cpu->pc = 0xFFFC;
   cpu->sp = 0x10;
   cpu->d = 0;

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

   initMem(mem);
}

void initMem(Mem* mem) {
   for (u32 i = 0; i < MEM_MAX; i++) {
      mem->data[i] = 0;
   }
}

void exec(CPU* cpu, Mem* mem, u32 cycles) {
   while (cycles > 0) {
      byte nextIns = readByteFromPC(cpu, mem, &cycles);
#ifdef DEBUG
      printf("Instruction: 0x%X\n", nextIns);
#endif
      switch (nextIns) {
      case INS_JSR: {
         word addr = readWordFromPC(&cpu->pc, mem, &cycles);
         pushWordToStack(mem, &cpu->sp, cpu->pc - 1, &cycles);

         cpu->pc = addr;
         cycles--;
      } break;
      case INS_RTS: {
         word addr = popWordFromStack(mem, &cpu->sp, &cycles);
         cpu->pc = addr;
         cycles--;
      } break;
      case INS_LDA_IM: {
         byte val = readByteFromPC(&cpu->pc, mem, &cycles);
         cpu->a = val;
         setNAndZFlags(cpu);
      } break;
      case INS_LDA_ZP: {
         byte zpAddr = readByteFromPC(&cpu->pc, mem, &cycles);
         cpu->a = readByteFromAddr(zpAddr, mem, &cycles);
         setNAndZFlags(cpu);
      } break;
      case INS_LDA_ZPX: {
         byte zpAddr = readByteFromPC(&cpu->pc, mem, &cycles);
         zpAddr += cpu->x;

         cpu->a = readByteFromAddr(zpAddr, mem, &cycles);
         setNAndZFlags(cpu);
      } break;
      case INS_LDA_ABS: {
         word absAddr = readWordFromPC(&cpu->pc, mem, &cycles);
         cycles--;

         cpu->a = readByteFromAddr(absAddr, mem, &cycles);
         setNAndZFlags(cpu);
      } break;
      case INS_LDA_ABSX: {
         word absAddr = readWordFromPC(&cpu->pc, mem, &cycles);
         word absAddrX = absAddr + cpu->x;
         const bool crossed = (absAddr ^ absAddrX) >> 8;
         if (crossed) {
            cycles--;
         }
         cpu->a = readByteFromAddr(absAddrX, mem, &cycles);
         setNAndZFlags(cpu);
      } break;
      case INS_LDA_ABSY: {
         word absAddr = readWordFromPC(&cpu->pc, mem, &cycles);
         word absAddrY = absAddr + cpu->y;
         const bool crossed = (absAddr ^ absAddrY) >> 8;
         if (crossed) {
            cycles--;
         }
         cpu->a = readByteFromAddr(absAddrY, mem, &cycles);
         setNAndZFlags(cpu);
      } break;
      case INS_LDA_INDX: {
         byte addrZp = readByteFromPC(&cpu->pc, mem, &cycles);
         addrZp += cpu->x;
         cycles--;
         word addr = readWordFromAddr(addrZp, mem, &cycles);
         
         cpu->a = readByteFromAddr(addr, mem, &cycles);
         setNAndZFlags(cpu);
      } break;
      default: {
         printf("Unsupported instruction: %d", nextIns);
      }
      }
   }
}
