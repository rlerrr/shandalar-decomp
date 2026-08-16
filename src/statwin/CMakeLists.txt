file(GLOB statwin_cpp_sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(statwin_sources
  ${statwin_cpp_sources}
  "${CMAKE_CURRENT_SOURCE_DIR}/../deckdll/src/magsnd.c"
)

add_library(statwin SHARED ${statwin_sources})

target_include_directories(statwin PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/../deckdll/src"
)

target_link_libraries(statwin PRIVATE user32 gdi32 vfw32)

if (MSVC)
  target_compile_options(statwin PRIVATE /Od /Zi)
  set_property(TARGET statwin PROPERTY LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/statwin.def\"")
  set_property(TARGET statwin APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/statwin.def")
  target_link_options(statwin PRIVATE /BASE:0x10000000)
endif()

set_target_properties(statwin PROPERTIES
  OUTPUT_NAME "StatWin"
  PREFIX ""
  SUFFIX ".dll"
)

if (MSVC_FOR_DECOMP)
  set_property(TARGET statwin PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
endif()

reccmp_add_target(statwin ID STATWIN)
