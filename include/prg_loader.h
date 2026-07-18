#ifndef PRG_LOADER_H
#define PRG_LOADER_H

typedef struct {
    unsigned int basepage_addr;
    unsigned int entry_addr;  // Where PC should start: basepage_addr + 0x100
    unsigned int stack_addr;  // Initial SSP: top of the allocated TPA
} PrgLoadResult;

int prg_load(const char *host_path, unsigned int load_addr, PrgLoadResult *result);

#endif
