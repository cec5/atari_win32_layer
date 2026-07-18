#include <stdio.h>
#include "gemdos/gemdos.h"
#include "gemdos/gemdos_console.h"
#include "gemdos/gemdos_file.h"
#include "gemdos/gemdos_mem.h"
#include "tos_layer.h"

/*
 * This file only routes a GEMDOS function number to its handler. The
 * handlers themselves, argument marshalling and the actual work, live
 * in the category file for that call (gemdos_console.c, gemdos_file.c, gemdos_mem.c, ...)
 */
#define GEMDOS_PTERM0  0x00
#define GEMDOS_CCONWS  0x09
#define GEMDOS_FCREATE 0x3C
#define GEMDOS_FOPEN   0x3D
#define GEMDOS_FCLOSE  0x3E
#define GEMDOS_FREAD   0x3F
#define GEMDOS_FWRITE  0x40
#define GEMDOS_FSEEK   0x42
#define GEMDOS_MALLOC  0x48
#define GEMDOS_MFREE   0x49
#define GEMDOS_MSHRINK 0x4A
#define GEMDOS_PTERM   0x4C
#define GEMDOS_FCNTL   0x104

unsigned int gemdos_dispatch(unsigned int func_num, unsigned int args_addr) {
    switch (func_num) {
        case GEMDOS_CCONWS:
            return gemdos_cconws(args_addr);

        case GEMDOS_FCREATE:
            return gemdos_fcreate(args_addr);

        case GEMDOS_FOPEN:
            return gemdos_fopen(args_addr);

        case GEMDOS_FCLOSE:
            return gemdos_fclose(args_addr);

        case GEMDOS_FREAD:
            return gemdos_fread(args_addr);

        case GEMDOS_FWRITE:
            return gemdos_fwrite(args_addr);

        case GEMDOS_FSEEK:
            return gemdos_fseek(args_addr);

        case GEMDOS_MALLOC:
            return gemdos_malloc(args_addr);

        case GEMDOS_MFREE:
            return gemdos_mfree(args_addr);

        case GEMDOS_MSHRINK:
            return gemdos_mshrink(args_addr);

        case GEMDOS_PTERM0:
        case GEMDOS_PTERM:
            tos_request_halt();
            return 0;

        case GEMDOS_FCNTL:
            // We are not (currently) modeling MiNT Exentsions for file-descriptor control (flags, isatty, probes, etc.); reporting success let's the startup continue
            return 0;

        default:
            printf("[gemdos] Unimplemented GEMDOS function 0x%02X\n", func_num);
            return 0;
    }
}