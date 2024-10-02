cmake_minimum_required(VERSION 3.22)

# Block in-source builds
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# ---- Project ----
project(
  Astri
  VERSION 0.1.0
  DESCRIPTION "This is a test"
  LANGUAGES CXX)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

# ---- Global Configuration ----
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GetGitRevisionDescription)
git_describe(PROJECT_VERSION_COMMIT)

# ---- Options ----
option(ENABLE_CCACHE "Enable CCache" ON)

option(ASTRI_BUILD_TESTS "Enable building the tests" ON)
option(ASTRI_BUILD_APP "Enable building the application" ON)

option(ASTRI_ENABLE_PROFILING "Enable profiling" OFF)

# ---- Dependencies ----
include(CPM)

if(ENABLE_CCACHE)
  find_package(Ccache)
  if(CCACHE_FOUND)
    set(CMAKE_CXX_COMPILER_LAUNCHER_VERSION "${CCACHE_VERSION}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_EXECUTABLE}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_EXECUTABLE}")
  else()
    message(WARNING "No CCache implementation found")
  endif()
endif()

# ---- Subdirectories ----
add_subdirectory(lib)

if(ASTRI_BUILD_APP)
  add_subdirectory(app)
endif()

if(ASTRI_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# ---- Packaging ----
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Arden")
set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_GENERATOR "ZIP;TGZ")
set(CPACK_SOURCE_GENERATOR "ZIP;TGZ")
set(CPACK_SOURCE_IGNORE_FILES "/build/;/\\\\.git/")

set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME_LOWER}-${PROJECT_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME_LOWER}-${PROJECT_VERSION}-source")

include(CPack)

# ---- Summary ----

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  # cmake-format: off
  message(STATUS "====================[ ${PROJECT_NAME} v${PROJECT_VERSION} (${PROJECT_VERSION_COMMIT}) ]====================")
  message(STATUS "")
  message(STATUS "System:      ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
  message(STATUS "CMake:       ${CMAKE_VERSION}")
  message(STATUS "Generator:   ${CMAKE_GENERATOR}")
  if(CCACHE_FOUND)
    message(STATUS "Launcher:    ${CCACHE_EXECUTABLE} v${CCACHE_VERSION}")
  else()
    message(STATUS "Launcher:    ")
  endif()
  message(STATUS "Compiler:    ${CMAKE_CXX_COMPILER} (${CMAKE_CXX_COMPILER_ID}) v${CMAKE_CXX_COMPILER_VERSION}")
  message(STATUS "")
  message(STATUS "ASTRI_BUILD_TESTS: ${ASTRI_BUILD_TESTS}")
  message(STATUS "ASTRI_BUILD_APP:   ${ASTRI_BUILD_APP}")
  message(STATUS "")
  message(STATUS "ASTRI_ENABLE_PROFILING: ${ASTRI_ENABLE_PROFILING}")
  message(STATUS "")
  message(STATUS "================================================================================")
  # cmake-format: on
endif()
