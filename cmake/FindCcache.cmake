include(FindPackageHandleStandardArgs)

find_program(CCACHE_EXECUTABLE NAMES sccache ccache)

if(CCACHE_EXECUTABLE)
  execute_process(
    COMMAND ${CCACHE_EXECUTABLE} --version
    OUTPUT_VARIABLE CCACHE_VERSION_OUTPUT
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

  string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" CCACHE_VERSION
               "${CCACHE_VERSION_OUTPUT}")
endif()

find_package_handle_standard_args(
  Ccache
  REQUIRED_VARS CCACHE_EXECUTABLE
  VERSION_VAR CCACHE_VERSION)

mark_as_advanced(CCACHE_EXECUTABLE CCACHE_VERSION)
