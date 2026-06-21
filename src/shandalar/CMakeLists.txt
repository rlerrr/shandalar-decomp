file(GLOB shandalar_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

set(magic_c_sources 
  "${CMAKE_CURRENT_SOURCE_DIR}/../magic/src/global_strings.c"
)
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
  "${CMAKE_CURRENT_SOURCE_DIR}/../deckdll/src/shared_resources.c"
)

file(GLOB facemaker_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/../facemaker/src/*.c")
list(FILTER facemaker_c_sources EXCLUDE REGEX ".*main\\.c$")

set(shandalar_sources
  ${magic_c_sources}
  ${shandalar_c_sources}
  ${drawcardlib_c_sources}
  ${cardartlib_c_sources}
  ${deckdll_c_sources}
  ${facemaker_c_sources}
  shandalar.rc
)

add_executable(shandalar ${shandalar_sources})

target_include_directories(shandalar PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(shandalar PRIVATE gdi32 user32 winmm)

if (TARGET cardartlib)
  target_link_libraries(shandalar PRIVATE cardartlib)
endif()

if (TARGET drawcardlib)
  target_link_libraries(shandalar PRIVATE drawcardlib)
endif()

if (TARGET deckdll)
  target_link_libraries(shandalar PRIVATE deckdll)
endif()

if (TARGET rpbits)
  target_link_libraries(shandalar PRIVATE rpbits)
endif()

if (MSVC)
  target_link_options(shandalar PRIVATE "/SUBSYSTEM:WINDOWS")
  target_compile_options(shandalar PRIVATE  /Od /Ob1 /Zi /DSHANDALAR)
endif()

if (MSVC_FOR_DECOMP)
  set_property(TARGET shandalar PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
endif()

reccmp_add_target(shandalar ID SHANDALAR)
