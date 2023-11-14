#include "test.h"
#include <stdlib.h>

void testResetCPU() {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu);

   ASSERT_EQUAL(0xFFFC, cpu.pc, "PC");
   ASSERT_EQUAL(0x10, cpu.sp, "SP");
   ASSERT_EQUAL(0, cpu.c, "C");
   ASSERT_EQUAL(0, cpu.z, "Z");
   ASSERT_EQUAL(0, cpu.i, "I");
   ASSERT_EQUAL(0, cpu.d, "D");
   ASSERT_EQUAL(0, cpu.b, "B");
   ASSERT_EQUAL(0, cpu.v, "V");
   ASSERT_EQUAL(0, cpu.n, "N");
   ASSERT_EQUAL(0, cpu.a, "A");
   ASSERT_EQUAL(0, cpu.x, "X");
   ASSERT_EQUAL(0, cpu.y, "Y");

   freeRAM(ram);
}

void testLDAImmediate() {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu);

   ram->data[0xFFFC] = INS_LDA_IM;
   ram->data[0xFFFD] = 0x42;

   exec(&cpu, ram, 2);

   ASSERT_EQUAL(0x42, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.n, "X");

   freeRAM(ram);
}

void testLDAZeroPage() {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu);

   ram->data[0xFFFC] = INS_LDA_ZP;
   ram->data[0xFFFD] = 0x42;
   exec(&cpu, ram, 3);

   ASSERT_EQUAL(0x42, ram->data[0xFFFD], "Z");

   freeRAM(ram);
}

void testLDAZeroPageX();
void testLDAAbsolute();
void testLDAAbsoluteX();
void testLDAAbsoluteY();
void testLDAIndirectX();
void testJSR();
void testRTS();

