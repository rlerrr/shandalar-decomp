set(manalinkinterface_sources
  src/main.c  
  manalinkinterface.rc
)

add_library(manalinkinterface SHARED ${manalinkinterface_sources})

target_include_directories(manalinkinterface PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(manalinkinterface PRIVATE user32)

if (MSVC)
  # Force manalinkinterface to build with debug-style settings.
  target_compile_options(manalinkinterface PRIVATE /Od /Zi)

  set_property(TARGET manalinkinterface PROPERTY LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/manalinkinterface.def\"")
  set_property(TARGET manalinkinterface APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/manalinkinterface.def")
endif()

set_target_properties(manalinkinterface PROPERTIES
  OUTPUT_NAME "manalinkinterface"
  PREFIX ""
  SUFFIX ".dll"
)

if (MSVC_FOR_DECOMP)
  set_property(TARGET manalinkinterface PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

reccmp_add_target(manalinkinterface ID MANALINKINTERFACE)
