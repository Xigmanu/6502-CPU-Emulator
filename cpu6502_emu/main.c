#include "cpu.h"
#include <stdio.h>
#define TEST

#ifndef TEST
int main() {
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFF1);

   ram->data[0xFFF1] = INS_LDA_IM;
   ram->data[0xFFF2] = 0x42;
   ram->data[0xFFF3] = INS_JSR;
   ram->data[0xFFF4] = 0xA2;
   ram->data[0xFFF5] = 0x45;
   ram->data[0x45A2] = INS_STA_ABS;
   ram->data[0x45A3] = 0xD4;
   ram->data[0x45A4] = 0x87;
   ram->data[0x45A5] = INS_TAX;
   ram->data[0x45A6] = INS_RTS;
   ram->data[0xFFF6] = INS_STX_ZP;
   ram->data[0xFFF7] = 0x42;
   ram->data[0xFFF8] = INS_LDY_ZP;
   ram->data[0xFFF9] = 0x42;
   exec(&cpu, ram, 7);

   freeRAM(ram);
   printf("CPU: A: 0x%X, X: 0x%X, Y: 0x%X, PC: 0x%X", cpu.a, cpu.x, cpu.y, cpu.pc);
   return 0;
}
#else
#include "test.h"
int main() {
   printf("RUNNING %d TESTS\n", TEST_COUNT);

   runTests();

   printf("\nFINISHED TESTING");
   return 0;
}
#endif // !TEST
