#include "test.h"
#include <stdlib.h>

void testResetCPU(void) {
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

void testJSR(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   ram->data[0xFFFC] = INS_JSR;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x24;
   ram->data[0x2442] = INS_LDA_IM;
   ram->data[0x2443] = 0x51;
   exec(&cpu, ram, 2);

   ASSERT_EQUAL(0x51, cpu.a, "A");
   ASSERT_EQUAL(0x2444, cpu.pc, "PC");
   ASSERT_EQUAL(8, cpu.cycles, "Cycles");

   freeRAM(ram);
}
void testRTS(void) {
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
   exec(&cpu, ram, 3);

   ASSERT_EQUAL(0x51, cpu.a, "A");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(14, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testLDAImmediate(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   ram->data[0xFFFC] = INS_LDA_IM;
   ram->data[0xFFFD] = 0x42;

   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, cpu.a, "A");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}

void testLDAZeroPage(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   ram->data[0xFFFC] = INS_LDA_ZP;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x42] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x24, cpu.a, "A");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}

void testLDAZeroPageX(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.x = 0x10;
   ram->data[0xFFFC] = INS_LDA_ZPX;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x52] = 0x70;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x70, cpu.a, "A");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}

void testLDAAbsolute(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   ram->data[0xFFFC] = INS_LDA_ABS;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1242] = 0x15;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x15, cpu.a, "A");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}
void testLDAAbsoluteX(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.x = 0x10;
   ram->data[0xFFFC] = INS_LDA_ABSX;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1252] = 0x15;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x15, cpu.a, "A");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}
void testLDAAbsoluteY(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.y = 0x10;
   ram->data[0xFFFC] = INS_LDA_ABSY;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1252] = 0x15;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x15, cpu.a, "A");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}
void testLDAIndirectX(void) {
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
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x51, cpu.a, "A");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(6, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}

void testLDAIndirectY(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.y = 0x10;
   ram->data[0xFFFC] = INS_LDA_INDY;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x42] = 0x15;
   ram->data[0x43] = 0x16;
   ram->data[0x1625] = 0x51;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x51, cpu.a, "A");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(5, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}

void testLDXImmediate(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   ram->data[0xFFFC] = INS_LDX_IM;
   ram->data[0xFFFD] = 0x42;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, cpu.x, "X");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}
void testLDXZeroPage(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   ram->data[0xFFFC] = INS_LDX_ZP;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x42] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x24, cpu.x, "X");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}
void testLDXZeroPageY(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);
   
   cpu.y = 0x10;
   ram->data[0xFFFC] = INS_LDX_ZPY;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x52] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x24, cpu.x, "X");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}
void testLDXAbsolute(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   ram->data[0xFFFC] = INS_LDX_ABS;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1242] = 0x15;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x15, cpu.x, "X");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}

void testLDXAbsoluteY(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.y = 0x10;
   ram->data[0xFFFC] = INS_LDX_ABSY;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1252] = 0x15;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x15, cpu.x, "X");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}

void testLDYImmediate(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);
   
   ram->data[0xFFFC] = INS_LDY_IM;
   ram->data[0xFFFD] = 0x42;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, cpu.y, "Y");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}

void testLDYZeroPage(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   ram->data[0xFFFC] = INS_LDY_ZP;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x42] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x24, cpu.y, "Y");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
  
   freeRAM(ram);
}

void testLDYZeroPageX(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.x = 0x10;
   ram->data[0xFFFC] = INS_LDY_ZPX;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x52] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x24, cpu.y, "Y");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}

void testLDYAbsolute(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   ram->data[0xFFFC] = INS_LDY_ABS;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1242] = 0x15;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x15, cpu.y, "Y");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}

void testLDYAbsoluteX(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.x = 0x10;
   ram->data[0xFFFC] = INS_LDY_ABSX;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1252] = 0x15;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x15, cpu.y, "Y");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}

void testSTAZeroPage(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.a = 0x42;
   ram->data[0xFFFC] = INS_STA_ZP;
   ram->data[0xFFFD] = 0x54;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x54], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testSTAZeroPageX(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.x = 0x24;
   ram->data[0xFFFC] = INS_STA_ZPX;
   ram->data[0xFFFD] = 0x54;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x78], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testSTAAbsolute(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.a = 0x42;
   ram->data[0xFFFC] = INS_STA_ABS;
   ram->data[0xFFFD] = 0x33;
   ram->data[0xFFFE] = 0x22;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x2233], "MEM");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   
   freeRAM(ram);
}

void testSTAAbsoluteX(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.x = 0x10;
   ram->data[0xFFFC] = INS_STA_ABSX;
   ram->data[0xFFFD] = 0x33;
   ram->data[0xFFFE] = 0x22;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x2243], "MEM");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(5, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testSTAAbsoluteY(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.y = 0x10;
   ram->data[0xFFFC] = INS_STA_ABSY;
   ram->data[0xFFFD] = 0x33;
   ram->data[0xFFFE] = 0x22;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x2243], "MEM");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(5, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testSTAIndirectX(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.x = 0x10;
   ram->data[0xFFFC] = INS_STA_INDX;
   ram->data[0xFFFD] = 0x24;
   ram->data[0x34] = 0x40;
   ram->data[0x35] = 0x89;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x8940], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(6, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testSTAIndirectY(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.y = 0x10;
   ram->data[0xFFFC] = INS_STA_INDY;
   ram->data[0xFFFD] = 0x12;
   ram->data[0x12] = 0x14;
   ram->data[0x13] = 0xD2;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0xD224], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(5, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testSTXZeroPage(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.x = 0x42;
   ram->data[0xFFFC] = INS_STX_ZP;
   ram->data[0xFFFD] = 0x34;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x34], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testSTXZeroPageY(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.x = 0x42;
   cpu.y = 0x10;
   ram->data[0xFFFC] = INS_STX_ZPY;
   ram->data[0xFFFD] = 0x34;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x44], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   
   freeRAM(ram);
}

void testSTXAbsolute(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.x = 0x42;
   ram->data[0xFFFC] = INS_STX_ABS;
   ram->data[0xFFFD] = 0x34;
   ram->data[0xFFFE] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x2434], "MEM");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   
   freeRAM(ram);
}

