file(GLOB magvid_cpp_sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(magvid_sources
  ${magvid_cpp_sources}
  "${CMAKE_CURRENT_SOURCE_DIR}/../deckdll/src/magsnd.c"
)

add_library(magvid SHARED ${magvid_sources})

target_include_directories(magvid PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/../deckdll/src"
)

target_link_libraries(magvid PRIVATE user32 vfw32 winmm)

if (MSVC)
  target_compile_options(magvid PRIVATE /Od /Zi)
  set_property(TARGET magvid PROPERTY LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/magvid.def\"")
  set_property(TARGET magvid APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/magvid.def")
  target_link_options(magvid PRIVATE /BASE:0x10000000)
endif()

set_target_properties(magvid PROPERTIES
  OUTPUT_NAME "magvid"
  PREFIX ""
  SUFFIX ".dll"
)

if (MSVC_FOR_DECOMP)
  set_property(TARGET magvid PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
endif()

reccmp_add_target(magvid ID MAGVID)
