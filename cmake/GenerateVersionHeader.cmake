if(__generate_version_header)
  return()
endif()
set(__generate_version_header YES)

find_package(Git QUIET)
set(__version_header_template
    "${CMAKE_CURRENT_LIST_DIR}/GenerateVersionHeader.hpp.in")

# Generates a C++ version header for the project.
#
# @param TEMPLATE Template file to use for generating the version header
# @param OUTPUT Destination file path for the version header
function(generate_version_header)
  set(options)
  set(single_value_args TEMPLATE OUTPUT)
  set(multi_value_args)
  cmake_parse_arguments("" "${options}" "${single_value_args}"
                        "${multi_value_args}" ${ARGN})

  if(NOT _TEMPLATE OR NOT EXISTS "${_TEMPLATE}")
    set(_TEMPLATE "${__version_header_template}")
  endif()
  if(NOT _OUTPUT)
    set(_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/include/version.hpp")
  endif()

  string(MAKE_C_IDENTIFIER ${PROJECT_NAME} _id)
  string(TOUPPER ${_id} PROJECT_UPPER)
  string(TOLOWER ${_id} PROJECT_LOWER)

  if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    include(GetGitRevisionDescription)
    get_git_head_revision(_refspec _hashvar)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
      OUTPUT_VARIABLE _commit_sha
      RESULT_VARIABLE _result
      OUTPUT_STRIP_TRAILING_WHITESPACE)

    set(PROJECT_VERSION_BUILD
        "${_commit_sha}"
        CACHE STRING "git short sha" FORCE)
  endif()

  if(NOT EXISTS "${_TEMPLATE}")
    message(
      FATAL_ERROR
        "GenerateVersionHeader.cmake: Version template file ${_TEMPLATE} was not found"
    )
  else()
    configure_file("${_TEMPLATE}" "${_OUTPUT}" @ONLY)
  endif()
endfunction()
