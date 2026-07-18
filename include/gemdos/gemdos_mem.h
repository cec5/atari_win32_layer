#ifndef GEMDOS_MEM_H
#define GEMDOS_MEM_H

#include <stdint.h>

void gemdos_mem_init(unsigned int heap_start, unsigned int heap_end);

unsigned int gemdos_mem_malloc(int32_t size);

int32_t gemdos_mem_free(unsigned int addr);

int32_t gemdos_mem_shrink(unsigned int addr, uint32_t newsize);

// GEMDOS trap handlers (Malloc/Mfree/Mshrink, TRAP #1)
unsigned int gemdos_malloc(unsigned int args_addr);
unsigned int gemdos_mfree(unsigned int args_addr);
unsigned int gemdos_mshrink(unsigned int args_addr);

#endif
