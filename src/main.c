#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "m68k.h"
#include "memory.h"
#include "tos_layer.h"
#include "prg_loader.h"
#include "gemdos/gemdos_mem.h"
#include "gemdos/gemdos_file.h"
#include "logger.h"
#include "host_paths.h"

#define LOAD_ADDR 0x00001000u

#define DEFAULT_PRG_PATH "test_programs/hello.prg"

/* Runs in chunks rather than a single fixed cycle count, since real
 * programs vary wildly in how much work they do before calling
 * Pterm/Pterm0. MAX_TOTAL_CYCLES is just a safety net against a genuinely
 * broken/infinite-looping guest program hanging the host forever. */
#define EXECUTION_CHUNK_CYCLES 1000000
#define MAX_TOTAL_CYCLES       500000000

int main(int argc, char *argv[]) {
    int debug_mode = 0;
    const char *prg_arg = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0) {
            debug_mode = 1;
        } else if (!prg_arg) {
            prg_arg = argv[i];
        }
    }

    char default_path[MAX_PATH];
    resolve_project_path(DEFAULT_PRG_PATH, default_path, sizeof(default_path));

    const char *prg_path = prg_arg ? prg_arg : default_path;

    if (debug_mode) {
        logger_init_for_program(prg_path);
        const char *log_path = logger_get_log_path();
        if (log_path) {
            log_write(LOG_INFO, "AtariWin32Layer starting for '%s'", prg_path);
            printf("Debug logging enabled: %s\n", log_path);
        }
    }

    printf("Initializing Virtual Environment...\n");

    memory_init();

    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    tos_layer_init();
    gemdos_file_init();

    PrgLoadResult prg;
    if (!prg_load(prg_path, LOAD_ADDR, &prg)) {
        fprintf(stderr, "Usage: %s [--debug] <path-to-.prg>\n", argv[0]);
        log_write(LOG_ERROR, "Failed to load '%s'", prg_path);
        logger_shutdown();
        return 1;
    }

    log_write(LOG_INFO, "Loaded '%s': basepage=0x%08X entry=0x%08X stack_top=0x%08X", prg_path, prg.basepage_addr, prg.entry_addr, prg.stack_addr);
    log_write(LOG_INFO, "Segments: TEXT=0x%08X(%u bytes) DATA=0x%08X(%u bytes) BSS=0x%08X(%u bytes)",prg.text_addr, prg.text_len, prg.data_addr, prg.data_len, prg.bss_addr, prg.bss_len);

    gemdos_mem_init(prg.stack_addr, MAX_MEM);

    m68k_write_memory_32(0x00000000u, prg.stack_addr);
    m68k_write_memory_32(0x00000004u, prg.entry_addr);

    m68k_pulse_reset();

    printf("Starting CPU execution...\n\n");
    log_write(LOG_INFO, "CPU execution starting");

    unsigned long total_cycles = 0;
    while (!tos_is_halted() && total_cycles < MAX_TOTAL_CYCLES) {
        total_cycles += (unsigned long)m68k_execute(EXECUTION_CHUNK_CYCLES);
    }

    printf("\n");

    if (!tos_is_halted()) {
        fprintf(stderr, "Warning: program did not call Pterm before the %lu cycle safety cap was reached.\n",
                (unsigned long)MAX_TOTAL_CYCLES);
        log_write(LOG_ERROR, "Program did not halt before the %lu cycle safety cap was reached",
                  (unsigned long)MAX_TOTAL_CYCLES);
    }

    printf("Execution Halted.\n");
    log_write(LOG_INFO, "Execution halted after %lu cycles", total_cycles);

    logger_shutdown();

    return 0;
}