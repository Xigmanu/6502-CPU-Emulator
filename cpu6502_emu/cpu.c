#include "cpu.h"
#include <stdio.h>
#include <stdbool.h>

#pragma region Private

byte readByteFromPC(word* pc, const RAM* ram, u32* cycles) {
   byte val = ram->data[*pc];
#ifdef _DEBUG
   printf("DEBUG\t| Read byte [0x%X] from PC: [0x%X]\n", val, *pc);
#endif //_DEBUG
   (*pc)++;
   (*cycles)--;

   return val;
}

byte readByteFromAddr(word addr, const RAM* ram, u32* cycles) {
   byte val = ram->data[addr];
   (*cycles)--;

#ifdef _DEBUG
   printf("DEBUG\t| Read byte [0x%X] from ADDR: [0x%X]\n", val, addr);
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

   (*cycles) -= 2;

#ifdef _DEBUG
   printf("DEBUG\t| Pushed [0x%X] to the stack. Current SP: [0x%X]\n", val, *sp);
#endif // _DEBUG
}

word popWordFromStack(const RAM* ram, byte* sp, u32* cycles) {
   byte hB = ram->data[*sp];
   (*sp)++;
   byte lB = ram->data[*sp];
   (*sp)++;
   (*cycles) -= 4;

   word val = (word)((hB << 8) | lB);

#ifdef _DEBUG
   printf("DEBUG\t| Popped [0x%X] from the stack. Current SP: [0x%X]\n", val, *sp);
#endif //_DEBUG

   return val;
}

void setLDAFlags(CPU* cpu) {
   cpu->z = (cpu->a == 0);
   cpu->n = (cpu->a & 0b10000000) > 0;
#ifdef _DEBUG
   printf("DEBUG\t| Set LDA Flags: Z: [0x%X], N: [0x%X]\n", cpu->z, cpu->n);
#endif // _DEBUG
}

#pragma endregion

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

#ifdef _DEBUG
   printf("DEBUG\t| Reset CPU\n");
#endif // _DEBUG

}

void exec(CPU* cpu, RAM* ram, u32 cycles) {
   while (cycles > 0) {
      byte nextIns = readByteFromPC(&cpu->pc, ram, &cycles);
#ifdef _DEBUG
      printf("DEBUG\t| Instruction: 0x%X\n", nextIns);
#endif //_DEBUG
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
         setLDAFlags(cpu);
      } break;
      case INS_LDA_ZP: {
         byte zpAddr = readByteFromPC(&cpu->pc, ram, &cycles);
         cpu->a = readByteFromAddr(zpAddr, ram, &cycles);
         setLDAFlags(cpu);
      } break;
      case INS_LDA_ZPX: {
         byte zpAddr = readByteFromPC(&cpu->pc, ram, &cycles);
         zpAddr += cpu->x;
         cycles--;
         cpu->a = readByteFromAddr(zpAddr, ram, &cycles);
         setLDAFlags(cpu);
      } break;
      case INS_LDA_ABS: {
         word absAddr = readWordFromPC(&cpu->pc, ram, &cycles);

         cpu->a = readByteFromAddr(absAddr, ram, &cycles);
         setLDAFlags(cpu);
      } break;
      case INS_LDA_ABSX: {
         word absAddr = readWordFromPC(&cpu->pc, ram, &cycles);
         word absAddrX = absAddr + cpu->x;
         const bool crossed = (absAddr ^ absAddrX) >> 8;
         if (crossed) {
            cycles--;
         }

         cpu->a = readByteFromAddr(absAddrX, ram, &cycles);
         setLDAFlags(cpu);
      } break;
      case INS_LDA_ABSY: {
         word absAddr = readWordFromPC(&cpu->pc, ram, &cycles);
         word absAddrY = absAddr + cpu->y;
         const bool crossed = (absAddr ^ absAddrY) >> 8;
         if (crossed) {
            cycles--;
         }

         cpu->a = readByteFromAddr(absAddrY, ram, &cycles);
         setLDAFlags(cpu);
      } break;
      case INS_LDA_INDX: {
         byte addrZp = readByteFromPC(&cpu->pc, ram, &cycles);
         addrZp += cpu->x;
         cycles--;
         word addr = readWordFromAddr(addrZp, ram, &cycles);
         
         cpu->a = readByteFromAddr(addr, ram, &cycles);
         setLDAFlags(cpu);
      } break;
      case INS_LDA_INDY: {
         byte addrZp = readByteFromPC(&cpu->pc, ram, &cycles);
         word addr = readWordFromAddr(addrZp, ram, &cycles);
         word addrY = addr + cpu->y;
         const bool crossed = (addr ^ addrY) >> 8;
         if (crossed) {
            cycles--;
         }

         cpu->a = readByteFromAddr(addrY, ram, &cycles);
         setLDAFlags(cpu);
      } break;
      default: {
         printf("Unsupported instruction: %d\n", nextIns);
      }
      }
   }
}
