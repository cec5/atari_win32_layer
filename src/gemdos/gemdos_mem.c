#include <stdlib.h>
#include "m68k.h"
#include "gemdos/gemdos_mem.h"

#define ALIGN_UP(n, a) (((n) + (a) - 1) & ~((unsigned int)(a) - 1))

#define ERR_INVALID_ADDR -36

/*
 * A first-fit free-list allocator over the pool. Bookkeeping nodes live
 * in host memory (plain malloc/free) - only addr/size describe guest
 * (virtual_ram) space. This lets Mfree/Mshrink actually reclaim and reuse
 * memory, unlike a bump allocator.
 */
typedef struct MemBlock {
    unsigned int addr;
    unsigned int size;
    int is_free;
    struct MemBlock *next;
} MemBlock;

static MemBlock *s_head = NULL;

void gemdos_mem_init(unsigned int heap_start, unsigned int heap_end) {
    while (s_head) {
        MemBlock *next = s_head->next;
        free(s_head);
        s_head = next;
    }

    if (heap_end <= heap_start) {
        return;
    }

    s_head = malloc(sizeof(MemBlock));
    s_head->addr = heap_start;
    s_head->size = heap_end - heap_start;
    s_head->is_free = 1;
    s_head->next = NULL;
}

static unsigned int largest_free_block(void) {
    unsigned int largest = 0;
    for (MemBlock *b = s_head; b; b = b->next) {
        if (b->is_free && b->size > largest) {
            largest = b->size;
        }
    }
    return largest;
}

// Splits off a new free block covering the tail of b (from 'used' bytes in to the end of b) and inserts it right after b in the list
static void split_tail(MemBlock *b, unsigned int used) {
    if (used >= b->size) {
        return;
    }

    MemBlock *remainder = malloc(sizeof(MemBlock));
    remainder->addr = b->addr + used;
    remainder->size = b->size - used;
    remainder->is_free = 1;
    remainder->next = b->next;

    b->size = used;
    b->next = remainder;
}

// Merges b with however many immediately-following free blocks are themselves free, so adjacent free space never stays fragmented
static void coalesce_forward(MemBlock *b) {
    while (b && b->is_free && b->next && b->next->is_free) {
        MemBlock *victim = b->next;
        b->size += victim->size;
        b->next = victim->next;
        free(victim);
    }
}

unsigned int gemdos_mem_malloc(int32_t size) {
    if (size == -1) {
        return largest_free_block();
    }
    if (size <= 0) {
        return 0;
    }

    unsigned int want = ALIGN_UP((unsigned int)size, 4);

    for (MemBlock *b = s_head; b; b = b->next) {
        if (!b->is_free || b->size < want) {
            continue;
        }

        split_tail(b, want);
        b->is_free = 0;
        return b->addr;
    }

    return 0;
}

static MemBlock *find_block(unsigned int addr, MemBlock **out_prev) {
    MemBlock *prev = NULL;
    for (MemBlock *b = s_head; b; b = b->next) {
        if (b->addr == addr) {
            if (out_prev) {
                *out_prev = prev;
            }
            return b;
        }
        prev = b;
    }
    return NULL;
}

int32_t gemdos_mem_free(unsigned int addr) {
    MemBlock *prev = NULL;
    MemBlock *b = find_block(addr, &prev);
    if (!b || b->is_free) {
        return ERR_INVALID_ADDR;
    }

    b->is_free = 1;
    coalesce_forward(b);
    if (prev) {
        coalesce_forward(prev);
    }

    return 0;
}

int32_t gemdos_mem_shrink(unsigned int addr, uint32_t newsize) {
    MemBlock *b = find_block(addr, NULL);
    if (!b || b->is_free) {
        return ERR_INVALID_ADDR;
    }

    unsigned int want = ALIGN_UP(newsize, 4);
    if (want >= b->size) {
        return 0;
    }

    split_tail(b, want);
    coalesce_forward(b->next);

    return 0;
}

/* GEMDOS trap handlers */

// Malloc(LONG number)
unsigned int gemdos_malloc(unsigned int args_addr) {
    int32_t size = (int32_t)m68k_read_memory_32(args_addr);
    return gemdos_mem_malloc(size);
}

// Mfree(VOID *block)
unsigned int gemdos_mfree(unsigned int args_addr) {
    unsigned int block = m68k_read_memory_32(args_addr);
    return (unsigned int)gemdos_mem_free(block);
}

// Mshrink(VOID *dummy, VOID *block, LONG newsize)
unsigned int gemdos_mshrink(unsigned int args_addr) {
    unsigned int block = m68k_read_memory_32(args_addr + 4);
    uint32_t newsize = m68k_read_memory_32(args_addr + 8);
    return (unsigned int)gemdos_mem_shrink(block, newsize);
}