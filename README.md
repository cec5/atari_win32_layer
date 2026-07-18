# Native Execution of Legacy Atari Applications on Modern Windows

**Course:** COMP4003 MSc Project  
**Author:** Christopher Cortez  
**Supervisor:** Dr. Steven Bagley  

## Project Overview
This project aims to develop a direct compatibility layer that enables legacy Atari TOS (Motorola 68000) applications to execute natively within a modern Windows environment. By mapping legacy TOS API calls (GEMDOS, BIOS, XBIOS) directly to modern Win32 calls, aiming for a seamless execution experience.

## Current State

The core CPU emulation engine (Musashi) is integrated, and the compatibility layer loads and runs cross-compiled GEMDOS executables. A binary produced by VBCC (`test_programs/hello_vbcc.tos`) runs successfully end to end, including its C runtime startup sequence.

- **Trap interception** (`src/tos_layer.c`): the exception vector table is populated so that `TRAP #1`/`#13`/`#14` (GEMDOS/BIOS/XBIOS) redirect into reserved dispatch addresses, which Musashi's instruction-hook callback intercepts before they're ever decoded as opcodes. The intercepted trap is resolved by manually restoring the CPU's registers to fake an `RTE` back to the caller.
- **GEMDOS call table** (`src/gemdos/`): a thin dispatcher (`gemdos.c`) routes each function number to a category module, so the call table stays navigable as it grows:
  - `gemdos_console.c` - `Cconws`, routed through the file layer's handle 1 so console output is identical regardless of which call produced it.
  - `gemdos_file.c` - `Fcreate`/`Fopen`/`Fclose`/`Fread`/`Fwrite`/`Fseek`, backed by a real handle table mapped to Win32 file APIs (`CreateFileA`/`ReadFile`/`WriteFile`/`SetFilePointer`), with handles 0/1/2 pre-bound to stdin/stdout/stderr.
  - `gemdos_mem.c` - `Malloc`/`Mfree`/`Mshrink`, backed by a real first-fit free-list allocator (splits on allocation, coalesces adjacent free blocks on free) carved out of the RAM left over above a loaded program's own memory block.
  - `Pterm`/`Pterm0` halt CPU execution; unrecognized functions log as unimplemented and return harmlessly rather than crashing.
- **`.PRG`/`.TOS` loader** (`src/prg_loader.c`): parses the 28-byte GEMDOS header, loads the TEXT/DATA segments and zero-fills BSS, walks the relocation table to fix up absolute addresses for the actual load location, and builds a basepage.
- **`guest_mem_util.c`**: shared helper for reading a NUL-terminated string out of guest memory, used by every call that takes a filename or console string pointer.

## Build Instructions
1. Clone the repository ensuring the Musashi submodule is initialized.
2. Open the project in VS Code (requires C/C++ and CMake Tools extensions).
3. Select your compiler kit (e.g., GCC/MinGW or MSVC).
4. Run **CMake: Build** to generate the core and compile the `AtariWin32Layer` executable.
5. Run the executable, passing the path to a file to execute:
   ```
   AtariWin32Layer.exe path\to\program.prg
   ```
   If no path is given, it falls back to the bundled `test_programs/hello.prg`.