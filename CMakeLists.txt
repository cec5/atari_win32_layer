cmake_minimum_required(VERSION 3.10)

project(AtariWin32Layer C)
set(MUSASHI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/musashi)
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/musashi_gen)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

file(MAKE_DIRECTORY ${GEN_DIR})

# Compile the m68kmake utility from source
add_executable(m68kmake ${MUSASHI_DIR}/m68kmake.c)

# Tell CMake to run the m68kmake utility to generate the emulator logic
# By default, m68kmake expects the output directory and the input instruction table
add_custom_command(
    OUTPUT ${GEN_DIR}/m68kops.c ${GEN_DIR}/m68kops.h
    COMMAND m68kmake ${GEN_DIR} ${MUSASHI_DIR}/m68k_in.c
    DEPENDS m68kmake ${MUSASHI_DIR}/m68k_in.c
    COMMENT "Generating Musashi CPU core files..."
)

# Complilation
add_executable(${PROJECT_NAME}
    src/main.c
    src/memory.c
    src/tos_layer.c
    src/gemdos/gemdos.c
    src/gemdos/gemdos_console.c
    src/gemdos/gemdos_mem.c
    src/gemdos/gemdos_file.c
    src/prg_loader.c
    src/guest_mem_util.c
    src/logger.c
    src/host_paths.c
    ${MUSASHI_DIR}/m68kcpu.c
    ${MUSASHI_DIR}/m68kdasm.c
    ${MUSASHI_DIR}/softfloat/softfloat.c
    ${GEN_DIR}/m68kops.c
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${MUSASHI_DIR}
    ${GEN_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)