#include "../include/test.h"
#include <stdlib.h>

static void test_reset_cpu(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   ASSERT_EQUAL(0xFFFC, cpu.pc, "PC");
   ASSERT_EQUAL(0x01FF, cpu.sp, "SP");
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

   free_ram(ram);
}

static void test_jsr(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   ram->data[0xFFFC] = JSR;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x24;
   ram->data[0x2442] = LDA_IM;
   ram->data[0x2443] = 0x51;
   exec(&cpu, ram, 2);

   ASSERT_EQUAL(0x51, cpu.a, "A");
   ASSERT_EQUAL(0x2444, cpu.pc, "PC");
   ASSERT_EQUAL(8, cpu.cycles, "Cycles");

   free_ram(ram);
}
static void test_rts(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   ram->data[0xFFFC] = JSR;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x24;
   ram->data[0x2442] = LDA_IM;
   ram->data[0x2443] = 0x51;
   ram->data[0x2444] = RTS;
   exec(&cpu, ram, 3);

   ASSERT_EQUAL(0x51, cpu.a, "A");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(14, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_lda_imm(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   ram->data[0xFFFC] = LDA_IM;
   ram->data[0xFFFD] = 0x42;

   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, cpu.a, "A");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
}

static void test_lda_zp(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   ram->data[0xFFFC] = LDA_ZP;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x42] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x24, cpu.a, "A");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
}

static void test_lda_zp_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.x = 0x10;
   ram->data[0xFFFC] = LDA_ZPX;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x52] = 0x70;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x70, cpu.a, "A");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
}

static void test_lda_abs(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   ram->data[0xFFFC] = LDA_ABS;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1242] = 0x15;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x15, cpu.a, "A");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
}
static void test_lda_abs_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.x = 0x10;
   ram->data[0xFFFC] = LDA_ABSX;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1252] = 0x15;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x15, cpu.a, "A");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
}
static void test_lda_abs_y(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.y = 0x10;
   ram->data[0xFFFC] = LDA_ABSY;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1252] = 0x15;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x15, cpu.a, "A");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
}
static void test_lda_ind_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.x = 0x10;
   ram->data[0xFFFC] = LDA_INDX;
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

   free_ram(ram);
}

static void test_lda_ind_y(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.y = 0x10;
   ram->data[0xFFFC] = LDA_INDY;
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

   free_ram(ram);
}

static void test_ldx_imm(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   ram->data[0xFFFC] = LDX_IM;
   ram->data[0xFFFD] = 0x42;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, cpu.x, "X");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
}
static void test_ldx_zp(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   ram->data[0xFFFC] = LDX_ZP;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x42] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x24, cpu.x, "X");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
} 
static void test_ldx_zp_y(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   
   cpu.y = 0x10;
   ram->data[0xFFFC] = LDX_ZPY;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x52] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x24, cpu.x, "X");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
}
static void test_ldx_abs(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   ram->data[0xFFFC] = LDX_ABS;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1242] = 0x15;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x15, cpu.x, "X");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
}

static void test_ldx_abs_y(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.y = 0x10;
   ram->data[0xFFFC] = LDX_ABSY;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1252] = 0x15;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x15, cpu.x, "X");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
}

static void test_ldy_imm(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   
   ram->data[0xFFFC] = LDY_IM;
   ram->data[0xFFFD] = 0x42;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, cpu.y, "Y");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
}

static void test_ldy_zp(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   ram->data[0xFFFC] = LDY_ZP;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x42] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x24, cpu.y, "Y");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
  
   free_ram(ram);
}

static void test_ldy_zp_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.x = 0x10;
   ram->data[0xFFFC] = LDY_ZPX;
   ram->data[0xFFFD] = 0x42;
   ram->data[0x52] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x24, cpu.y, "Y");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
}

static void test_ldy_abs(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   ram->data[0xFFFC] = LDY_ABS;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1242] = 0x15;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x15, cpu.y, "Y");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
}

static void test_ldy_abs_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.x = 0x10;
   ram->data[0xFFFC] = LDY_ABSX;
   ram->data[0xFFFD] = 0x42;
   ram->data[0xFFFE] = 0x12;
   ram->data[0x1252] = 0x15;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x15, cpu.y, "Y");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(0x0, cpu.z, "Z");

   free_ram(ram);
}