void testSTYZeroPage(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.y = 0x42;
   ram->data[0xFFFC] = INS_STY_ZP;
   ram->data[0xFFFD] = 0x34;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x34], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testSTYZeroPageX(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.x = 0x42;
   cpu.y = 0x10;
   ram->data[0xFFFC] = INS_STY_ZPX;
   ram->data[0xFFFD] = 0x34;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x10, ram->data[0x76], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testSTYAbsolute(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFFC);

   cpu.y = 0x42;
   ram->data[0xFFFC] = INS_STY_ABS;
   ram->data[0xFFFD] = 0x34;
   ram->data[0xFFFE] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x2434], "MEM");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testTAX(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   resetCPU(&cpu, 0xFFFC);
   RAM* ram = initRAM();

   cpu.a = 0x42;
   ram->data[0xFFFC] = INS_TAX;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(cpu.a, cpu.x, "X");
   ASSERT_EQUAL(0xFFFD, cpu.pc, "PC");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testTXA(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   resetCPU(&cpu, 0xFFFC);
   RAM* ram = initRAM();

   cpu.x = 0x42;
   ram->data[0xFFFC] = INS_TXA;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(cpu.x, cpu.a, "A");
   ASSERT_EQUAL(0xFFFD, cpu.pc, "PC");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testTAY(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   resetCPU(&cpu, 0xFFFC);
   RAM* ram = initRAM();

   cpu.a = 0x42;
   ram->data[0xFFFC] = INS_TAY;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(cpu.a, cpu.y, "Y");
   ASSERT_EQUAL(0xFFFD, cpu.pc, "PC");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testTYA(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   resetCPU(&cpu, 0xFFFC);
   RAM* ram = initRAM();

   cpu.y = 0x42;
   ram->data[0xFFFC] = INS_TAY;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(cpu.y, cpu.a, "A");
   ASSERT_EQUAL(0xFFFD, cpu.pc, "PC");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");

   freeRAM(ram);
}

void testADCImmediate(void) {
   PRINT_TEST_NAME();
   CPU cpu;
   RAM* ram = initRAM();
   resetCPU(&cpu, 0xFFF3);

   ram->data[0xFFF3] = INS_LDA_IM;
   ram->data[0xFFF4] = 0x42;
   ram->data[0xFFF5] = INS_ADC_IM;
   ram->data[0xFFF6] = 0x22;
   ram->data[0xFFF7] = INS_ADC_IM;
   ram->data[0xFFF8] = 0x42;
   exec(&cpu, ram, 3);

   ASSERT_EQUAL(0xA6, cpu.a, "A");
   ASSERT_EQUAL(0xFFF9, cpu.pc, "PC");
   ASSERT_EQUAL(6, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   freeRAM(ram);
}

void(*tests[TEST_COUNT])(void) = {
   [0] = &testResetCPU,
   [1] = &testJSR,
   [2] = &testRTS,
   [3] = &testLDAImmediate,
   [4] = &testLDAZeroPage,
   [5] = &testLDAZeroPageX,
   [6] = &testLDAAbsolute,
   [7] = &testLDAAbsoluteX,
   [8] = &testLDAAbsoluteY,
   [9] = &testLDAIndirectX,
   [10] = &testLDAIndirectY,
   [11] = &testLDXImmediate,
   [12] = &testLDXZeroPage,
   [13] = &testLDAZeroPageX,
   [14] = &testLDXAbsolute,
   [15] = &testLDXAbsoluteY,
   [16] = &testLDYImmediate,
   [17] = &testLDYZeroPage,
   [18] = &testLDYZeroPageX,
   [19] = &testLDYAbsolute,
   [20] = &testLDYAbsoluteX,
   [21] = &testSTAZeroPage,
   [22] = &testSTAZeroPageX,
   [23] = &testSTAAbsolute,
   [24] = &testSTAAbsoluteX,
   [25] = &testSTAAbsoluteY,
   [26] = &testSTAIndirectX,
   [27] = &testSTAIndirectY,
   [28] = &testSTXZeroPage,
   [29] = &testSTXZeroPageY,
   [30] = &testSTXAbsolute,
   [31] = &testSTYZeroPage,
   [32] = &testSTYZeroPageX,
   [33] = &testSTYAbsolute,
   [34] = &testTAX,
   [35] = &testTXA,
   [36] = &testTAY,
   [37] = &testTYA,
};

void runTests() {
   for (u32 i = 0; i < TEST_COUNT; i++) {
      if (NULL != tests[i]) {
         tests[i]();
      }
   }
}
