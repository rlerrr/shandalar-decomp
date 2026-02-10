cmake_minimum_required(VERSION 3.20)

project(rpbits LANGUAGES C ASM_MASM)

# ------------------------------------------------------------
# Force 32-bit build (important for MASM + old-style asm)
# ------------------------------------------------------------

if (MSVC)
    # Make sure we are targeting x86, not x64
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        message(FATAL_ERROR "This project must be built as Win32 (x86), not x64.")
    endif()
endif()

add_library(rpbits STATIC
    src/rpbits.asm
)

# Tell CMake this is MASM, not GAS
set_source_files_properties(rpbits.asm PROPERTIES
    LANGUAGE ASM_MASM
)
