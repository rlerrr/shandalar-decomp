file(GLOB drawcardlib_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
file(GLOB drawcardlib_asm_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.asm")

set(drawcardlib_sources ${drawcardlib_c_sources})
if (SHANDALAR_HAVE_NASM)
  list(APPEND drawcardlib_sources ${drawcardlib_asm_sources})
endif()

add_library(drawcardlib SHARED ${drawcardlib_sources})

if (SHANDALAR_HAVE_NASM)
  set_source_files_properties(${drawcardlib_asm_sources} PROPERTIES LANGUAGE ASM_NASM)
  shandalar_target_nasm_includes(drawcardlib
    "${PROJECT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}"
  )
endif()

target_include_directories(drawcardlib PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(drawcardlib PRIVATE gdi32 gdiplus shlwapi msimg32)

if (TARGET cardartlib)
  target_link_libraries(drawcardlib PRIVATE cardartlib)
endif()

if (TARGET image)
  target_link_libraries(drawcardlib PRIVATE image)
endif()

if (MSVC)
  set_property(TARGET drawcardlib PROPERTY LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/Drawcardlib.def\"")
  set_property(TARGET drawcardlib APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Drawcardlib.def")
endif()

set_target_properties(drawcardlib PROPERTIES
  OUTPUT_NAME "Drawcardlib"
  PREFIX ""
  SUFFIX ".dll"
)

if (MSVC_FOR_DECOMP)
  set_property(TARGET drawcardlib PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

reccmp_add_target(drawcardlib ID DRAWCARDLIB)
