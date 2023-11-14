#include "test.h"
#include <stdlib.h>

void testResetCPU() {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   ASSERT_EQUAL(0xFFFC, cpu.pc, "PC");
   ASSERT_EQUAL(0xFF, cpu.sp, "SP");
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
   resetCPU(&cpu, 0xFFFC);

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
   resetCPU(&cpu, 0xFFFC);

   ram->data[0xFFFC] = INS_LDA_ZP;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x42] = 0x24;
   exec(&cpu, ram, 3);

   ASSERT_EQUAL(0x24, cpu.a, "A");

   freeRAM(ram);
}

void testLDAZeroPageX() {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.x = 0x10;
   ram->data[0xFFFC] = INS_LDA_ZPX;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x52] = 0x70;
   exec(&cpu, ram, 4);

   ASSERT_EQUAL(0x70, cpu.a, "A");

   freeRAM(ram);
}

void testLDAAbsolute() {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   ram->data[0xFFFC] = INS_LDA_ABS;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1242] = 0x15;
   exec(&cpu, ram, 4);

   ASSERT_EQUAL(0x15, cpu.a, "A");

   freeRAM(ram);
}
void testLDAAbsoluteX() {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.x = 0x10;
   ram->data[0xFFFC] = INS_LDA_ABSX;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1252] = 0x15;
   exec(&cpu, ram, 4);

   ASSERT_EQUAL(0x15, cpu.a, "A");

   freeRAM(ram);
}
void testLDAAbsoluteY() {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.y = 0x10;
   ram->data[0xFFFC] = INS_LDA_ABSY;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1252] = 0x15;
   exec(&cpu, ram, 4);

   ASSERT_EQUAL(0x15, cpu.a, "A");

   freeRAM(ram);
}
void testLDAIndirectX() {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.x = 0x10;
   ram->data[0xFFFC] = INS_LDA_INDX;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x52] = 0x15;
   ram->data[0x53] = 0x16;
   ram->data[0x1615] = 0x51;
   exec(&cpu, ram, 6);

   ASSERT_EQUAL(0x51, cpu.a, "A");

   freeRAM(ram);
}

void testLDAIndirectY() {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.y = 0x10;
   ram->data[0xFFFC] = INS_LDA_INDY;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x52] = 0x15;
   ram->data[0x53] = 0x16;
   ram->data[0x1615] = 0x51;
   exec(&cpu, ram, 6);

   ASSERT_EQUAL(0x51, cpu.a, "A");

   freeRAM(ram);
}
void testJSR() {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   ram->data[0xFFFC] = INS_JSR;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x24;
   ram->data[0x2442] = INS_LDA_IM;
   ram->data[0x2443] = 0x51;
   exec(&cpu, ram, 8);

   ASSERT_EQUAL(0x51, cpu.a, "A");
   ASSERT_EQUAL(0x2444, cpu.pc, "PC");

   freeRAM(ram);
}
void testRTS() {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   ram->data[0xFFFC] = INS_JSR;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x24;
   ram->data[0x2442] = INS_LDA_IM;
   ram->data[0x2443] = 0x51;
   ram->data[0x2444] = INS_RTS;
   exec(&cpu, ram, 14);

   ASSERT_EQUAL(0x51, cpu.a, "A");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");

   freeRAM(ram);
}

