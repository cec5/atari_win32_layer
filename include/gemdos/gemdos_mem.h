#ifndef GEMDOS_MEM_H
#define GEMDOS_MEM_H

#include <stdint.h>

// Sets up the GEMDOS memory pool backing Malloc/Mfree/Mshrink as a single
// free block spanning [heap_start, heap_end), the RAM left over above a
// loaded program's own TPA.
void gemdos_mem_init(unsigned int heap_start, unsigned int heap_end);

// Malloc(n): n > 0 allocates n bytes and returns the address, or 0 if
// nothing is free enough. n == -1 returns the size of the largest free
// block instead.
unsigned int gemdos_mem_malloc(int32_t size);

// Mfree(addr): frees a block from gemdos_mem_malloc. Returns 0 on
// success, a negative GEMDOS error code if addr isn't a live allocation.
int32_t gemdos_mem_free(unsigned int addr);

// Mshrink(addr, newsize): shrinks an allocation, releasing the remainder
// back to the pool. Growing isn't supported and is treated as a no-op success.
int32_t gemdos_mem_shrink(unsigned int addr, uint32_t newsize);

// GEMDOS trap handlers (Malloc/Mfree/Mshrink, TRAP #1)
unsigned int gemdos_malloc(unsigned int args_addr);
unsigned int gemdos_mfree(unsigned int args_addr);
unsigned int gemdos_mshrink(unsigned int args_addr);

#endif
