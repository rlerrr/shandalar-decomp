set(SHANDALAR_SRC_ROOT "${CMAKE_CURRENT_LIST_DIR}")

add_subdirectory(rpbits)

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

if (SHANDALAR_BUILD_FACEMAKER)
  add_subdirectory(facemaker)
endif()

if (SHANDALAR_BUILD_MANALINKINTERFACE)
  add_subdirectory(manalinkinterface)
endif()

if (SHANDALAR_BUILD_MAGIC)
  add_subdirectory(magic)
endif()

if (SHANDALAR_BUILD_MAGSND)
  add_subdirectory(magsnd)
endif()

if (SHANDALAR_BUILD_STATWIN)
  add_subdirectory(statwin)
endif()

if (SHANDALAR_BUILD_SHANDALAR)
  add_subdirectory(shandalar)
endif()
