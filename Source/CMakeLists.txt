add_subdirectory(Common)
add_subdirectory(Core)

if (ENABLE_QT)
  add_subdirectory(ApeQt)
endif()

if (ENABLE_CLI)
  add_subdirectory(ApeCLI)
endif()

if (ENABLE_TOOLS)
  add_subdirectory(Tools)
endif()


if (ENABLE_TESTS)
  add_subdirectory(Tests)
endif()
