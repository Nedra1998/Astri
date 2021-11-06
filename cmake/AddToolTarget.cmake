# Add a utility target for executing a program on a set of sources.
#
# @arg NAME The name of the new target ('Fix' is appended for the fix target if
#   that has been enabled).
# @flag FIX Enables a 'Fix' mode target, intended to be used in conjunction
#   with `FIX_ARGS` to apply automated fixes.
# @flag TEST Enables a CTest test for running the utility.
# @flag STATIC_ANALYSER Declares that this tool is a static analyser and may be
#   disabled by the global flag `CITITAS_STATIC_ANALYSIS`.
# @flag FORMATTER Declares that this tool is a formatter and may be disabled by
#   the global flag `CITITAS_FORMATTING`.
# @param EXECUTABLE The executable that will be run.
# @param COMMENT A custom comment to add for the target (defaults to 'Executing
#   automated <EXECUTABLE> checks').
# @param FIX_COMMENT A custom comment to add for the 'Fix' mode target
#   (defaults to the value of `COMMENT`).
# @group ARGS Arguments to pass to the executable.
# @group TEST_ARGS Additional arguments to pass to the executable when
#   executing the CTest test.
# @group FIX_ARGS Additional arguments to pass to the executable when executing
#   the 'Fix' mode target.
# @group EXTRA_ARGS Additional arguments to pass to the executable when running
#   normallly (not in a test or in 'Fix' mode).
# @group SOURCES A list of sources to pass to the program.
# @group DEPENDS A list of CMake targets or sources to mark as depends for the
#   custom targets.
function(add_tool_target NAME)
  set(options FIX TEST STATIC_ANALYSER FORMATTER)
  set(single_value_args EXECUTABLE COMMENT FIX_COMMENT)
  set(multi_value_args
      ARGS
      END_ARGS
      TEST_ARGS
      FIX_ARGS
      EXTRA_ARGS
      SOURCES
      DEPENDS)
  cmake_parse_arguments("" "${options}" "${single_value_args}"
                        "${multi_value_args}" ${ARGN})

  if(NOT _EXECUTABLE OR NOT EXISTS "${_EXECUTABLE}")
    message(SEND_ERROR "add_tool_target requires a valid executable")
    return()
  endif()
  if(NOT ((_STATIC_ANALYSER AND CIVITAS_LINTERS) OR (_FORMATTER
                                                     AND CIVITAS_FORMATTERS)))
    return()
  endif()

  if(_TEST_ARGS)
    set(_TEST ON)
  endif()
  if(_FIX_ARGS)
    set(_FIX ON)
  endif()
  if(NOT _COMMENT)
    set(_COMMENT "Executing automated ${_EXECUTABLE} checks")
  endif()
  if(NOT _FIX_COMMENT)
    set(_FIX_COMMENT "${_COMMENT}")
  endif()

  add_custom_target(
    ${NAME}
    COMMAND "${_EXECUTABLE}" ${_ARGS} ${_EXTRA_ARGS} ${_SOURCES} ${_END_ARGS}
    COMMENT "${_COMMENT}"
    DEPENDS ${_DEPENDS} ${_SOURCES})
  if(_FIX)
    add_custom_target(
      ${NAME}Fix
      COMMAND "${_EXECUTABLE}" ${_ARGS} ${_FIX_ARGS} ${_SOURCES} ${_END_ARGS}
      COMMENT "${_FIX_COMMENT}"
      DEPENDS ${_DEPENDS} ${_SOURCES})
  endif()
  if(_TEST)
    add_test(${NAME} "${_EXECUTABLE}" ${_ARGS} ${_TEST_ARGS} ${_SOURCES}
             ${_END_ARGS})
  endif()
endfunction()

set(_cppcheck_type STATIC_ANALYSER)
set(_clangtidy_type STATIC_ANALYSER)
set(_cpplint_type STATIC_ANALYSER)
set(_iwyu_type STATIC_ANALYSER)
set(_cmakelint_type STATIC_ANALYSER)
set(_clangformat_type FORMATTER)
set(_cmakeformat_type FORMATTER)

set(_cxx_tools Cppcheck ClangTidy Iwyu ClangFormat)
set(_cmake_tools CmakeLint CmakeFormat)

