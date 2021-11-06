include(FindPackageHandleStandardArgs)

# Implements common functionality used for finding most executables
#
# @arg NAME The module name.
# @arg VERSION A regex to match the version string from the response of
#   `<exec> --version`.
# @group ... List of potential executable names to search for.
macro(FindExecutablePackage NAME VERSION)
  string(TOUPPER "${NAME}" name)
  file(TO_CMAKE_PATH "${${name}_ROOT_DIR}" ${name}_ROOT_DIR)
  set(${name}_ROOT_DIR
      "${${name}_ROOT_DIR}"
      CACHE PATH "Path to search for ${NAME}")
  set(_old_app_bundle_settings ${CMAKE_FIND_APPBUNDLE})
  set(CMAKE_FIND_APPBUNDLE NEVER)
  if(${name}_EXECUTABLE AND NOT EXISTS "${${name}_EXECUTABLE}")
    set(${name}_EXECUTABLE
        "notfound"
        CACHE PATH "" FORCE)
  endif()

  if(${name}_ROOT_DIR)
    find_program(
      ${name}_EXECUTABLE
      NAMES ${ARGN}
      PATHS "${${name}_ROOT_DIR}"
      NO_DEFAULT_PATH)
  else()
    find_program(${name}_EXECUTABLE NAMES ${ARGN})
  endif()

  set(CMAKE_FIND_APPBUNDLE ${_old_app_bundle_settings})

  execute_process(
    COMMAND "${${name}_EXECUTABLE}" --version
    OUTPUT_VARIABLE ${name}_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(REGEX MATCH "${VERSION}" ${name}_VERSION "${${name}_VERSION}")
  find_package_handle_standard_args(
    ${NAME}
    REQUIRED_VARS ${name}_EXECUTABLE
    VERSION_VAR ${name}_VERSION)

  if(${name}_FOUND OR ${name}_MARK_AS_ADVANCED)
    mark_as_advanced(${name}_ROOT_DIR)
  endif()
  mark_as_advanced(${name}_EXECUTABLE)
endmacro()
