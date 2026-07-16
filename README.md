# Native Execution of Legacy Atari Applications on Modern Windows

**Course:** COMP4003 MSc Project  
**Author:** Christopher Cortez  
**Supervisor:** Dr. Steven Bagley  

## Project Overview
This project aims to develop a direct compatibility layer that enables legacy Atari TOS (Motorola 68000) applications to execute natively within a modern Windows environment. By mapping legacy TOS API calls (GEMDOS, BIOS, XBIOS) directly to modern Win32 calls, this software bypasses the isolation and performance bottlenecks of traditional sandboxed emulators, aiming for a seamless, rootless execution experience.

## Current State
**Phase: Core Development & Baseline Testing (WP3)**

The core CPU emulation engine (Musashi) is integrated, and the compatibility layer can now load and run a real GEMDOS `.PRG` executable:

- **Trap interception** (`src/tos_layer.c`): the exception vector table is populated so that `TRAP #1`/`#13`/`#14` (GEMDOS/BIOS/XBIOS) redirect into reserved dispatch addresses, which Musashi's instruction-hook callback intercepts before they're ever decoded as opcodes. The intercepted trap is resolved by manually restoring the CPU's registers to fake an `RTE` back to the caller.
- **GEMDOS call table** (`src/gemdos.c`): `Cconws` (console write string) is mapped directly to the Win32 console API (`WriteConsoleA`, falling back to `WriteFile` when output is redirected), and `Pterm`/`Pterm0` halts CPU execution. Any other GEMDOS function logs as unimplemented and returns harmlessly. BIOS/XBIOS calls are not yet implemented (stubbed, logged, and no-op'd).
- **`.PRG` loader** (`src/prg_loader.c`): parses the 28-byte GEMDOS header, loads the TEXT/DATA segments and zero-fills BSS, walks the relocation table to fix up absolute addresses for the actual load location, and builds a basepage - so real compiled binaries (not just hand-assembled test code) can be loaded and executed.

A hand-built test binary with a real GEMDOS header and relocation entry lives at `test_programs/hello.prg`, used as the default program if none is specified.

## Next Steps
- Implement the most common BIOS calls (e.g. `Bconout`) alongside GEMDOS, since real compiled programs tend to use both.
- Expand the GEMDOS call table beyond console I/O (file operations, memory allocation, etc.) as test binaries demand them.
- Test the loader against an actual cross-compiled `.PRG` (e.g. via VBCC or a GCC m68k-atari-mint target) rather than only hand-assembled fixtures.

## Build Instructions
1. Clone the repository ensuring the Musashi submodule is initialized.
2. Open the project in VS Code (requires C/C++ and CMake Tools extensions).
3. Select your compiler kit (e.g., GCC/MinGW or MSVC).
4. Run **CMake: Build** to generate the core and compile the `AtariWin32Layer` executable.
5. Run the executable, passing the path to a `.PRG` file to execute:
   ```
   AtariWin32Layer.exe path\to\program.prg
   ```
   If no path is given, it falls back to the bundled `test_programs/hello.prg`.