file(GLOB facemaker_c_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

set(cardartlib_c_sources 
  "${CMAKE_CURRENT_SOURCE_DIR}/../cardartlib/src/assert.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/../cardartlib/src/catalog.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/../cardartlib/src/palette.c"
)

set(drawcardlib_c_sources 
  "${CMAKE_CURRENT_SOURCE_DIR}/../drawcardlib/src/pic.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/../drawcardlib/src/pcxw.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/../drawcardlib/src/drawing.c" 
)

set(facemaker_sources 
  ${facemaker_c_sources}
  ${drawcardlib_c_sources}
  ${cardartlib_c_sources}
)

add_executable(facemaker ${facemaker_sources})

target_include_directories(facemaker PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(facemaker PRIVATE comdlg32 gdi32 user32)

if (TARGET rpbits)
  target_link_libraries(facemaker PRIVATE rpbits)
endif()

if (MSVC)
  target_link_options(facemaker PRIVATE "/SUBSYSTEM:WINDOWS")
  target_compile_options(facemaker PRIVATE /Od /Zi)
endif()

if (MSVC_FOR_DECOMP)
  set_property(TARGET facemaker PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
endif()

reccmp_add_target(facemaker ID FACEMAKER)