static void test_sta_zp(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0x42;
   ram->data[0xFFFC] = STA_ZP;
   ram->data[0xFFFD] = 0x54;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x54], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_sta_zp_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.x = 0x24;
   ram->data[0xFFFC] = STA_ZPX;
   ram->data[0xFFFD] = 0x54;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x78], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_sta_abs(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0x42;
   ram->data[0xFFFC] = STA_ABS;
   ram->data[0xFFFD] = 0x33;
   ram->data[0xFFFE] = 0x22;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x2233], "MEM");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   
   free_ram(ram);
}

static void test_sta_abs_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.x = 0x10;
   ram->data[0xFFFC] = STA_ABSX;
   ram->data[0xFFFD] = 0x33;
   ram->data[0xFFFE] = 0x22;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x2243], "MEM");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(5, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_sta_abs_y(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.y = 0x10;
   ram->data[0xFFFC] = STA_ABSY;
   ram->data[0xFFFD] = 0x33;
   ram->data[0xFFFE] = 0x22;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x2243], "MEM");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(5, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_sta_ind_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.x = 0x10;
   ram->data[0xFFFC] = STA_INDX;
   ram->data[0xFFFD] = 0x24;
   ram->data[0x34] = 0x40;
   ram->data[0x35] = 0x89;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x8940], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(6, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_sta_ind_y(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.y = 0x10;
   ram->data[0xFFFC] = STA_INDY;
   ram->data[0xFFFD] = 0x12;
   ram->data[0x12] = 0x14;
   ram->data[0x13] = 0xD2;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0xD224], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(6, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_stx_zp(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.x = 0x42;
   ram->data[0xFFFC] = STX_ZP;
   ram->data[0xFFFD] = 0x34;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x34], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_stx_zp_y(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.x = 0x42;
   cpu.y = 0x10;
   ram->data[0xFFFC] = STX_ZPY;
   ram->data[0xFFFD] = 0x34;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x44], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   
   free_ram(ram);
}

static void test_stx_abs(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.x = 0x42;
   ram->data[0xFFFC] = STX_ABS;
   ram->data[0xFFFD] = 0x34;
   ram->data[0xFFFE] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x2434], "MEM");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");
   
   free_ram(ram);
}

