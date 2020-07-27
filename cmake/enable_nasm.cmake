
if(WIN32)

    set(NASM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/nasm/win64/nasm.exe")
    set(NASM_FLAGS "-f win64 -dWIN64=1")

elseif(APPLE)

    set(NASM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin/nasm/mach-o/nasm")
    set(NASM_FLAGS "-f macho64 -dMACHO64=1")

else(UNIX AND NOT APPLE)

    set(NASM_PATH "/usr/bin/nasm")
    set(NASM_FLAGS "-f elf64 -dELF64=1")

endif(WIN32)

if(NOT EXISTS ${NASM_PATH})

    message(" NASM_PATH: ${NASM_PATH}")
    message(FATAL_ERROR " - Cound not find NASM, make sure nasm is installed or NASM_PATH variable is correctly set.")

endif(NOT EXISTS ${NASM_PATH})

set(CMAKE_ASM_NASM_COMPILER ${NASM_PATH} CACHE INTERNAL "" FORCE)
enable_language(ASM_NASM)

set(CMAKE_ASM_NASM_COMPILER_ARG1 "" CACHE INTERNAL "" FORCE)
set(CMAKE_ASM_NASM_FLAGS ${NASM_FLAGS} CACHE INTERNAL "" FORCE)
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS "asm;nasm;s;S")

