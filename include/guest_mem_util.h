#ifndef GUEST_MEM_UTIL_H
#define GUEST_MEM_UTIL_H

#include <stddef.h>

/*
 * Copies a NUL-terminated string out of guest memory at addr into a host
 * buffer, truncating to max_len - 1 bytes plus a NUL terminator. Shared
 * by any trap handler (GEMDOS/BIOS/XBIOS) that takes a string pointer -
 * filenames, console strings, etc. Returns the number of bytes copied,
 * excluding the terminator.
 */
unsigned int guest_read_cstring(unsigned int addr, char *out, size_t max_len);

#endif
