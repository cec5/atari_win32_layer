#include <stdio.h>
#include "m68k.h"
#include "tos_layer.h"
#include "gemdos.h"

static int s_halt_requested = 0;

/* Exception vector number for TRAP #n is 32 + n; its slot in the vector
 * table (based at VBR, which is left at 0) is (32 + n) * 4. */
#define TRAP_VECTOR_ADDR(n) ((32u + (n)) * 4u)

static void install_trap_vector(unsigned int trap_num, unsigned int handler_addr) {
    m68k_write_memory_32(TRAP_VECTOR_ADDR(trap_num), handler_addr);
}

/* On a real 68000, TRAP pushes a 3-word exception frame onto the
 * (supervisor) stack: SR at SP+0, return PC at SP+2..5. Since it's intercepted
 * before that frame's target address is ever executed, we have to pop it
 * back off ourselves to fake an RTE back to the caller. */
static unsigned int pop_exception_frame(void) {
    unsigned int sp = m68k_get_reg(NULL, M68K_REG_SP);
    unsigned int sr = m68k_read_memory_16(sp);
    unsigned int return_pc = m68k_read_memory_32(sp + 2);

    m68k_set_reg(M68K_REG_SP, sp + 6);
    m68k_set_reg(M68K_REG_SR, sr);
    m68k_set_reg(M68K_REG_PC, return_pc);

    return return_pc;
}

static void handle_gemdos_trap(void) {
    /* The caller's own pushed args (function number, then parameters)
     * sit just past the 6-byte exception frame Musashi pushed for us. */
    unsigned int sp = m68k_get_reg(NULL, M68K_REG_SP);
    unsigned int func_num = m68k_read_memory_16(sp + 6);
    unsigned int args_addr = sp + 6 + 2;

    pop_exception_frame();

    unsigned int result = gemdos_dispatch(func_num, args_addr);
    m68k_set_reg(M68K_REG_D0, result);
}

// I'm not touching BIOS yet
static void handle_unimplemented_trap(const char *name, unsigned int pc) {
    printf("[tos_layer] Unimplemented %s trap hit at PC 0x%08X\n", name, pc);
    pop_exception_frame();
    m68k_set_reg(M68K_REG_D0, 0);
}

static void trap_instr_hook(unsigned int pc) {
    if (pc == GEMDOS_TRAP_ADDR) {
        handle_gemdos_trap();
    } else if (pc == BIOS_TRAP_ADDR) {
        handle_unimplemented_trap("BIOS", pc);
    } else if (pc == XBIOS_TRAP_ADDR) {
        handle_unimplemented_trap("XBIOS", pc);
    }

    if (s_halt_requested) {
        m68k_end_timeslice();
    }
}

void tos_layer_init(void) {
    install_trap_vector(1, GEMDOS_TRAP_ADDR);
    install_trap_vector(13, BIOS_TRAP_ADDR);
    install_trap_vector(14, XBIOS_TRAP_ADDR);

    m68k_set_instr_hook_callback(trap_instr_hook);
}

void tos_request_halt(void) {
    s_halt_requested = 1;
}
