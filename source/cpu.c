#include "../include/cpu.h"
#include <stdio.h>
#include <stdbool.h>

#define EMPTY_ADDR 0x0

#pragma region Memory helpers

static inline byte r_byte_from_addr(word addr, const struct RAM* ram, uint32_t* cycles) {
   byte val = ram->data[addr];
   (*cycles)++;

#ifdef _DEBUG
   printf_s("DEBUG\t| Read [0x%X] from [0x%X]\n", val, addr);
#endif //_DEBUG

   return val;
}

static word r_word_from_addr(word addr, const struct RAM* ram, uint32_t* cycles) {
   byte loB = r_byte_from_addr(addr, ram, cycles);
   byte hiB = r_byte_from_addr(addr + 1, ram, cycles);

#ifdef _DEBUG
   printf_s("DEBUG\t| Read word. loB=[0x%X], hiB=[0x%X]\n", loB, hiB);
#endif // _DEBUG

   return (word)(loB | (hiB << 8));
}

static byte r_byte_from_pc(word* pc, const struct RAM* ram, uint32_t* cycles) {
   byte val = r_byte_from_addr(*pc, ram, cycles);
   (*pc)++;
   return val;
}

static word r_word_from_pc(word* pc, const struct RAM* ram, uint32_t* cycles) {
   word val = r_word_from_addr(*pc, ram, cycles);
   (*pc)+=2;
   return val;
}

static inline void w_byte_to_mem(byte val, word addr, struct RAM* ram, uint32_t* cycles) {
   ram->data[addr] = val;
   (*cycles)++;
#ifdef _DEBUG
   printf_s("DEBUG\t| Wrote [0x%X] to [0x%X]\n", val, addr);
#endif // _DEBUG
}

static inline void push_byte_to_stack(struct RAM* ram, word* sp, byte val, uint32_t* cycles) {
   (*sp)--;
   ram->data[*sp] = val;
   (*cycles)++;
#ifdef _DEBUG
   printf_s("DEBUG\t| Pushed [0x%X] to the stack. Current SP: [0x%X]\n", val, *sp);
#endif // _DEBUG
}

static inline byte pop_byte_from_stack(const struct RAM* ram, word* sp, uint32_t* cycles) {
   byte val = ram->data[*sp];
   (*sp)++;
   (*cycles)++;
#ifdef _DEBUG
   printf_s("DEBUG\t| Popped [0x%X] from the stack. Current SP: [0x%X]\n", val, *sp);
#endif //_DEBUG
   return val;
}

static void push_word_to_stack(struct RAM* ram, word* sp, word val, uint32_t* cycles) {
   push_byte_to_stack(ram, sp, (val >> 8), cycles);
   push_byte_to_stack(ram, sp, (val & 0xFF), cycles);
}

static word pop_word_from_stack(const struct RAM* ram, word* sp, uint32_t* cycles) {
   byte lB = pop_byte_from_stack(ram, sp, cycles);
   byte hB = pop_byte_from_stack(ram, sp, cycles);
   (*cycles)++;
   return (word)((hB << 8) | lB);
}

#pragma endregion

#pragma region Addressing mode helpers

typedef enum {
   NONE,
   X,
   Y
} AddrModeReg;

static byte zp_addr(struct CPU* cpu, const struct RAM* ram, AddrModeReg reg) {
   byte zpAddr = r_byte_from_pc(&cpu->pc, ram, &cpu->cycles);
   byte regVal;

   switch (reg)
   {
   case X: regVal = cpu->x; break;
   case Y: regVal = cpu->y; break;
   default: return zpAddr;
   }

   zpAddr += regVal;
   cpu->cycles++;

   return zpAddr;
}

static word abs_addr(struct CPU* cpu, const struct RAM* ram, AddrModeReg reg, bool canPageCross) {
   word absAddr = r_word_from_pc(&cpu->pc, ram, &cpu->cycles);
   byte regVal;

   switch (reg)
   {
   case X: regVal = cpu->x; break;
   case Y: regVal = cpu->y; break;
   default: return absAddr;
   }

   word absAddrReg = absAddr + regVal;
   if (canPageCross) {
      cpu->cycles += ((absAddr & 0xFF00) != (absAddrReg & 0xFF00)) ? 1 : 0;
   }
   else {
      cpu->cycles++;
   }

   return absAddrReg;
}

