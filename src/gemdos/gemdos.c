#include <stdio.h>
#include "gemdos/gemdos.h"
#include "gemdos/gemdos_console.h"
#include "gemdos/gemdos_file.h"
#include "gemdos/gemdos_mem.h"
#include "tos_layer.h"
#include "logger.h"

/*
 * This file only routes a GEMDOS function number to its handler. The
 * handlers themselves - argument marshalling and the actual work - live
 * in the category file for that call (gemdos_console.c, gemdos_file.c,
 * gemdos_mem.c, ...).
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

static const char *gemdos_function_name(unsigned int func_num) {
    switch (func_num) {
        case GEMDOS_PTERM0:  return "Pterm0";
        case GEMDOS_CCONWS:  return "Cconws";
        case GEMDOS_FCREATE: return "Fcreate";
        case GEMDOS_FOPEN:   return "Fopen";
        case GEMDOS_FCLOSE:  return "Fclose";
        case GEMDOS_FREAD:   return "Fread";
        case GEMDOS_FWRITE:  return "Fwrite";
        case GEMDOS_FSEEK:   return "Fseek";
        case GEMDOS_MALLOC:  return "Malloc";
        case GEMDOS_MFREE:   return "Mfree";
        case GEMDOS_MSHRINK: return "Mshrink";
        case GEMDOS_PTERM:   return "Pterm";
        case GEMDOS_FCNTL:   return "Fcntl";
        default:             return "Unknown";
    }
}

unsigned int gemdos_dispatch(unsigned int func_num, unsigned int args_addr) {
    log_write(LOG_API, "GEMDOS %s (0x%02X) called", gemdos_function_name(func_num), func_num);

    unsigned int result = 0;

    switch (func_num) {
        case GEMDOS_CCONWS:
            result = gemdos_cconws(args_addr);
            break;

        case GEMDOS_FCREATE:
            result = gemdos_fcreate(args_addr);
            break;

        case GEMDOS_FOPEN:
            result = gemdos_fopen(args_addr);
            break;

        case GEMDOS_FCLOSE:
            result = gemdos_fclose(args_addr);
            break;

        case GEMDOS_FREAD:
            result = gemdos_fread(args_addr);
            break;

        case GEMDOS_FWRITE:
            result = gemdos_fwrite(args_addr);
            break;

        case GEMDOS_FSEEK:
            result = gemdos_fseek(args_addr);
            break;

        case GEMDOS_MALLOC:
            result = gemdos_malloc(args_addr);
            break;

        case GEMDOS_MFREE:
            result = gemdos_mfree(args_addr);
            break;

        case GEMDOS_MSHRINK:
            result = gemdos_mshrink(args_addr);
            break;

        case GEMDOS_PTERM0:
        case GEMDOS_PTERM:
            tos_request_halt();
            break;

        case GEMDOS_FCNTL:
            // MiNT extension for file-descriptor control (flags, isatty
            // probes, etc.) - not modeled; reporting success lets the
            // program's startup continue rather than treating it as fatal.
            break;

        default:
            printf("[gemdos] Unimplemented GEMDOS function 0x%02X\n", func_num);
            log_write(LOG_ERROR, "GEMDOS function 0x%02X is unimplemented", func_num);
            break;
    }

    log_write(LOG_API, "GEMDOS %s (0x%02X) returned D0=0x%08X", gemdos_function_name(func_num), func_num, result);

    return result;
}
