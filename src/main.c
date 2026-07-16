#include <stdio.h>
#include <stdint.h>
#include "m68k.h"
#include "memory.h"
#include "tos_layer.h"

#define PROGRAM_ADDR 0x00001000u
#define STRING_ADDR  0x00002000u
#define INITIAL_SSP  0x00FFFF00u

// Tiny TOS program that calls GEMDOS Cconws to print a string, then Pterm0 to exit - the smallest possible trap-based "Hello World"

static void load_hello_world(void) {
    m68k_write_memory_32(0x00000000u, INITIAL_SSP);
    m68k_write_memory_32(0x00000004u, PROGRAM_ADDR);

    const char *msg = "Hello, TOS World!\r\n";
    unsigned int addr = STRING_ADDR;
    for (const char *p = msg; *p; p++) {
        m68k_write_memory_8(addr++, (unsigned char)*p);
    }
    m68k_write_memory_8(addr, 0x00);

    unsigned int pc = PROGRAM_ADDR;

    m68k_write_memory_16(pc, 0x4879); pc += 2;
    m68k_write_memory_32(pc, STRING_ADDR); pc += 4;

    m68k_write_memory_16(pc, 0x3F3C); pc += 2;
    m68k_write_memory_16(pc, 0x0009); pc += 2;

    m68k_write_memory_16(pc, 0x4E41); pc += 2;

    m68k_write_memory_16(pc, 0x5C8F); pc += 2;

    m68k_write_memory_16(pc, 0x3F3C); pc += 2;
    m68k_write_memory_16(pc, 0x0000); pc += 2;

    m68k_write_memory_16(pc, 0x4E41); pc += 2;

    m68k_write_memory_16(pc, 0x60FE); pc += 2;
}

int main() {
    printf("Initializing Virtual Environment...\n");

    memory_init();
    load_hello_world();

    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    tos_layer_init();
    m68k_pulse_reset();

    printf("Starting CPU execution...\n\n");
    m68k_execute(1000);
    printf("\n");

    printf("Execution Halted.\n");

    return 0;
}