#include <stdio.h>
#include "m68k.h"
#include "gemdos.h"
#include "tos_layer.h"

#define GEMDOS_PTERM0 0x00
#define GEMDOS_CCONWS 0x09
#define GEMDOS_PTERM  0x4C

/* Cconws(char *str) - write a NUL-terminated string to the console. */
static unsigned int gemdos_cconws(unsigned int args_addr) {
    unsigned int str_addr = m68k_read_memory_32(args_addr);

    for (unsigned int i = str_addr; ; i++) {
        unsigned char c = (unsigned char)m68k_read_memory_8(i);
        if (c == 0) {
            break;
        }
        putchar(c);
    }
    fflush(stdout);

    return 0;
}

unsigned int gemdos_dispatch(unsigned int func_num, unsigned int args_addr) {
    switch (func_num) {
        case GEMDOS_CCONWS:
            return gemdos_cconws(args_addr);

        case GEMDOS_PTERM0:
        case GEMDOS_PTERM:
            tos_request_halt();
            return 0;

        default:
            printf("[gemdos] Unimplemented GEMDOS function 0x%02X\n", func_num);
            return 0;
    }
}
