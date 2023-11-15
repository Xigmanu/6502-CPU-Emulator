#include "cpu.h"
#include <stdio.h>
#define TEST


#ifndef TEST
int main() {
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFF1);

   ram->data[0x89] = 0x12;
   ram->data[0xFFF1] = INS_JSR;
   ram->data[0xFFF2] = 0x41;
   ram->data[0xFFF3] = 0x55;
   ram->data[0x5541] = INS_LDA_ZP;
   ram->data[0x5542] = 0x89;
   ram->data[0x5543] = INS_ADC_IM;
   ram->data[0x5544] = 0x21;
   ram->data[0x5545] = INS_RTS;
   ram->data[0xFFF4] = INS_ADC_IM;
   ram->data[0xFFF5] = 0x10;
   exec(&cpu, ram, 5);

   freeRAM(ram);
   printf("CPU: A: 0x%X, PC: 0x%X", cpu.a, cpu.pc);
   return 0;
}
#else
#include "test.h"
int main() {
   printf("TESTING\n");

   runTests();

   printf("\nFINISHED TESTING");
   return 0;
}
#endif // !TEST
