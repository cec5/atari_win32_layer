#include <stdio.h>
#include <stdint.h>
#include "m68k.h" 

extern uint8_t virtual_ram[];

void load_test_program() {
    virtual_ram[0] = 0x00; virtual_ram[1] = 0xFF; virtual_ram[2] = 0xFF; virtual_ram[3] = 0x00;
    virtual_ram[4] = 0x00; virtual_ram[5] = 0x00; virtual_ram[6] = 0x04; virtual_ram[7] = 0x00;

    // MOVE.L #$12345678, D0 (Machine Code: 20 3C 12 34 56 78)
    virtual_ram[0x400] = 0x20; virtual_ram[0x401] = 0x3C;
    virtual_ram[0x402] = 0x12; virtual_ram[0x403] = 0x34;
    virtual_ram[0x404] = 0x56; virtual_ram[0x405] = 0x78;

    // NOP (Machine Code: 4E 71)
    virtual_ram[0x406] = 0x4E; virtual_ram[0x407] = 0x71;

    // BRA.S $408 (Branch to Self)
    virtual_ram[0x408] = 0x60; virtual_ram[0x409] = 0xFE;
}

int main() {
    printf("Initializing Virtual Environment...\n");

    load_test_program();
    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    m68k_pulse_reset();

    printf("Starting CPU execution...\n");
    m68k_execute(100);

    unsigned int d0_value = m68k_get_reg(NULL, M68K_REG_D0);
    unsigned int pc_value = m68k_get_reg(NULL, M68K_REG_PC);

    printf("Execution Halted.\n");
    printf("Register D0: 0x%08X (Expected: 0x12345678)\n", d0_value);
    printf("Current PC : 0x%08X (Expected: 0x00000408)\n", pc_value);

    return 0;
}