static word ind_addr(struct CPU* cpu, const struct RAM* ram, AddrModeReg reg, bool canPageCross) {
   byte zpAddr = r_byte_from_pc(&cpu->pc, ram, &cpu->cycles);
   word baseAddr;

   switch (reg)
   {
   case X: {
      zpAddr += cpu->x;
      cpu->cycles++;

      baseAddr = r_word_from_addr(zpAddr, ram, &cpu->cycles);
      return baseAddr;
   }
   case Y: {
      baseAddr = r_word_from_addr(zpAddr, ram, &cpu->cycles);
      word addrY = baseAddr + cpu->y;
      if (canPageCross) {
         cpu->cycles += ((baseAddr & 0xFF00) != (addrY & 0xFF00)) ? 1 : 0;
      }
      else {
         cpu->cycles++;
      }

      return addrY;
   }
   default: return 0x0;
   }
}

#pragma endregion

#pragma region Flag helpers

static void update_ps(struct CPU* cpu) {
   byte ps = 0x0;

   ps |= (cpu->n << 7);
   ps |= (cpu->v << 6);
   ps |= (cpu->b << 4);
   ps |= (cpu->d << 3);
   ps |= (cpu->i << 2);
   ps |= (cpu->z << 1);
   ps |= cpu->c;

   cpu->ps = ps;

#ifdef _DEBUG
   printf_s("DEBUG\t| Set Processor Status: PS: [0x%X]\n", ps);
#endif // _DEBUG
}

static inline void set_zn_flags(struct CPU* cpu, byte reg) {
   cpu->z = (reg == 0);
   cpu->n = (reg & 0x80) > 0;
#ifdef _DEBUG
   printf_s("DEBUG\t| Set Flags: Z: [0x%X], N: [0x%X]\n", cpu->z, cpu->n);
#endif // _DEBUG
   update_ps(cpu);
}

static inline void set_cv_flags(struct CPU* cpu, word sum) {
   cpu->c = sum > 0xFF;
   cpu->v = (sum & 0x100) != 0;
#ifdef _DEBUG
   printf_s("DEBUG\t| Set Flags: C: [0x%X], V: [0x%X]\n", cpu->c, cpu->v);
#endif // _DEBUG
   update_ps(cpu);
}

#pragma endregion

#pragma region Instruction helpers

typedef enum {
   AND,
   EOR,
   ORA
} LogIns;

static void ld_ins(struct CPU* cpu, const struct RAM* ram, byte* reg, word valAddr) {
   byte val = EMPTY_ADDR == valAddr 
      ? r_byte_from_pc(&cpu->pc, ram, &cpu->cycles) 
      : r_byte_from_addr(valAddr, ram, &cpu->cycles);
   (*reg) = val;
   set_zn_flags(cpu, *reg);
}

static void logic_ins(struct CPU* cpu, const struct RAM* ram, word valAddr, LogIns ins) {
   byte val = EMPTY_ADDR == valAddr
      ? r_byte_from_pc(&cpu->pc, ram, &cpu->cycles)
      : r_byte_from_addr(valAddr, ram, &cpu->cycles);
   
   switch (ins)
   {
   case AND: 
      cpu->a &= val;
      break;
   case EOR:
      cpu->a ^= val;
      break;
   case ORA:
      cpu->a |= val;
      break;
   }

   set_zn_flags(cpu, cpu->a);
}

#pragma endregion

#pragma region Instruction handlers

static void jsr(struct CPU* cpu, struct RAM* ram) {
   word addr = r_word_from_pc(&cpu->pc, ram, &cpu->cycles);
   push_word_to_stack(ram, &cpu->sp, cpu->pc - 1, &cpu->cycles);

   cpu->pc = addr;
   cpu->cycles++;
}

static void rts(struct CPU* cpu, const struct RAM* ram) {
   word addr = pop_word_from_stack(ram, &cpu->sp, &cpu->cycles);
   cpu->pc = addr + 1;
   cpu->cycles += 2;
}

static void lda_imm(struct CPU* cpu, const struct RAM* ram) {
   ld_ins(cpu, ram, &cpu->a, EMPTY_ADDR);
}

