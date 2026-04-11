file(GLOB drawcardlib_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB cardartlib_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/../cardartlib/src/*.c")
list(FILTER cardartlib_c_sources EXCLUDE REGEX ".*main\\.c$")

set(drawcardlib_sources
  ${drawcardlib_c_sources}
  ${cardartlib_c_sources}
)

add_library(drawcardlib SHARED ${drawcardlib_sources})

target_include_directories(drawcardlib PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(drawcardlib PRIVATE gdi32)

if (TARGET cardartlib)
  target_link_libraries(drawcardlib PRIVATE cardartlib)
endif()

if (TARGET rpbits)
  target_link_libraries(drawcardlib PRIVATE rpbits)
  target_link_options(drawcardlib PRIVATE)
endif()

if (MSVC)
  # Force DrawCardLib to build with debug-style settings.
  target_compile_options(drawcardlib PRIVATE /Od /Ob1 /Zi)
  target_compile_definitions(drawcardlib PRIVATE _DEBUG DEBUG)

  set_property(TARGET drawcardlib PROPERTY LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/Drawcardlib.def\"")
  set_property(TARGET drawcardlib APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Drawcardlib.def")
endif()

set_target_properties(drawcardlib PROPERTIES
  OUTPUT_NAME "Drawcardlib"
  PREFIX ""
  SUFFIX ".dll"
)

if (MSVC_FOR_DECOMP)
  set_property(TARGET drawcardlib PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
endif()

reccmp_add_target(drawcardlib ID DRAWCARDLIB)
