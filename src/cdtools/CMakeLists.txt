set(cdtools_sources
  src/main.c
)

add_library(cdtools SHARED ${cdtools_sources})

target_include_directories(cdtools PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(cdtools PRIVATE advapi32 user32)

if (MSVC)
  # Force cdtools to build with debug-style settings.
  target_compile_options(cdtools PRIVATE /Od /Zi)
  target_compile_definitions(cdtools PRIVATE _DEBUG DEBUG)

  set_property(TARGET cdtools PROPERTY LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/cdtools.def\"")
  set_property(TARGET cdtools APPEND_STRING PROPERTY LINK_FLAGS " /DEBUG")
  set_property(TARGET cdtools APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/cdtools.def")
endif()

set_target_properties(cdtools PROPERTIES
  OUTPUT_NAME "cdtools"
  PREFIX ""
  SUFFIX ".dll"
)

if (MSVC_FOR_DECOMP)
  set_property(TARGET cdtools PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
endif()

reccmp_add_target(cdtools ID cdtools)
