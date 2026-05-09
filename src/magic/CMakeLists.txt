file(GLOB_RECURSE magic_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
set(drawcardlib_c_sources 
  "${CMAKE_CURRENT_SOURCE_DIR}/../drawcardlib/src/pic.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/../drawcardlib/src/pcxw.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/../drawcardlib/src/drawing.c" 
)
set(cardartlib_c_sources
  "${CMAKE_CURRENT_SOURCE_DIR}/../cardartlib/src/assert.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../cardartlib/src/catalog.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../cardartlib/src/palette.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../cardartlib/src/haar.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../cardartlib/src/huffman.c"
)
set(deckdll_c_sources
  "${CMAKE_CURRENT_SOURCE_DIR}/../deckdll/src/buttonclass.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../deckdll/src/card_db.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../deckdll/src/checks.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../deckdll/src/drm.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/../deckdll/src/magsnd.c"
)

set(magic_sources
  ${cards_c_sources}
  ${magic_c_sources}
  ${drawcardlib_c_sources}
  ${cardartlib_c_sources}
  ${deckdll_c_sources}
  magic.rc
)

add_executable(magic ${magic_sources})

target_include_directories(magic PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(magic PRIVATE gdi32 user32)

if (TARGET drawcardlib)
  target_link_libraries(magic PRIVATE drawcardlib)
endif()

if (TARGET deckdll)
  target_link_libraries(magic PRIVATE deckdll)
endif()

if (TARGET manalinkinterface)
  target_link_libraries(magic PRIVATE manalinkinterface)
endif()

if (TARGET rpbits)
  target_link_libraries(magic PRIVATE rpbits)
endif()

if (MSVC)
  target_link_options(magic PRIVATE "/SUBSYSTEM:WINDOWS")
  target_compile_options(magic PRIVATE /Od /Ob1 /Zi)
endif()

if (MSVC_FOR_DECOMP)
  set_property(TARGET magic PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
endif()

reccmp_add_target(magic ID MAGIC)
