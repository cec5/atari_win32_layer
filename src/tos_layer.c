#include <stdio.h>
#include "m68k.h"
#include "tos_layer.h"
#include "gemdos/gemdos.h"
#include "logger.h"

static int s_halt_requested = 0;

#define TRAP_VECTOR_ADDR(n) ((32u + (n)) * 4u)

static void install_trap_vector(unsigned int trap_num, unsigned int handler_addr) {
    m68k_write_memory_32(TRAP_VECTOR_ADDR(trap_num), handler_addr);
}

// CPU fault vectors are addressed directly (vector_num * 4), unlike
// TRAP #n which lives at (32 + n) * 4.
static void install_vector(unsigned int vector_num, unsigned int handler_addr) {
    m68k_write_memory_32(vector_num * 4u, handler_addr);
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
    unsigned int sp = m68k_get_reg(NULL, M68K_REG_SP);
    unsigned int func_num = m68k_read_memory_16(sp + 6);
    unsigned int args_addr = sp + 6 + 2;

    unsigned int return_pc = pop_exception_frame();
    unsigned int call_site = return_pc - 2; // TRAP #n is always a 2-byte opcode
    log_write(LOG_TRAP, "TRAP #1 (GEMDOS) at 0x%08X - function=0x%02X, resuming at 0x%08X",
              call_site, func_num, return_pc);

    unsigned int result = gemdos_dispatch(func_num, args_addr);
    m68k_set_reg(M68K_REG_D0, result);
}

// Shared handler for BIOS/XBIOS traps (neither is implemented yet).
static void handle_unimplemented_trap(const char *name, unsigned int pc) {
    unsigned int sp = m68k_get_reg(NULL, M68K_REG_SP);
    unsigned int func_num = m68k_read_memory_16(sp + 6);

    unsigned int return_pc = pop_exception_frame();
    unsigned int call_site = return_pc - 2;

    printf("[tos_layer] Unimplemented %s trap hit at PC 0x%08X (function=0x%02X)\n", name, pc, func_num);
    log_write(LOG_ERROR, "TRAP (%s) at 0x%08X - function=0x%02X is unimplemented", name, call_site, func_num);

    m68k_set_reg(M68K_REG_D0, 0);
}

// Log the CPU fault (out of our control) and halt.
static void handle_cpu_fault(const char *name) {
    unsigned int fault_pc = pop_exception_frame();

    fprintf(stderr, "[tos_layer] CPU fault: %s at PC=0x%08X - halting\n", name, fault_pc);
    log_write(LOG_ERROR, "CPU FAULT: %s at PC=0x%08X - halting execution", name, fault_pc);

    tos_request_halt();
}

static void trap_instr_hook(unsigned int pc) {
    if (pc == GEMDOS_TRAP_ADDR) {
        handle_gemdos_trap();
    } else if (pc == BIOS_TRAP_ADDR) {
        handle_unimplemented_trap("BIOS", pc);
    } else if (pc == XBIOS_TRAP_ADDR) {
        handle_unimplemented_trap("XBIOS", pc);
    } else if (pc == FAULT_BUS_ERROR_ADDR) {
        handle_cpu_fault("Bus Error");
    } else if (pc == FAULT_ADDRESS_ERROR_ADDR) {
        handle_cpu_fault("Address Error");
    } else if (pc == FAULT_ILLEGAL_INSTR_ADDR) {
        handle_cpu_fault("Illegal Instruction");
    } else if (pc == FAULT_ZERO_DIVIDE_ADDR) {
        handle_cpu_fault("Zero Divide");
    } else if (pc == FAULT_CHK_ADDR) {
        handle_cpu_fault("CHK Instruction");
    } else if (pc == FAULT_TRAPV_ADDR) {
        handle_cpu_fault("TRAPV Instruction");
    } else if (pc == FAULT_PRIVILEGE_ADDR) {
        handle_cpu_fault("Privilege Violation");
    } else if (pc == FAULT_LINE_A_ADDR) {
        handle_cpu_fault("Line 1010 (A-line) Emulator");
    } else if (pc == FAULT_LINE_F_ADDR) {
        handle_cpu_fault("Line 1111 (F-line) Emulator");
    }

    if (s_halt_requested) {
        m68k_end_timeslice();
    }
}

void tos_layer_init(void) {
    install_trap_vector(1, GEMDOS_TRAP_ADDR);
    install_trap_vector(13, BIOS_TRAP_ADDR);
    install_trap_vector(14, XBIOS_TRAP_ADDR);

    install_vector(2, FAULT_BUS_ERROR_ADDR);
    install_vector(3, FAULT_ADDRESS_ERROR_ADDR);
    install_vector(4, FAULT_ILLEGAL_INSTR_ADDR);
    install_vector(5, FAULT_ZERO_DIVIDE_ADDR);
    install_vector(6, FAULT_CHK_ADDR);
    install_vector(7, FAULT_TRAPV_ADDR);
    install_vector(8, FAULT_PRIVILEGE_ADDR);
    install_vector(10, FAULT_LINE_A_ADDR);
    install_vector(11, FAULT_LINE_F_ADDR);

    m68k_set_instr_hook_callback(trap_instr_hook);
}

void tos_request_halt(void) {
    s_halt_requested = 1;
}

int tos_is_halted(void) {
    return s_halt_requested;
}