static void lda_zp(struct CPU* cpu, const struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, NONE);
   ld_ins(cpu, ram, &cpu->a, addr);
}

static void lda_zp_X(struct CPU* cpu, const struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, X);
   ld_ins(cpu, ram, &cpu->a, addr);
}

static void lda_abs(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, NONE, false);
   ld_ins(cpu, ram, &cpu->a, addr);
}

static void lda_abs_X(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, X, true);
   ld_ins(cpu, ram, &cpu->a, addr);
}

static void lda_abs_Y(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, Y, true);
   ld_ins(cpu, ram, &cpu->a, addr);
}

static void lda_ind_X(struct CPU* cpu, const struct RAM* ram) {
   word addr = ind_addr(cpu, ram, X, false);   
   ld_ins(cpu, ram, &cpu->a, addr);
}

static void lda_ind_Y(struct CPU* cpu, const struct RAM* ram) {
   word addr = ind_addr(cpu, ram, Y, true);
   ld_ins(cpu, ram, &cpu->a, addr);
}

static void ldx_imm(struct CPU* cpu, const struct RAM* ram) {
   ld_ins(cpu, ram, &cpu->x, EMPTY_ADDR);
}

static void ldx_zp(struct CPU* cpu, const struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, NONE);
   ld_ins(cpu, ram, &cpu->x, addr);
}

static void ldx_zp_y(struct CPU* cpu, const struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, Y);
   ld_ins(cpu, ram, &cpu->x, addr);
}

static void ldx_abs(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, NONE, false);
   ld_ins(cpu, ram, &cpu->x, addr);
}

static void ldx_abs_y(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, Y, true);
   ld_ins(cpu, ram, &cpu->x, addr);
}

static void ldy_imm(struct CPU* cpu, const struct RAM* ram) {
   ld_ins(cpu, ram, &cpu->y, EMPTY_ADDR);
}

static void ldy_zp(struct CPU* cpu, const struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, NONE);
   ld_ins(cpu, ram, &cpu->y, addr);
}

static void ldy_zp_x(struct CPU* cpu, const struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, X);
   ld_ins(cpu, ram, &cpu->y, addr);
}

static void ldy_abs(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, NONE, false);
   ld_ins(cpu, ram, &cpu->y, addr);
}

static void ldy_abs_x(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, X, true);
   ld_ins(cpu, ram, &cpu->y, addr);
}

static void sta_zp(struct CPU* cpu, struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, NONE);
   w_byte_to_mem(cpu->a, addr, ram, &cpu->cycles);
}

static void sta_zp_x(struct CPU* cpu, struct RAM* ram) {
   word addr = zp_addr(cpu, ram, X);
   w_byte_to_mem(cpu->a, addr, ram, &cpu->cycles);
}

static void sta_abs(struct CPU* cpu, struct RAM* ram) {
   word addr = abs_addr(cpu, ram, NONE, false);
   w_byte_to_mem(cpu->a, addr, ram, &cpu->cycles);
}

static void sta_abs_x(struct CPU* cpu, struct RAM* ram) {
   word addr = abs_addr(cpu, ram, X, false);
   w_byte_to_mem(cpu->a, addr, ram, &cpu->cycles);
}

static void sta_abs_y(struct CPU* cpu, struct RAM* ram) {
   word addr = abs_addr(cpu, ram, Y, false);
   w_byte_to_mem(cpu->a, addr, ram, &cpu->cycles);
}

static void sta_ind_x(struct CPU* cpu, struct RAM* ram) {
   word addr = ind_addr(cpu, ram, X, false);
   w_byte_to_mem(cpu->a, addr, ram, &cpu->cycles);
}

static void sta_ind_y(struct CPU* cpu, struct RAM* ram) {
   word addr = ind_addr(cpu, ram, Y, false);
   w_byte_to_mem(cpu->a, addr, ram, &cpu->cycles);
}

static void stx_zp(struct CPU* cpu, struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, NONE);
   w_byte_to_mem(cpu->x, addr, ram, &cpu->cycles);
}

static void stx_zp_y(struct CPU* cpu, struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, Y);
   w_byte_to_mem(cpu->x, addr, ram, &cpu->cycles);
}

