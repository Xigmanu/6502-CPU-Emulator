#include "cpu.h"
#include <stdio.h>
#include <stdbool.h>

byte readByteFromPC(word* pc, const RAM* ram, u32* cycles) {
   byte data = ram->data[*pc];
   (*pc)++;
   (*cycles)--;

   return data;
}

byte readByteFromAddr(word addr, const RAM* ram, u32* cycles) {
   byte data = ram->data[addr];
   (*cycles)--;

   return data;
}

word readWordFromPC(word* pc, const RAM* ram, u32* cycles) {
   byte loB = readByteFromPC(pc, ram, cycles);
   byte hiB = readByteFromPC(pc, ram, cycles);

#ifdef DEBUG
   printf("\tRead word. loB=[0x%X], hiB=[0x%X]\n", loB, hiB);
#endif // DEBUG

   return (word)(loB | (hiB << 8));
}

word readWordFromAddr(word addr, const RAM* ram, u32* cycles) {
   byte loB = readByteFromAddr(addr, ram, cycles);
   byte hiB = readByteFromAddr(addr + 1, ram, cycles);

#ifdef DEBUG
   printf("\tRead word from [0x%X+0x%X]. loB=[0x%X], hiB=[0x%X]\n", addr, addr + 1, loB, hiB);
#endif // DEBUG

   return (word)(loB | (hiB << 8));
}

void pushWordToStack(RAM* ram, byte* sp, word val, u32* cycles) {
   (*sp)--;
   ram->data[*sp] = val & 0xFF;
   (*sp)--;
   ram->data[*sp] = val >> 8;

   (*cycles) -= 2;

#ifdef DEBUG
   printf("\tPushed [0x%X] to the stack. Current sp: [0x%X]\n", val, *sp);
#endif // DEBUG

}

word popWordFromStack(const RAM* ram, byte* sp, u32* cycles) {
   byte hB = ram->data[*sp];
   (*sp)++;
   byte lB = ram->data[*sp];
   (*sp)++;
   (*cycles) -= 4;

   word data = (word)((hB << 8) | lB);
   return data;
}

void setNAndZFlags(CPU* cpu) {
   cpu->z = (cpu->a == 0);
   cpu->n = (cpu->a & 0b10000000) > 0;
}

void resetRAM(RAM* ram) {
   for (u32 i = 0; i < MEM_MAX; i++) {
      ram->data[i] = 0;
   }
}

void resetCPU(CPU* cpu, RAM* ram) {
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

   resetRAM(ram);
}

void exec(CPU* cpu, RAM* ram, u32 cycles) {
   while (cycles > 0) {
      byte nextIns = readByteFromPC(&cpu->pc, ram, &cycles);
#ifdef DEBUG
      printf("Instruction: 0x%X\n", nextIns);
#endif
      switch (nextIns) {
      case INS_JSR: {
         word addr = readWordFromPC(&cpu->pc, ram, &cycles);
         pushWordToStack(ram, &cpu->sp, cpu->pc - 1, &cycles);

         cpu->pc = addr;
         cycles--;
      } break;
      case INS_RTS: {
         word addr = popWordFromStack(ram, &cpu->sp, &cycles);
         cpu->pc = addr;
         cycles--;
      } break;
      case INS_LDA_IM: {
         byte val = readByteFromPC(&cpu->pc, ram, &cycles);
         cpu->a = val;
         setNAndZFlags(cpu);
      } break;
      case INS_LDA_ZP: {
         byte zpAddr = readByteFromPC(&cpu->pc, ram, &cycles);
         cpu->a = readByteFromAddr(zpAddr, ram, &cycles);
         setNAndZFlags(cpu);
      } break;
      case INS_LDA_ZPX: {
         byte zpAddr = readByteFromPC(&cpu->pc, ram, &cycles);
         zpAddr += cpu->x;

         cpu->a = readByteFromAddr(zpAddr, ram, &cycles);
         setNAndZFlags(cpu);
      } break;
      case INS_LDA_ABS: {
         word absAddr = readWordFromPC(&cpu->pc, ram, &cycles);
         cycles--;

         cpu->a = readByteFromAddr(absAddr, ram, &cycles);
         setNAndZFlags(cpu);
      } break;
      case INS_LDA_ABSX: {
         word absAddr = readWordFromPC(&cpu->pc, ram, &cycles);
         word absAddrX = absAddr + cpu->x;
         const bool crossed = (absAddr ^ absAddrX) >> 8;
         if (crossed) {
            cycles--;
         }
         cpu->a = readByteFromAddr(absAddrX, ram, &cycles);
         setNAndZFlags(cpu);
      } break;
      case INS_LDA_ABSY: {
         word absAddr = readWordFromPC(&cpu->pc, ram, &cycles);
         word absAddrY = absAddr + cpu->y;
         const bool crossed = (absAddr ^ absAddrY) >> 8;
         if (crossed) {
            cycles--;
         }
         cpu->a = readByteFromAddr(absAddrY, ram, &cycles);
         setNAndZFlags(cpu);
      } break;
      case INS_LDA_INDX: {
         byte addrZp = readByteFromPC(&cpu->pc, ram, &cycles);
         addrZp += cpu->x;
         cycles--;
         word addr = readWordFromAddr(addrZp, ram, &cycles);
         
         cpu->a = readByteFromAddr(addr, ram, &cycles);
         setNAndZFlags(cpu);
      } break;
      default: {
         printf("Unsupported instruction: %d", nextIns);
      }
      }
   }
}
