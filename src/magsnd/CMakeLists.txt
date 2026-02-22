file(GLOB magsnd_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_library(magsnd SHARED ${magsnd_c_sources})

target_include_directories(magsnd PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(magsnd PRIVATE dsound winmm user32 vfw32)

if (MSVC)
  set_property(TARGET magsnd PROPERTY LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/magsnd.def\"")
  set_property(TARGET magsnd APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/magsnd.def")
  target_link_options(magsnd PRIVATE /BASE:0x10000000)
endif()

set_target_properties(magsnd PROPERTIES
  OUTPUT_NAME "magsnd"
  PREFIX ""
  SUFFIX ".dll"
)

if (MSVC_FOR_DECOMP)
  set_property(TARGET magsnd PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
endif()

reccmp_add_target(magsnd ID MAGSND)
