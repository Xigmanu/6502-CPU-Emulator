#include "../include/cpu.h"
#include <stdio.h>
#define TEST

#ifndef TEST
int main() {
   return 0;
}
#else
#include "../include/test.h"
int main() {
   printf_s("RUNNING %d TESTS\n", TEST_COUNT);

   run_tests();

   printf_s("\nFINISHED TESTING");
   return 0;
}
#endif // TEST