static void stx_abs(struct CPU* cpu, struct RAM* ram) {
   word addr = abs_addr(cpu, ram, NONE, false);
   w_byte_to_mem(cpu->x, addr, ram, &cpu->cycles);
}

static void sty_zp(struct CPU* cpu, struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, NONE);
   w_byte_to_mem(cpu->y, addr, ram, &cpu->cycles);
}

static void sty_zp_x(struct CPU* cpu, struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, X);
   w_byte_to_mem(cpu->y, addr, ram, &cpu->cycles);
}

static void sty_abs(struct CPU* cpu, struct RAM* ram) {
   word addr = abs_addr(cpu, ram, NONE, false);
   w_byte_to_mem(cpu->y, addr, ram, &cpu->cycles);
}

static void tax(struct CPU* cpu, const struct RAM* ram) {
   (void)ram; //unused

   cpu->x = cpu->a;
   cpu->cycles++;
   set_zn_flags(cpu, cpu->x);
}

static void txa(struct CPU* cpu, const struct RAM* ram) {
   (void)ram; //unused

   cpu->a = cpu->x;
   cpu->cycles++;
   set_zn_flags(cpu, cpu->a);
}

static void tay(struct CPU* cpu, const struct RAM* ram) {
   (void)ram; //unused

   cpu->y = cpu->a;
   cpu->cycles++;
   set_zn_flags(cpu, cpu->y);
}

static void tya(struct CPU* cpu, const struct RAM* ram) {
   (void)ram; //unused

   cpu->a = cpu->y;
   cpu->cycles++;
   set_zn_flags(cpu, cpu->a);
}

static void tsx(struct CPU* cpu, const struct RAM* ram) {
   (void)ram;

   cpu->x = (cpu->sp & 0x00FF);
   cpu->cycles++;
   set_zn_flags(cpu, cpu->x);
}

static void txs(struct CPU* cpu, const struct RAM* ram) {
   (void)ram;

   cpu->sp = 0x0100 | cpu->x;
   cpu->cycles++;
}

static void pha(struct CPU* cpu, struct RAM* ram) {
   push_byte_to_stack(ram, &cpu->sp, cpu->a, &cpu->cycles);
   cpu->cycles++;
}

static void php(struct CPU* cpu, struct RAM* ram) {
   push_byte_to_stack(ram, &cpu->sp, cpu->ps, &cpu->cycles);
   cpu->cycles++;
}

static void pla(struct CPU* cpu, const struct RAM* ram) {
   byte val = pop_byte_from_stack(ram, &cpu->sp, &cpu->cycles);
   cpu->a = val;
   cpu->cycles += 2;
   set_zn_flags(cpu, cpu->a);
}

static void plp(struct CPU* cpu, const struct RAM* ram) {
   byte val = pop_byte_from_stack(ram, &cpu->sp, &cpu->cycles);
   cpu->ps = val;
   cpu->cycles++;

   cpu->c = (val & 0b00000001) != 0;
   cpu->n = (val & 0b10000000) != 0;
   cpu->v = (val & 0b01000000) != 0;
   cpu->i = (val & 0b00010000) != 0;
   cpu->d = (val & 0b00001000) != 0;
   cpu->b = (val & 0b00000100) != 0;
   cpu->v = (val & 0b00000010) != 0;
   cpu->cycles++;
}

static void and_imm(struct CPU* cpu, const struct RAM* ram) {
   logic_ins(cpu, ram, EMPTY_ADDR, AND);
}

static void and_zp(struct CPU* cpu, const struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, NONE);
   logic_ins(cpu, ram, addr, AND);
}

static void and_zp_x(struct CPU* cpu, const struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, X);
   logic_ins(cpu, ram, addr, AND);
}

static void and_abs(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, NONE, false);
   logic_ins(cpu, ram, addr, AND);
}

static void and_abs_x(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, X, true);
   logic_ins(cpu, ram, addr, AND);
}

static void and_abs_y(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, Y, true);
   logic_ins(cpu, ram, addr, AND);
}

static void and_ind_x(struct CPU* cpu, const struct RAM* ram) {
   word addr = ind_addr(cpu, ram, X, false);
   logic_ins(cpu, ram, addr, AND);
}

static void and_ind_y(struct CPU* cpu, const struct RAM* ram) {
   word addr = ind_addr(cpu, ram, Y, true);
   logic_ins(cpu, ram, addr, AND);
}

