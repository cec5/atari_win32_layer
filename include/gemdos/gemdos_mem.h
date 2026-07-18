#ifndef GEMDOS_MEM_H
#define GEMDOS_MEM_H

#include <stdint.h>

/*
 * Initializes the GEMDOS memory pool backing Malloc/Mfree/Mshrink, as a
 * single free block spanning [heap_start, heap_end) - the unused RAM left
 * over above a loaded program's own TPA.
 */
void gemdos_mem_init(unsigned int heap_start, unsigned int heap_end);

/*
 * Malloc(n): n > 0 allocates n bytes and returns the block's address, or
 * 0 if no free block is large enough. n == -1 returns the size of the
 * largest free block instead of an address, per the GEMDOS convention.
 */
unsigned int gemdos_mem_malloc(int32_t size);

/* Mfree(addr): frees a block previously returned by gemdos_mem_malloc.
 * Returns 0 on success, a negative GEMDOS error code if addr isn't a
 * live allocation. */
int32_t gemdos_mem_free(unsigned int addr);

/* Mshrink(addr, newsize): shrinks an allocated block down to newsize
 * bytes, releasing the remainder back to the pool. Growing is not
 * supported (matches real GEMDOS) and is treated as a no-op success.
 * Returns 0 on success, a negative GEMDOS error code if addr isn't a
 * live allocation. */
int32_t gemdos_mem_shrink(unsigned int addr, uint32_t newsize);

/* GEMDOS trap handlers (Malloc/Mfree/Mshrink, TRAP #1): read args from
 * the guest stack and call the API above. */
unsigned int gemdos_malloc(unsigned int args_addr);
unsigned int gemdos_mfree(unsigned int args_addr);
unsigned int gemdos_mshrink(unsigned int args_addr);

#endif
