#include "cpu.h"

int main() {
   CPU cpu;
   Mem mem;

   reset(&cpu, &mem);
   mem.data[0xFFFC] = INS_JSR;
   mem.data[0xFFFD] = 0x42;
   mem.data[0xFFFE] = 0x42;
   mem.data[0x4242] = INS_LDA_IM;
   mem.data[0x4243] = 0x82;
   mem.data[0x4244] = INS_RTS;
   mem.data[0xFFFF] = 0x24;
   exec(&cpu, &mem, 14);

   return 0;
}