static void eor_imm(struct CPU* cpu, const struct RAM* ram) {
   logic_ins(cpu, ram, EMPTY_ADDR, EOR);
}

static void eor_zp(struct CPU* cpu, const struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, NONE);
   logic_ins(cpu, ram, addr, EOR);
}

static void eor_zp_x(struct CPU* cpu, const struct RAM* ram) {
   byte addr = zp_addr(cpu, ram, X);
   logic_ins(cpu, ram, addr, EOR);
}

static void eor_abs(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, NONE, false);
   logic_ins(cpu, ram, addr, EOR);
}

static void eor_abs_x(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, X, true);
   logic_ins(cpu, ram, addr, EOR);
}

static void eor_abs_y(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, Y, true);
   logic_ins(cpu, ram, addr, EOR);
}

static void eor_ind_x(struct CPU* cpu, const struct RAM* ram) {
   word addr = ind_addr(cpu, ram, X, false);
   logic_ins(cpu, ram, addr, EOR);
}

static void eor_ind_y(struct CPU* cpu, const struct RAM* ram) {
   word addr = ind_addr(cpu, ram, Y, true);
   logic_ins(cpu, ram, addr, EOR);
}

static void ora_imm(struct CPU* cpu, const struct RAM* ram) {
   logic_ins(cpu, ram, EMPTY_ADDR, ORA);
}

static void ora_zp(struct CPU* cpu, const struct RAM* ram) {
   word addr = zp_addr(cpu, ram, NONE);
   logic_ins(cpu, ram, addr, ORA);
}

static void ora_zp_x(struct CPU* cpu, const struct RAM* ram) {
   word addr = zp_addr(cpu, ram, X);
   logic_ins(cpu, ram, addr, ORA);
}

static void ora_abs(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, NONE, false);
   logic_ins(cpu, ram, addr, ORA);
}

static void ora_abs_x(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, X, true);
   logic_ins(cpu, ram, addr, ORA);
}

static void ora_abs_y(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, Y, true);
   logic_ins(cpu, ram, addr, ORA);
}

static void ora_ind_x(struct CPU* cpu, const struct RAM* ram) {
   word addr = ind_addr(cpu, ram, X, false);
   logic_ins(cpu, ram, addr, ORA);
}

static void ora_ind_y(struct CPU* cpu, const struct RAM* ram) {
   word addr = ind_addr(cpu, ram, Y, true);
   logic_ins(cpu, ram, addr, ORA);
}

static void bit_zp(struct CPU* cpu, const struct RAM* ram) {
   word addr = zp_addr(cpu, ram, NONE);
   byte val = r_byte_from_addr(addr, ram, &cpu->cycles);

   byte res = cpu->a & val;
   set_zn_flags(cpu, res);
   cpu->v = (res & 0x6) != 0;
   update_ps(cpu);
}

static void bit_abs(struct CPU* cpu, const struct RAM* ram) {
   word addr = abs_addr(cpu, ram, NONE, false);
   byte val = r_byte_from_addr(addr, ram, &cpu->cycles);

   byte res = cpu->a & val;
   set_zn_flags(cpu, res);
   cpu->v = (res & 0x6) != 0;
   update_ps(cpu);
}

static void adc_imm(struct CPU* cpu, const struct RAM* ram) {
   byte val = r_byte_from_pc(&cpu->pc, ram, &cpu->cycles);
   word sum = cpu->a + val + cpu->c;
   cpu->a = (byte)sum;

   set_zn_flags(cpu, cpu->a);
   set_cv_flags(cpu, sum);
}

#pragma endregion

