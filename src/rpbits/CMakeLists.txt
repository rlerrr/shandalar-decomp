cmake_minimum_required(VERSION 3.20)

project(rpbits LANGUAGES C)

if (SHANDALAR_MODERN_FIXES)
  add_library(rpbits STATIC
    src/rpbits.c
  )
else()
  enable_language(ASM_MASM)

  add_library(rpbits STATIC
    src/rpbits.asm
  )

  # Tell CMake this is MASM, not GAS
  set_source_files_properties(src/rpbits.asm PROPERTIES
    LANGUAGE ASM_MASM
  )
endif()
