#include "cpu.h"
#include <stdio.h>
#ifdef _DEBUG
#define TEST
#endif


#ifndef TEST
int main() {
   return 0;
}
#else
#include "test.h"
int main() {
   printf("TESTING\n");

   testResetCPU();
   testLDAImmediate();
   testLDAZeroPage();

   printf("\nFINISHED TESTING");
   return 0;
}
#endif // !TEST
