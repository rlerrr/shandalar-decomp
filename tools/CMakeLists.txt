add_executable(cardsdat2csv
  cardsdat2csv/main.c
)

target_include_directories(cardsdat2csv PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
)

add_executable(carddata_dump
  carddata_dump/main.c
)

target_include_directories(carddata_dump PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
)

if (MSVC)
  # Keep build noise down for a tiny utility.
  target_compile_definitions(cardsdat2csv PRIVATE _CRT_SECURE_NO_WARNINGS)
  target_compile_definitions(carddata_dump PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

set_target_properties(cardsdat2csv PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools/cardsdat2csv"
)

set_target_properties(carddata_dump PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools/carddata_dump"
)
