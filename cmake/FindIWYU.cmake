include(FindExecutablePackage)
FindExecutablePackage(IWYU "[0-9]+\\\\.[0-9]+" iwyu include-what-you-use)

if(IWYU_FOUND)
  find_program(
    IWYU_TOOL_EXECUTABLE
    NAMES iwyu-tool
    PATHS "${IWYU_ROOT_DIR}")
  if(NOT IWYU_TOOL_EXECUTABLE)
    message(WARNING "Failed to find iwyu-tool")
  endif()
endif()
