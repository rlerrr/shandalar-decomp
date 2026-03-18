file(GLOB magic_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB cards_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/../cards/*.c")

set(magic_sources ${cards_c_sources} ${magic_c_sources} magic.rc)

add_executable(magic ${magic_sources})

target_include_directories(magic PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(magic PRIVATE gdi32 user32)

if (TARGET deckdll)
  target_link_libraries(magic PRIVATE deckdll)
endif()

if (TARGET manalinkinterface)
  target_link_libraries(magic PRIVATE manalinkinterface)
endif()

if (MSVC)
  target_link_options(magic PRIVATE "/SUBSYSTEM:WINDOWS")
  target_compile_options(magic PRIVATE /Od /Zi)
endif()

if (MSVC_FOR_DECOMP)
  set_property(TARGET magic PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
endif()

reccmp_add_target(magic ID MAGIC)
