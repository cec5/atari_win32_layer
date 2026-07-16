#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "m68k.h"
#include "gemdos.h"
#include "tos_layer.h"

#define GEMDOS_PTERM0 0x00
#define GEMDOS_CCONWS 0x09
#define GEMDOS_PTERM  0x4C
#define CCONWS_MAX_LEN (1u * 1024u * 1024u)

static unsigned int gemdos_cconws(unsigned int args_addr) {
    unsigned int str_addr = m68k_read_memory_32(args_addr);

    unsigned int len = 0;
    while (len < CCONWS_MAX_LEN && m68k_read_memory_8(str_addr + len) != 0) {
        len++;
    }

    char *buffer = malloc(len);
    if (!buffer) {
        return 0;
    }
    for (unsigned int i = 0; i < len; i++) {
        buffer[i] = (char)m68k_read_memory_8(str_addr + i);
    }

    fflush(stdout);

    HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;
    if (!WriteConsoleA(stdout_handle, buffer, len, &written, NULL)) {
        WriteFile(stdout_handle, buffer, len, &written, NULL);
    }

    free(buffer);
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
