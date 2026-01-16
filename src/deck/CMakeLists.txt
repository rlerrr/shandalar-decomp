file(GLOB deck_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

set(deck_sources ${deck_c_sources})

add_executable(deck ${deck_sources})

target_include_directories(deck PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(deck PRIVATE gdi32 user32 deckdll)

set_property(TARGET deckdll PROPERTY IMPORTED_LOCATION
    "${CMAKE_CURRENT_SOURCE_DIR}/src/deckdll/deckdll.dll"
)
set_property(TARGET deckdll PROPERTY IMPORTED_IMPLIB
    "${CMAKE_CURRENT_SOURCE_DIR}/src/deckdll/deckdll.lib"
)

if (MSVC_FOR_DECOMP)
  set_property(TARGET deck PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

reccmp_add_target(deck ID DECK)
