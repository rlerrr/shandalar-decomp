set(cardartlib_sources
  src/main.c
)

add_library(cardartlib SHARED ${cardartlib_sources})

target_include_directories(cardartlib PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(cardartlib PRIVATE gdi32 user32)

if (MSVC)
  # Force CardArtLib to build with debug-style settings.
  target_compile_options(cardartlib PRIVATE /Od /Zi)
  target_compile_definitions(cardartlib PRIVATE _DEBUG DEBUG)

  set_property(TARGET cardartlib PROPERTY LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/CardArtLib.def\"")
  set_property(TARGET cardartlib APPEND_STRING PROPERTY LINK_FLAGS " /DEBUG")
  set_property(TARGET cardartlib APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/CardArtLib.def")
endif()

set_target_properties(cardartlib PROPERTIES
  OUTPUT_NAME "CardArtLib"
  PREFIX ""
  SUFFIX ".dll"
)

if (MSVC_FOR_DECOMP)
  set_property(TARGET cardartlib PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
endif()

reccmp_add_target(cardartlib ID CARDARTLIB)