//Instruction map.
void(*insTable[256])(struct CPU* cpu, struct RAM* ram) = {
   [JSR] = &jsr,
   [RTS] = &rts,
   [LDA_IM] = &lda_imm,
   [LDA_ZP] = &lda_zp,
   [LDA_ZPX] = &lda_zp_X,
   [LDA_ABS] = &lda_abs,
   [LDA_ABSX] = &lda_abs_X,
   [LDA_ABSY] = &lda_abs_Y,
   [LDA_INDX] = &lda_ind_X,
   [LDA_INDY] = &lda_ind_Y,
   [LDX_IM] = &ldx_imm,
   [LDX_ZP] = &ldx_zp,
   [LDX_ZPY] = &ldx_zp_y,
   [LDX_ABS] = &ldx_abs,
   [LDX_ABSY] = &ldx_abs_y,
   [LDY_IM] = &ldy_imm,
   [LDY_ZP] = &ldy_zp,
   [LDY_ZPX] = &ldy_zp_x,
   [LDY_ABS] = &ldy_abs,
   [LDY_ABSX] = &ldy_abs_x,
   [STA_ZP] = &sta_zp,
   [STA_ZPX] = &sta_zp_x,
   [STA_ABS] = &sta_abs,
   [STA_ABSX] = &sta_abs_x,
   [STA_ABSY] = &sta_abs_y,
   [STA_INDX] = &sta_ind_x,
   [STA_INDY] = &sta_ind_y,
   [STX_ZP] = &stx_zp,
   [STX_ZPY] = &stx_zp_y,
   [STX_ABS] = &stx_abs,
   [STY_ZP] = &sty_zp,
   [STY_ZPX] = &sty_zp_x,
   [STY_ABS] = &sty_abs,
   [TAX] = &tax,
   [TXA] = &txa,
   [TAY] = &tay,
   [TYA] = &tya,
   [TSX] = &tsx,
   [TXS] = &txs,
   [PHA] = &pha,
   [PHP] = &php,
   [PLA] = &pla,
   [PLP] = &plp,
   [AND_IM] = &and_imm,
   [AND_ZP] = &and_zp,
   [AND_ZPX] = &and_zp_x,
   [AND_ABS] = &and_abs,
   [AND_ABSX] = &and_abs_x,
   [AND_ABSY] = &and_abs_y,
   [AND_INDX] = &and_ind_x,
   [AND_INDY] = &and_ind_y,
   [EOR_IM] = &eor_imm,
   [EOR_ZP] = &eor_zp,
   [EOR_ZPX] = &eor_zp_x,
   [EOR_ABS] = &eor_abs,
   [EOR_ABSX] = &eor_abs_x,
   [EOR_ABSY] = &eor_abs_y,
   [EOR_INDX] = &eor_ind_x,
   [EOR_INDY] = &eor_ind_y,
   [ORA_IM] = &ora_imm,
   [ORA_ZP] = &ora_zp,
   [ORA_ZPX] = &ora_zp_x,
   [ORA_ABS] = &ora_abs,
   [ORA_ABSX] = &ora_abs_x,
   [ORA_ABSY] = &ora_abs_y,
   [ORA_INDX] = &ora_ind_x,
   [ORA_INDY] = &ora_ind_y,
   [BIT_ZP] = &bit_zp,
   [BIT_ABS] = &bit_abs,
   [ADC_IM] = &adc_imm,
};

struct RAM* init_ram() {
   struct RAM* ram = malloc(sizeof(struct RAM));
   if (NULL == ram) {
      printf_s("Allocation error.");
      return NULL;
   }

   ram->data = (byte*)calloc(MEM_MAX, sizeof(byte)); //Allocate MEM_MAX*1B on the heap.
   if (NULL == ram->data) {
      printf_s("Allocation error");
      free(ram);
      return NULL;
   }

#ifdef _DEBUG
   printf_s("DEBUG\t| Initialized RAM\n");
#endif // _DEBUG

   return ram;
}

//Does not set RAM ptr to NULL.
void free_ram(struct RAM* ram) {
   free(ram->data);
   free(ram);

#ifdef _DEBUG
   printf_s("DEBUG\t| Freed allocated memory\n");
#endif //_DEBUG
}

void reset_cpu(struct CPU* cpu, word sPC) {
   cpu->pc = sPC;
   cpu->sp = 0x01FF;

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

   cpu->cycles = 0;

#ifdef _DEBUG
   printf_s("DEBUG\t| Reset CPU\n");
#endif // _DEBUG
}

int exec(struct CPU* cpu, struct RAM* ram, uint32_t insCount) {
   for (uint32_t i = insCount; i > 0; i--) {
      byte opCode = r_byte_from_pc(&cpu->pc, ram, &cpu->cycles);
      
      if (NULL != insTable[opCode]) {
         insTable[opCode](cpu, ram);
      }
      else {
         return 1;
      }
   }

   return 0;
}
