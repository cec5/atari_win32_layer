#ifndef TOS_LAYER_H
#define TOS_LAYER_H

/*
 * Reserved addresses the fake trap handlers live at. These sit right after
 * the exception vector table (which occupies 0x000-0x3FF) and are never
 * actually fetched/decoded as opcodes, the instruction hook callback
 * intercepts execution the moment PC lands here, before Musashi reads
 * whatever bytes happen to be at these addresses.
 */

#define GEMDOS_TRAP_ADDR 0x00000400u
#define BIOS_TRAP_ADDR   0x00000404u
#define XBIOS_TRAP_ADDR  0x00000408u

/*
 * Same trick, applied to the CPU's own fault vectors instead of TRAP.
 * These aren't OS calls to service - if a guest program hits one, it's a
 * genuine CPU-level fault (buggy compiled code, a corrupted segment, a
 * bug in our own loader), so the handler just logs which fault it was
 * and halts, rather than pretending it can resume something a real 68000
 * couldn't resume either. Each fault needs its own address (not one
 * shared address like GEMDOS/BIOS/XBIOS) because the plain 68000
 * exception stack frame doesn't record which vector fired - the only way
 * to know is which reserved address the CPU landed on.
 */
#define FAULT_BUS_ERROR_ADDR      0x0000040Cu
#define FAULT_ADDRESS_ERROR_ADDR  0x00000410u
#define FAULT_ILLEGAL_INSTR_ADDR  0x00000414u
#define FAULT_ZERO_DIVIDE_ADDR    0x00000418u
#define FAULT_CHK_ADDR            0x0000041Cu
#define FAULT_TRAPV_ADDR          0x00000420u
#define FAULT_PRIVILEGE_ADDR      0x00000424u
#define FAULT_LINE_A_ADDR         0x00000428u
#define FAULT_LINE_F_ADDR         0x0000042Cu

void tos_layer_init(void);

void tos_request_halt(void);

// True once Pterm/Pterm0 has requested a halt. Lets main() run in chunks
// until the guest program actually terminates itself, instead of
// guessing a fixed cycle budget upfront.
int tos_is_halted(void);

#endif