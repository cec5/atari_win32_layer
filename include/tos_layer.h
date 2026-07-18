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

void tos_layer_init(void);

void tos_request_halt(void);

#endif
