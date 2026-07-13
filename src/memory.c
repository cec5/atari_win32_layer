#include <stdint.h>
#include <stdio.h>

#define MAX_MEM (16 * 1024 * 1024)
uint8_t virtual_ram[MAX_MEM];

// Double checks boundary
static inline int check_bounds(unsigned int address) {
    if (address >= MAX_MEM) {
        printf("Out of bounds memory access at 0x%08X\n", address);
        return 0;
    }
    return 1;
}

// MUSASHI READ CALLBACKS
unsigned int m68k_read_memory_8(unsigned int address) {
    if (!check_bounds(address)) return 0;
    return virtual_ram[address];
}

unsigned int m68k_read_memory_16(unsigned int address) {
    if (!check_bounds(address + 1)) return 0;
    return (virtual_ram[address] << 8) | virtual_ram[address + 1];
}

unsigned int m68k_read_memory_32(unsigned int address) {
    if (!check_bounds(address + 3)) return 0;
    return (virtual_ram[address] << 24) | 
           (virtual_ram[address + 1] << 16) | 
           (virtual_ram[address + 2] << 8) | 
           virtual_ram[address + 3];
}

// MUSASHI WRITE CALLBACKS
void m68k_write_memory_8(unsigned int address, unsigned int value) {
    if (!check_bounds(address)) return;
    virtual_ram[address] = value & 0xFF;
}

void m68k_write_memory_16(unsigned int address, unsigned int value) {
    if (!check_bounds(address + 1)) return;
    virtual_ram[address]     = (value >> 8) & 0xFF;
    virtual_ram[address + 1] = value & 0xFF;
}

void m68k_write_memory_32(unsigned int address, unsigned int value) {
    if (!check_bounds(address + 3)) return;
    virtual_ram[address]     = (value >> 24) & 0xFF;
    virtual_ram[address + 1] = (value >> 16) & 0xFF;
    virtual_ram[address + 2] = (value >> 8) & 0xFF;
    virtual_ram[address + 3] = value & 0xFF;
}

// (BANDAID-FIX) DISSEMBLER CALLBACKS (Fine for now, but needs to be properly written)

unsigned int m68k_read_disassembler_8(unsigned int address) {
    return m68k_read_memory_8(address);
}

unsigned int m68k_read_disassembler_16(unsigned int address) {
    return m68k_read_memory_16(address);
}

unsigned int m68k_read_disassembler_32(unsigned int address) {
    return m68k_read_memory_32(address);
}