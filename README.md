# Native Execution of Legacy Atari Applications on Modern Windows

**Course:** COMP4003 MSc Project  
**Author:** Christopher Cortez  
**Supervisor:** Dr. Steven Bagley  

## Project Overview
This project aims to develop a direct compatibility layer that enables legacy Atari TOS (Motorola 68000) applications to execute natively within a modern Windows environment. By mapping legacy TOS API calls (GEMDOS, BIOS, XBIOS) directly to modern Win32 calls, aiming for a seamless execution experience.

## Current State

The core CPU emulation engine (Musashi) is integrated, and the compatibility layer loads and runs cross-compiled GEMDOS executables. Binaries produced by VBCC (`test_programs/hello_vbcc.tos`) runs successfully.

CPU execution runs in cycle chunks rather than a single fixed budget, continuing until the guest program calls `Pterm`/`Pterm0` or a generous safety cap is hit.

## Build Instructions
1. Clone the repository.
2. Open the project in VS Code (requires C/C++ and CMake Tools extensions).
3. Select your compiler kit (e.g., GCC/MinGW or MSVC).
4. Run **CMake: Build** to generate the core and compile the `AtariWin32Layer` executable.
5. Run the executable, passing the path to a file to execute:
   ```
   AtariWin32Layer.exe path\to\program.prg
   ```
   If no path is given, it falls back to the bundled `test_programs/hello.prg`.

## Debug Mode
Pass `--debug` to record a full trace of the run:
```
AtariWin32Layer.exe --debug path\to\program.prg
```
This creates `logs/<program-name>_<timestamp>.log` at the project root, with one line per event: every `TRAP` (with the address that issued it), every GEMDOS call by name with its return value, the Win32 call each one mapped to, and any errors (invalid handles, failed Win32 calls with `GetLastError`, unimplemented functions).