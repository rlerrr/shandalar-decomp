file(GLOB shandalar_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB magic_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/../magic/cards/*.c")

set(shandalar_sources
  ${magic_c_sources}
  ${shandalar_c_sources} 
  shandalar.rc
)

add_executable(shandalar ${shandalar_sources})

target_include_directories(shandalar PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(shandalar PRIVATE gdi32 user32)

if (MSVC)
  target_link_options(shandalar PRIVATE "/SUBSYSTEM:WINDOWS")
  target_compile_options(shandalar PRIVATE /Od /Zi)
endif()

if (MSVC_FOR_DECOMP)
  set_property(TARGET shandalar PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
endif()

reccmp_add_target(shandalar ID SHANDALAR)
