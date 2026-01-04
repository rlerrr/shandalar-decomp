file(GLOB_RECURSE manalinkeh_c_sources
  "${CMAKE_CURRENT_SOURCE_DIR}/cards/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/functions/*.c"
)
file(GLOB_RECURSE manalinkeh_cpp_sources
  "${CMAKE_CURRENT_SOURCE_DIR}/cards/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/functions/*.cpp"
)
file(GLOB_RECURSE manalinkeh_asm_sources
  "${CMAKE_CURRENT_SOURCE_DIR}/cards/*.asm"
  "${CMAKE_CURRENT_SOURCE_DIR}/functions/*.asm"
)

set(manalinkeh_sources
  ${manalinkeh_c_sources}
  ${manalinkeh_cpp_sources}
)
if (SHANDALAR_HAVE_NASM)
  list(APPEND manalinkeh_sources ManalinkEh.asm ${manalinkeh_asm_sources})
else()
  message(WARNING \"ASM_NASM not enabled; ManalinkEh.asm will be skipped.\")
endif()

add_library(manalinkeh SHARED ${manalinkeh_sources})

if (SHANDALAR_HAVE_NASM)
  set_source_files_properties(${manalinkeh_asm_sources} ManalinkEh.asm PROPERTIES LANGUAGE ASM_NASM)
  shandalar_target_nasm_includes(manalinkeh
    "${PROJECT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/cards"
    "${CMAKE_CURRENT_SOURCE_DIR}/functions"
  )
endif()

target_include_directories(manalinkeh PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/cards"
  "${CMAKE_CURRENT_SOURCE_DIR}/functions"
)

target_link_libraries(manalinkeh PRIVATE gdi32 msimg32)

if (TARGET drawcardlib)
  target_link_libraries(manalinkeh PRIVATE drawcardlib)
endif()

if (TARGET capturestackbacktrace)
  target_link_libraries(manalinkeh PRIVATE capturestackbacktrace)
endif()

if (MSVC)
  set_property(TARGET manalinkeh PROPERTY LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/ManalinkEh.def\"")
  set_property(TARGET manalinkeh APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ManalinkEh.def")
endif()

set_target_properties(manalinkeh PROPERTIES
  OUTPUT_NAME "ManalinkEh"
  PREFIX ""
  SUFFIX ".dll"
)

if (MSVC_FOR_DECOMP)
  set_property(TARGET manalinkeh PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

reccmp_add_target(manalinkeh ID MANALINKEH)
