set(SHANDALAR_SRC_ROOT "${CMAKE_CURRENT_LIST_DIR}")

function(shandalar_add_def_import_library name def_file)
  if(MSVC)
    set(out_lib "${CMAKE_CURRENT_BINARY_DIR}/${name}.lib")
    add_custom_command(
      OUTPUT "${out_lib}"
      COMMAND lib /nologo /def:"${def_file}" /machine:x86 /out:"${out_lib}"
      DEPENDS "${def_file}"
      VERBATIM
    )
    add_custom_target(${name}_importlib DEPENDS "${out_lib}")
    add_library(${name} STATIC IMPORTED GLOBAL)
    set_target_properties(${name} PROPERTIES IMPORTED_LOCATION "${out_lib}")
    add_dependencies(${name} ${name}_importlib)
  else()
    add_library(${name} INTERFACE)
  endif()
endfunction()

if (SHANDALAR_BUILD_CARDARTLIB)
  add_subdirectory(cardartlib)
endif()

if (SHANDALAR_BUILD_CDTOOLS)
  add_subdirectory(cdtools)
endif()

if (SHANDALAR_BUILD_DRAWCARDLIB)
  add_subdirectory(drawcardlib)
endif()

if (SHANDALAR_BUILD_DECKDLL)
  add_subdirectory(deckdll)
endif()

if (SHANDALAR_BUILD_DECK)
  add_subdirectory(deck)
endif()

if (SHANDALAR_BUILD_MANALINKINTERFACE)
  add_subdirectory(manalinkinterface)
endif()
