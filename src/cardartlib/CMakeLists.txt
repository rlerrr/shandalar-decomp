set(cardartlib_sources
  src/main.cpp
)

add_library(cardartlib SHARED ${cardartlib_sources})

target_include_directories(cardartlib PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(cardartlib PRIVATE gdiplus user32)

if (SHANDALAR_USE_BOOST)
  find_package(Boost COMPONENTS filesystem system)
  if (Boost_FOUND)
    target_include_directories(cardartlib PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(cardartlib PRIVATE Boost::filesystem Boost::system)
  else()
    message(WARNING "Boost filesystem/system not found; CardArtLib may fail to link.")
  endif()
endif()

if (MSVC)
  set_property(TARGET cardartlib PROPERTY LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/CardArtLib.def\"")
  set_property(TARGET cardartlib APPEND PROPERTY LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/CardArtLib.def")
endif()

set_target_properties(cardartlib PROPERTIES
  OUTPUT_NAME "CardArtLib"
  PREFIX ""
  SUFFIX ".dll"
)

if (MSVC_FOR_DECOMP)
  set_property(TARGET cardartlib PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

reccmp_add_target(cardartlib ID CARDARTLIB)
