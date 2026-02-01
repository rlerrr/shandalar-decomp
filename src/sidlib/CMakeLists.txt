set(sidlib_sources
  src/pcxw.c
)

add_library(sidlib STATIC ${sidlib_sources})

target_include_directories(sidlib PUBLIC
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

if (MSVC)
  # Match the other decompilation targets (debug-style settings).
  target_compile_options(sidlib PRIVATE /Od /Zi)
  target_compile_definitions(sidlib PRIVATE _DEBUG DEBUG)
endif()

if (MSVC_FOR_DECOMP)
  set_property(TARGET sidlib PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
endif()
