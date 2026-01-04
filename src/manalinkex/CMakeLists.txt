set(manalinkex_sources
  manalinkex.c
)

add_library(manalinkex SHARED ${manalinkex_sources})

target_include_directories(manalinkex PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

if (MSVC)
  set_property(TARGET manalinkex PROPERTY LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/ManalinkEx.def\"")
  set_property(TARGET manalinkex APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ManalinkEx.def")
endif()

set_target_properties(manalinkex PROPERTIES
  OUTPUT_NAME "ManalinkEx"
  PREFIX ""
  SUFFIX ".dll"
)

if (MSVC_FOR_DECOMP)
  set_property(TARGET manalinkex PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

reccmp_add_target(manalinkex ID MANALINKEX)
