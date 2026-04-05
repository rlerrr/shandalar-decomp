cmake_minimum_required(VERSION 3.20)

project(rpbits LANGUAGES C ASM_MASM)

add_library(rpbits STATIC
    src/rpbits.asm
)

# Tell CMake this is MASM, not GAS
set_source_files_properties(rpbits.asm PROPERTIES
    LANGUAGE ASM_MASM
)