# Adds utility targets for all available static analysers and formatters.
#
# @flag NO_STATIC_ANALYSIS Disables all static analyser targets from being
#   created.
# @flag NO_FORMATTING Disables all formatting targets from being created.
# @param TARGET A CMake target to extract the sources from for running the
#   utility on.
# @param NAME A name for the generated targets (will default to the value of
#   `TARGET` if it is available).
# @group SOURCES A list of sources to pass to the program.
# @group DEPENDS A list of CMake targets or sources to mark as depends for the
#   custom targets.
# @group <TOOL> Additional fields to control the generated targets for the tool.
#   `<TOOL>` can be any one of `CPPCHECK`, `CLANGTIDY`, `CPPLINT`, `IWYU`,
#   `CMAKELINT`, `CLANGFORMAT` or `CMAKEFORMAT`.
#
#   @flag TEST Enables a CTest test for running the utility.
#   @param COMMENT A custom comment to add for the target (defaults to 'Executing
#     automated <EXECUTABLE> checks').
#   @param FIX_COMMENT A custom comment to add for the 'Fix' mode target
#     (defaults to the value of `COMMENT`).
#   @group ARGS Arguments to pass to the executable.
#   @group TEST_ARGS Additional arguments to pass to the executable when
#     executing the CTest test.
#   @group FIX_ARGS Additional arguments to pass to the executable when executing
#     the 'Fix' mode target.
#   @group EXTRA_ARGS Additional arguments to pass to the executable when running
#     normallly (not in a test or in 'Fix' mode).
function(add_tooling_targets)
  set(options TEST NO_STATIC_ANALYSIS NO_FORMATTING)
  set(single_value_args TARGET NAME)
  set(multi_value_args SOURCES DEPENDS)

  foreach(tool in ${_cxx_tools} ${_cmake_tools})
    string(TOUPPER "${tool}" upper)
    list(APPEND multi_value_args "${upper}")
  endforeach()
  cmake_parse_arguments("" "${options}" "${single_value_args}"
                        "${multi_value_args}" ${ARGN})

  if(NOT _TARGET AND NOT _NAME)
    message(SEND_ERROR "add_tooling_targets requires either a target or a name")
    return()
  elseif(NOT _TARGET AND NOT _SOURCES)
    message(
      SEND_ERROR
        "add_tooling_targets requires either a target or a list of source files"
    )
    return()
  endif()
  if(NOT _NAME)
    set(_NAME "${_TARGET}")
  endif()
  if(_TARGET AND TARGET ${_TARGET})
    get_target_property(sources ${_TARGET} SOURCES)
    list(APPEND _SOURCES ${sources})
  endif()

  list(GET _SOURCES 0 first_source)
  if("${first_source}" MATCHES ".*\\.(c|cc|cpp|cxx|c\\+\\+|h|h|hpp|h\\+\\+)$")
    set(lang "cxx")
  elseif("${first_source}" MATCHES ".*(\\.cmake|CMakeLists\\.txt)$")
    set(lang "cmake")
  elseif("${first_source}" MATCHES ".*\\.(sh|bash|zsh)$")
    set(lang "sh")
  elseif("${first_source}" MATCHES ".*\\.(py)$")
    set(lang "python")
  endif()

  if(NOT lang)
    message(
      SEND_ERROR "add_tooling_targets failed to recognize language of sources")
    return()
  endif()

  foreach(tool ${_${lang}_tools})
    string(TOLOWER "${tool}" lower)
    string(TOUPPER "${tool}" upper)
    if(NOT ${tool}_FOUND)
      continue()
    endif()
    if(_TEST)
      set(pargs "TEST")
    else()
      set(pargs)
    endif()
    set(exec "${${upper}_EXECUTABLE}")
    if("${tool}" STREQUAL "Iwyu")
      set(exec "${IWYU_TOOL_EXECUTABLE}")
    endif()
    add_tool_target(
      ${_NAME}${tool} ${_${lower}_type}
      EXECUTABLE "${exec}" ${pargs} ${_${upper}}
      SOURCES ${_${upper}_SOURCES} ${_SOURCES}
      DEPENDS ${_${upper}_DEPENDS} ${_DEPENDS})
  endforeach()

endfunction()
