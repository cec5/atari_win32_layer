#ifndef GUEST_MEM_UTIL_H
#define GUEST_MEM_UTIL_H

#include <stddef.h>

// Copies a NUL-terminated string from guest memory into a host buffer,
// truncating to max_len; 1 bytes plus a terminator.
unsigned int guest_read_cstring(unsigned int addr, char *out, size_t max_len);

#endif