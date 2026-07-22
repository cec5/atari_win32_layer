#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "m68k.h"
#include "memory.h"
#include "tos_layer.h"
#include "prg_loader.h"
#include "gemdos/gemdos_mem.h"
#include "gemdos/gemdos_file.h"

#define LOAD_ADDR 0x00001000u

#define DEFAULT_PRG_PATH "test_programs/hello.prg"

/* Runs in chunks rather than a single fixed cycle count, since real
 * programs vary wildly in how much work they do before calling
 * Pterm/Pterm0. MAX_TOTAL_CYCLES is just a safety net against a genuinely
 * broken/infinite-looping guest program hanging the host forever. */
#define EXECUTION_CHUNK_CYCLES 1000000
#define MAX_TOTAL_CYCLES       500000000

static void get_default_prg_path(char *out, size_t out_size) {
    char exe_path[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, exe_path, MAX_PATH);
    if (len == 0 || len == MAX_PATH) {
        snprintf(out, out_size, "%s", DEFAULT_PRG_PATH);
        return;
    }

    char *last_sep = strrchr(exe_path, '\\');
    if (last_sep) {
        *last_sep = '\0';
    }

    snprintf(out, out_size, "%s\\..\\..\\%s", exe_path, DEFAULT_PRG_PATH);
}

int main(int argc, char *argv[]) {
    printf("Initializing Virtual Environment...\n");

    memory_init();

    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    tos_layer_init();
    gemdos_file_init();

    char default_path[MAX_PATH];
    get_default_prg_path(default_path, sizeof(default_path));

    const char *prg_path = (argc >= 2) ? argv[1] : default_path;

    PrgLoadResult prg;
    if (!prg_load(prg_path, LOAD_ADDR, &prg)) {
        fprintf(stderr, "Usage: %s <path-to-.prg>\n", argv[0]);
        return 1;
    }

    gemdos_mem_init(prg.stack_addr, MAX_MEM);

    m68k_write_memory_32(0x00000000u, prg.stack_addr);
    m68k_write_memory_32(0x00000004u, prg.entry_addr);

    m68k_pulse_reset();

    printf("Starting CPU execution...\n\n");

    unsigned long total_cycles = 0;
    while (!tos_is_halted() && total_cycles < MAX_TOTAL_CYCLES) {
        total_cycles += (unsigned long)m68k_execute(EXECUTION_CHUNK_CYCLES);
    }

    printf("\n");

    if (!tos_is_halted()) {
        fprintf(stderr, "Warning: program did not call Pterm before the %lu cycle safety cap was reached.\n",
                (unsigned long)MAX_TOTAL_CYCLES);
    }

    printf("Execution Halted.\n");

    return 0;
}