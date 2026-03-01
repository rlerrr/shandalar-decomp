file(GLOB deckdll_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB sidlib_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/../sidlib/src/*.c")
set(cardartlib_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/../cardartlib/src/assert.c" "${CMAKE_CURRENT_SOURCE_DIR}/../cardartlib/src/palette.c")

set(deckdll_sources
  ${deckdll_c_sources}
  ${sidlib_c_sources}
  ${cardartlib_c_sources}
  deckdll.rc
)

add_library(deckdll SHARED ${deckdll_sources})

target_include_directories(deckdll PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(deckdll PRIVATE comctl32 gdi32)

if (TARGET drawcardlib)
  target_link_libraries(deckdll PRIVATE drawcardlib)
endif()

if (TARGET rpbits)
  target_link_libraries(deckdll PRIVATE rpbits)
  target_link_options(deckdll PRIVATE /BASE:0x101f7000)
endif()

if (MSVC)
  target_compile_options(deckdll PRIVATE /Od /Ob1 /Zi /DDECKDLL)

  set_property(TARGET deckdll PROPERTY LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/DeckDll.def\"")
  set_property(TARGET deckdll APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/DeckDll.def")
endif()

set_target_properties(deckdll PROPERTIES
  OUTPUT_NAME "DeckDll"
  PREFIX ""
  SUFFIX ".dll"
)

if (MSVC_FOR_DECOMP)
  set_property(TARGET deckdll PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
endif()

reccmp_add_target(deckdll ID DECKDLL)
