#ifndef GEMDOS_H
#define GEMDOS_H

/*
 * Dispatches a GEMDOS (TRAP #1) call.
 *
 * func_num  - the GEMDOS function number, as pushed by the caller.
 * args_addr - address of the first parameter on the stack, immediately
 *             following the function number word.
 *
 * Returns the value the caller expects back in D0.L.
 */
unsigned int gemdos_dispatch(unsigned int func_num, unsigned int args_addr);

#endif
