#ifndef GEMDOS_CONSOLE_H
#define GEMDOS_CONSOLE_H

/* Cconws(char *str) trap handler: reads the string pointer from args_addr
 * and writes it out via the GEMDOS file layer's handle 1 (gemdos_file.h),
 * so console output stays consistent whether a program calls Cconws or
 * Fwrite(1, ...) to print. */
unsigned int gemdos_cconws(unsigned int args_addr);

#endif