static void test_sty_zp(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.y = 0x42;
   ram->data[0xFFFC] = STY_ZP;
   ram->data[0xFFFD] = 0x34;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x34], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_sty_zp_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.x = 0x42;
   cpu.y = 0x10;
   ram->data[0xFFFC] = STY_ZPX;
   ram->data[0xFFFD] = 0x34;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x10, ram->data[0x76], "MEM");
   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_sty_abs(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.y = 0x42;
   ram->data[0xFFFC] = STY_ABS;
   ram->data[0xFFFD] = 0x34;
   ram->data[0xFFFE] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, ram->data[0x2434], "MEM");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_tax(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   reset_cpu(&cpu, 0xFFFC);
   struct RAM* ram = init_ram();

   cpu.a = 0x42;
   ram->data[0xFFFC] = TAX;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(cpu.a, cpu.x, "X");
   ASSERT_EQUAL(0xFFFD, cpu.pc, "PC");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_txa(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   reset_cpu(&cpu, 0xFFFC);
   struct RAM* ram = init_ram();

   cpu.x = 0x42;
   ram->data[0xFFFC] = TXA;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(cpu.x, cpu.a, "A");
   ASSERT_EQUAL(0xFFFD, cpu.pc, "PC");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_tay(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   reset_cpu(&cpu, 0xFFFC);
   struct RAM* ram = init_ram();

   cpu.a = 0x42;
   ram->data[0xFFFC] = TAY;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(cpu.a, cpu.y, "Y");
   ASSERT_EQUAL(0xFFFD, cpu.pc, "PC");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_tya(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   reset_cpu(&cpu, 0xFFFC);
   struct RAM* ram = init_ram();

   cpu.y = 0xBE;
   ram->data[0xFFFC] = TYA;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(cpu.y, cpu.a, "A");
   ASSERT_EQUAL(0xFFFD, cpu.pc, "PC");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x1, cpu.n, "N");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_tsx(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   reset_cpu(&cpu, 0xFFFC);
   struct RAM* ram = init_ram();

   cpu.sp = 0x42;
   ram->data[0xFFFC] = TSX;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, cpu.x, "X");
   ASSERT_EQUAL(0xFFFD, cpu.pc, "PC");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_txs(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   reset_cpu(&cpu, 0xFFFC);
   struct RAM* ram = init_ram();
   
   cpu.x = 0x42;
   ram->data[0xFFFC] = TXS;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x42, cpu.x, "X");
   ASSERT_EQUAL(0xFFFD, cpu.pc, "PC");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_pha(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   reset_cpu(&cpu, 0xFFFC);
   struct RAM* ram = init_ram();

   cpu.a = 0x42;
   ram->data[0xFFFC] = PHA;
 
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x01FE, cpu.sp, "SP");
   ASSERT_EQUAL(0xFFFD, cpu.pc, "PC");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_php(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   reset_cpu(&cpu, 0xFFFC);
   struct RAM* ram = init_ram();

   ram->data[0xFFFC] = LDA_IM;
   ram->data[0xFFFD] = 0xBE;
   ram->data[0xFFFE] = PHP;
   exec(&cpu, ram, 2);
   
   ASSERT_EQUAL(0x01FE, cpu.sp, "SP");
   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(0x80, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x1, cpu.n, "N");
   ASSERT_EQUAL(5, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_pla(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   reset_cpu(&cpu, 0xFFFC);
   struct RAM* ram = init_ram();
   
   cpu.sp = 0x01FE;
   ram->data[0x01FE] = 0x42;
   ram->data[0xFFFC] = PLA;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x01FF, cpu.sp, "SP");
   ASSERT_EQUAL(0xFFFD, cpu.pc, "PC");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_plp(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   reset_cpu(&cpu, 0xFFFC);
   struct RAM* ram = init_ram();

   cpu.sp = 0x01FE;
   ram->data[0x01FE] = 0x81;
   ram->data[0xFFFC] = PLP;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0x01FF, cpu.sp, "SP");
   ASSERT_EQUAL(0xFFFD, cpu.pc, "PC");
   ASSERT_EQUAL(0x81, cpu.ps, "PS");
   ASSERT_EQUAL(0x1, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x1, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_and_imm(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;

   ram->data[0xFFFC] = AND_IM;
   ram->data[0xFFFD] = 0x42;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x0, cpu.a, "A");
   ASSERT_EQUAL(0x2, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x1, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_and_zp(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;

   ram->data[0x0001] = 0x42;
   ram->data[0xFFFC] = AND_ZP;
   ram->data[0xFFFD] = 0x1;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x0, cpu.a, "A");
   ASSERT_EQUAL(0x2, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x1, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_and_zp_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;
   cpu.x = 0x10;
   
   ram->data[0x0011] = 0x42;
   ram->data[0xFFFC] = AND_ZPX;
   ram->data[0xFFFD] = 0x1;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x0, cpu.a, "A");
   ASSERT_EQUAL(0x2, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x1, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_and_abs(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;

   ram->data[0xFFFC] = AND_ABS;
   ram->data[0xFFFD] = 0x53;
   ram->data[0xFFFE] = 0xA4;
   ram->data[0xA453] = 0x42;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(0x0, cpu.a, "A");
   ASSERT_EQUAL(0x2, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x1, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_and_abs_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;
   cpu.x = 0x10;

   ram->data[0xFFFC] = AND_ABSX;
   ram->data[0xFFFD] = 0x53;
   ram->data[0xFFFE] = 0xA4;
   ram->data[0xA463] = 0x42;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(0x0, cpu.a, "A");
   ASSERT_EQUAL(0x2, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x1, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_and_abs_y(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;
   cpu.y = 0x10;

   ram->data[0xFFFC] = AND_ABSY;
   ram->data[0xFFFD] = 0x53;
   ram->data[0xFFFE] = 0xA4;
   ram->data[0xA463] = 0x42;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(0x0, cpu.a, "A");
   ASSERT_EQUAL(0x2, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x1, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_and_ind_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.x = 0x10;
   ram->data[0xFFFC] = AND_INDX;
   ram->data[0xFFFD] = 0x24;
   ram->data[0x34] = 0x40;
   ram->data[0x35] = 0x89;
   ram->data[0x8940] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x0, cpu.a, "A");
   ASSERT_EQUAL(0x2, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x1, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(6, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_and_ind_y(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.y = 0x10;
   ram->data[0xFFFC] = AND_INDY;
   ram->data[0xFFFD] = 0x24;
   ram->data[0x24] = 0x40;
   ram->data[0x25] = 0x89;
   ram->data[0x8950] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x0, cpu.a, "A");
   ASSERT_EQUAL(0x2, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x1, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(5, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_eor_imm(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;

   ram->data[0xFFFC] = EOR_IM;
   ram->data[0xFFFD] = 0x42;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x66, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_eor_zp(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;

   ram->data[0x0001] = 0x42;
   ram->data[0xFFFC] = EOR_ZP;
   ram->data[0xFFFD] = 0x1;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x66, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_eor_zp_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;
   cpu.x = 0x10;

   ram->data[0x0011] = 0x42;
   ram->data[0xFFFC] = EOR_ZPX;
   ram->data[0xFFFD] = 0x1;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x66, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_eor_abs(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;

   ram->data[0xFFFC] = EOR_ABS;
   ram->data[0xFFFD] = 0x53;
   ram->data[0xFFFE] = 0xA4;
   ram->data[0xA453] = 0x42;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(0x66, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_eor_abs_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;
   cpu.y = 0x10;

   ram->data[0xFFFC] = EOR_ABSY;
   ram->data[0xFFFD] = 0x53;
   ram->data[0xFFFE] = 0xA4;
   ram->data[0xA463] = 0x42;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(0x66, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_eor_abs_y(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;
   cpu.y = 0x10;

   ram->data[0xFFFC] = EOR_ABSY;
   ram->data[0xFFFD] = 0x53;
   ram->data[0xFFFE] = 0xA4;
   ram->data[0xA463] = 0x42;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(0x66, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_eor_ind_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.x = 0x10;
   ram->data[0xFFFC] = EOR_INDX;
   ram->data[0xFFFD] = 0x24;
   ram->data[0x34] = 0x40;
   ram->data[0x35] = 0x89;
   ram->data[0x8940] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x66, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(6, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_eor_ind_y(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.y = 0x10;
   ram->data[0xFFFC] = EOR_INDY;
   ram->data[0xFFFD] = 0x24;
   ram->data[0x24] = 0x40;
   ram->data[0x25] = 0x89;
   ram->data[0x8950] = 0x24;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x66, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(5, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_ora_imm(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;

   ram->data[0xFFFC] = ORA_IM;
   ram->data[0xFFFD] = 0x55;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x75, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(2, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_ora_zp(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;

   ram->data[0x0001] = 0x55;
   ram->data[0xFFFC] = ORA_ZP;
   ram->data[0xFFFD] = 0x1;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x75, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_ora_zp_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;
   cpu.x = 0x10;

   ram->data[0x0011] = 0x55;
   ram->data[0xFFFC] = ORA_ZPX;
   ram->data[0xFFFD] = 0x1;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x75, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_ora_abs(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;

   ram->data[0xFFFC] = ORA_ABS;
   ram->data[0xFFFD] = 0x53;
   ram->data[0xFFFE] = 0xA4;
   ram->data[0xA453] = 0x55;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(0x75, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_ora_abs_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;
   cpu.y = 0x10;

   ram->data[0xFFFC] = ORA_ABSY;
   ram->data[0xFFFD] = 0x53;
   ram->data[0xFFFE] = 0xA4;
   ram->data[0xA463] = 0x55;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(0x75, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_ora_abs_y(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);
   cpu.a = 0x24;
   cpu.y = 0x10;

   ram->data[0xFFFC] = ORA_ABSY;
   ram->data[0xFFFD] = 0x53;
   ram->data[0xFFFE] = 0xA4;
   ram->data[0xA463] = 0x55;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(0x75, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_ora_ind_x(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.x = 0x10;
   ram->data[0xFFFC] = ORA_INDX;
   ram->data[0xFFFD] = 0x24;
   ram->data[0x34] = 0x40;
   ram->data[0x35] = 0x89;
   ram->data[0x8940] = 0x55;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x57, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(6, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_ora_ind_y(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0x42;
   cpu.y = 0x10;
   ram->data[0xFFFC] = ORA_INDY;
   ram->data[0xFFFD] = 0x24;
   ram->data[0x24] = 0x40;
   ram->data[0x25] = 0x89;
   ram->data[0x8950] = 0x55;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0x57, cpu.a, "A");
   ASSERT_EQUAL(0x0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x0, cpu.n, "N");
   ASSERT_EQUAL(5, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_bit_zp(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0xFF;
   ram->data[0x0041] = 0xC3;
   ram->data[0xFFFC] = BIT_ZP;
   ram->data[0xFFFD] = 0x41;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFE, cpu.pc, "PC");
   ASSERT_EQUAL(0xFF, cpu.a, "A");
   ASSERT_EQUAL(0xC0, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x1, cpu.v, "V");
   ASSERT_EQUAL(0x1, cpu.n, "N");
   ASSERT_EQUAL(3, cpu.cycles, "Cycles");

   free_ram(ram);
}

static void test_bit_abs(void) {
   PRINT_TEST_NAME();
   struct CPU cpu;
   struct RAM* ram = init_ram();
   reset_cpu(&cpu, 0xFFFC);

   cpu.a = 0xA5;
   ram->data[0x5141] = 0xC3;
   ram->data[0xFFFC] = BIT_ABS;
   ram->data[0xFFFD] = 0x41;
   ram->data[0xFFFE] = 0x51;
   exec(&cpu, ram, 1);

   ASSERT_EQUAL(0xFFFF, cpu.pc, "PC");
   ASSERT_EQUAL(0xA5, cpu.a, "A");
   ASSERT_EQUAL(0x80, cpu.ps, "PS");
   ASSERT_EQUAL(0x0, cpu.c, "C");
   ASSERT_EQUAL(0x0, cpu.z, "Z");
   ASSERT_EQUAL(0x0, cpu.i, "I");
   ASSERT_EQUAL(0x0, cpu.d, "D");
   ASSERT_EQUAL(0x0, cpu.b, "B");
   ASSERT_EQUAL(0x0, cpu.v, "V");
   ASSERT_EQUAL(0x1, cpu.n, "N");
   ASSERT_EQUAL(4, cpu.cycles, "Cycles");

   free_ram(ram);
}

void(*tests[])(void) = {
   &test_reset_cpu,
   &test_jsr,
   &test_rts,
   &test_lda_imm,
   &test_lda_zp,
   &test_lda_zp_x,
   &test_lda_abs,
   &test_lda_abs_x,
   &test_lda_abs_y,
   &test_lda_ind_x,
   &test_lda_ind_y,
   &test_ldx_imm,
   &test_ldx_zp,
   &test_ldx_zp_y,
   &test_ldx_abs,
   &test_ldx_abs_y,
   &test_ldy_imm,
   &test_ldy_zp,
   &test_ldy_zp_x,
   &test_ldy_abs,
   &test_ldy_abs_x,
   &test_sta_zp,
   &test_sta_zp_x,
   &test_sta_abs,
   &test_sta_abs_x,
   &test_sta_abs_y,
   &test_sta_ind_x,
   &test_sta_ind_y,
   &test_stx_zp,
   &test_stx_zp_y,
   &test_stx_abs,
   &test_sty_zp,
   &test_sty_zp_x,
   &test_sty_abs,
   &test_tax,
   &test_txa,
   &test_tay,
   &test_tya,
   &test_tsx,
   &test_txs,
   &test_pha,
   &test_php,
   &test_pla,
   &test_plp,
   &test_and_imm,
   &test_and_zp,
   &test_and_zp_x,
   &test_and_abs,
   &test_and_abs_x,
   &test_and_abs_y,
   &test_and_ind_x,
   &test_and_ind_y,
   &test_eor_imm,
   &test_eor_zp,
   &test_eor_zp_x,
   &test_eor_abs,
   &test_eor_abs_x,
   &test_eor_abs_y,
   &test_eor_ind_x,
   &test_eor_ind_y,
   &test_ora_imm,
   &test_ora_zp,
   &test_ora_zp_x,
   &test_ora_abs,
   &test_ora_abs_x,
   &test_ora_abs_y,
   &test_ora_ind_x,
   &test_ora_ind_y,
   &test_bit_zp,
   &test_bit_abs
};

void run_tests() {
   for (uint32_t i = 0; i <= TEST_COUNT; i++) {
      if (NULL != tests[i]) {
         tests[i]();
      }
   }
}
