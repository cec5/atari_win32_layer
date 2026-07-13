# Native Execution of Legacy Atari Applications on Modern Windows

**Course:** COMP4003 MSc Project  
**Author:** Christopher Cortez  
**Supervisor:** Dr. Steven Bagley  

## Project Overview
This project aims to develop a direct compatibility layer that enables legacy Atari TOS (Motorola 68000) applications to execute natively within a modern Windows environment. By mapping legacy TOS API calls (GEMDOS, BIOS, XBIOS) directly to modern Win32 calls, this software bypasses the isolation and performance bottlenecks of traditional sandboxed emulators, aiming for a seamless, rootless execution experience.

## Current State
**Phase: Core Development & Baseline Testing (WP3)**

The project is currently in the foundational bootstrapping phase. The core CPU emulation engine has been integrated and verified. 

## Next Steps
The immediate next goal is to implement the instruction interceptor to catch software interrupts (`TRAP #1`, `#13`, `#14`) and begin routing GEMDOS/BIOS requests from the Atari application to the host's Win32 API translation layer.

## Build Instructions
1. Clone the repository ensuring the Musashi submodule is initialized.
2. Open the project in VS Code (requires C/C++ and CMake Tools extensions).
3. Select your compiler kit (e.g., GCC/MinGW or MSVC).
4. Run **CMake: Build** to generate the core and compile the `AtariWin32Layer` executable.
5. Run the executable located in the `build/` directory.