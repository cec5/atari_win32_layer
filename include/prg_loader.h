#ifndef PRG_LOADER_H
#define PRG_LOADER_H

typedef struct {
    unsigned int basepage_addr;
    unsigned int entry_addr;  // Where PC should start: basepage_addr + 0x100
    unsigned int stack_addr;  // Initial SSP: top of the allocated TPA
} PrgLoadResult;

/*
 * Loads a GEMDOS .PRG/.TOS executable from the host filesystem into
 * virtual_ram at load_addr, applying its relocation table and building a
 * basepage. Returns 1 and fills *result on success, 0 on failure (bad
 * magic, truncated file, or it doesn't fit in RAM).
 */
int prg_load(const char *host_path, unsigned int load_addr, PrgLoadResult *result);

#endif
