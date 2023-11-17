#include "cpu.h"
#include <stdio.h>
#define TEST


#ifndef TEST
int main() {
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFF1);

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
