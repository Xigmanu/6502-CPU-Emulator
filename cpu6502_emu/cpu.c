#include "cpu.h"
#include <stdio.h>

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

byte readByte(word* pc, const Mem* mem, u32* cycles) {
   byte data = mem->data[*pc];
   (*pc)++;

   (*cycles)--;
   return data;
}

word readWord(word* pc, const Mem* mem, u32* cycles) {
   word data = mem->data[*pc];
   (*pc)++;

   data |= (mem->data[*pc] << 8);
   (*pc)++;
   (*cycles) -= 2;

#ifdef DEBUG
   printf("\tRead word: 0x%X\n", data);
#endif // DEBUG

   return data;
}

void pushWordToStack(Mem* mem, byte* sp, word val, u32* cycles) {
   mem->data[*sp] = val & 0xFF;
   (*sp)--;
   mem->data[*sp] = val >> 8;
   (*sp)--;

   (*cycles) -= 2;

#ifdef DEBUG
   printf("\tPushed [0x%X] to the stack. Current sp: [0x%X]\n", val, *sp);
#endif // DEBUG

}

word popWordFromStack(const Mem* mem, byte* sp, u32* cycles) {
   word data = mem->data[*sp]; //lB
   (*sp)++;
   data |= (mem->data[*sp] << 8);
   (*sp)++;
   (*cycles) -= 2;

   return data;
}

void setLDAFlags(CPU* cpu) {
   cpu->z = (cpu->a == 0);
   cpu->n = (cpu->a & 0b10000000) > 0;
}

void exec(CPU* cpu, Mem* mem, u32 cycles) {
   while (cycles > 0) {
      byte nextIns = readByte(cpu, mem, &cycles);
#ifdef DEBUG
      printf("Instruction: 0x%X\n", nextIns);
#endif
      switch (nextIns) {
      case INS_JSR: {
         word addr = readWord(&cpu->pc, mem, &cycles);
         pushWordToStack(mem, &cpu->sp, addr, &cycles);

         cpu->pc = addr;
         cycles--;
      } break;
      case INS_LDA_IM: {
         byte val = readByte(&cpu->pc, mem, &cycles);
         cpu->a = val;
         setLDAFlags(cpu);
      } break;
      default: {
         printf("Unsupported instruction: %d", nextIns);
      }
      }
   }
}
