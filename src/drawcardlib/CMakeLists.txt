set(drawcardlib_sources
  src/main.c
)

add_library(drawcardlib SHARED ${drawcardlib_sources})

target_include_directories(drawcardlib PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(drawcardlib PRIVATE gdi32 gdiplus shlwapi msimg32)

if (TARGET cardartlib)
  target_link_libraries(drawcardlib PRIVATE cardartlib)
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
  set_property(TARGET drawcardlib PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
endif()

reccmp_add_target(drawcardlib ID DRAWCARDLIB)
