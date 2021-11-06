# PROJECT {{{
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(
  Astri
  VERSION 0.1.0
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake"
     "${PROJECT_SOURCE_DIR}/cmake/find")

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE
      "Debug"
      CACHE STRING "CMake build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

# }}}
# OPTIONS {{{

include(CMakeDependentOption)
include(CheckIPOSupported)

find_package(OpenGL)

find_package(CCache QUIET)
check_ipo_supported(RESULT IPO_SUPPORTED LANGUAGES C CXX)
cmake_dependent_option(ASTRI_IPO "Enable interprocedural optimization" ON
                       "IPO_SUPPORTED;CMAKE_BUILD_TYPE MATCHES Release" OFF)
cmake_dependent_option(ASTRI_CCACHE "Enable C/CXX compiler cache" ON
                       "CCACHE_FOUND" OFF)

find_package(Cppcheck QUIET)
find_package(ClangTidy QUIET)
find_package(Cpplint QUIET)
find_package(IWYU QUIET)
find_package(CmakeLint QUIET)
cmake_dependent_option(
  ASTRI_LINTERS
  "Enable automated static analysis tools"
  ON
  "CPPCHECK_FOUND OR CLANGTIDY_FOUND OR CPPLINT_FOUND OR IWYU_FOUND OR CMAKELINT_FOUND"
  OFF)

find_package(ClangFormat QUIET)
find_package(CmakeFormat QUIET)
cmake_dependent_option(ASTRI_FORMATTERS "Enable automated formatting tools" ON
                       "CLANGFORMAT_FOUND OR CMAKEFORMAT_FOUND" OFF)

# }}}
# CONFIGURATION {{{

set(CMAKE_CXX_EXTENSIONS OFF)

if(ASTRI_CCACHE)
  set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_EXECUTABLE}")
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_EXECUTABLE}")
endif()

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
  if(NOT EXISTS "${PROJECT_SOURCE_DIR}/.git/hooks/commit-msg"
     AND EXISTS "${PROJECT_SOURCE_DIR}/.github/hooks/commit-msg")
    execute_process(
      COMMAND
        ${CMAKE_COMMAND} -E create_symlink
        "${PROJECT_SOURCE_DIR}/.github/hooks/commit-msg"
        "${PROJECT_SOURCE_DIR}/.git/hooks/commit-msg")
  endif()
endif()

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug|RelWithDebInfo")
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  if(NOT EXISTS "${PROJECT_SOURCE_DIR}/compile_commands.json")
    execute_process(
      COMMAND
        ${CMAKE_COMMAND} -E create_symlink
        "${PROJECT_BINARY_DIR}/compile_commands.json"
        "${PROJECT_SOURCE_DIR}/compile_commands.json")
  endif()
endif()

# }}}
# DEPENDENCIES {{{

include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.9.2
  GIT_SHALLOW TRUE)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 8.0.1
  GIT_SHALLOW TRUE)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw
  GIT_TAG 3.3.5
  GIT_SHALLOW TRUE)
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG docking
  GIT_SHALLOW TRUE)

FetchContent_GetProperties(spdlog)
if(NOT spdlog_POPULATED)
  FetchContent_Populate(spdlog)
  add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(fmt)
if(NOT fmt_POPULATED)
  FetchContent_Populate(fmt)
  add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
  FetchContent_Populate(glfw)
  add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_GetProperties(imgui SOURCE_DIR imgui_SOURCE_DIR)
if(NOT imgui_POPULATED)
  FetchContent_Populate(imgui)

  add_library(
    imgui
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)
  target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR}"
                                          "${imgui_SOURCE_DIR}/backends")
  target_link_libraries(imgui PUBLIC glfw)
  add_library(imgui::imgui ALIAS imgui)
endif()

# }}}
# TARGETS {{{

add_subdirectory(scripts)
add_subdirectory(src)

# }}}
# SUMMARY {{{

include(FeatureSummary)

set_package_properties(OpenGL PROPERTIES URL "https://www.opengl.org/")
set_package_properties(CCache PROPERTIES URL "https://ccache.dev/")
set_package_properties(Git PROPERTIES URL "https://git-scm.com/")
set_package_properties(Cppcheck PROPERTIES URL "http://cppcheck.net/")
set_package_properties(ClangTidy PROPERTIES
                       URL "https://clang.llvm.org/extra/clang-tidy/")
set_package_properties(Cpplint PROPERTIES
                       URL "https://github.com/cpplint/cpplint")
set_package_properties(IWYU PROPERTIES URL "https://include-what-you-use.org/")
set_package_properties(
  CmakeLint PROPERTIES
  URL "https://cmake-format.readthedocs.io/en/latest/cmake-lint.html")

set_package_properties(ClangFormat PROPERTIES
                       URL "https://clang.llvm.org/docs/ClangFormat.html")
set_package_properties(
  CmakeFormat PROPERTIES
  URL "https://cmake-format.readthedocs.io/en/latest/cmake-format.html")

set_package_properties(
  Threads PROPERTIES
  URL "https://cmake.org/cmake/help/latest/module/FindThreads.html")

add_feature_info(IPO ASTRI_IPO "Interprocedural Optimization")
add_feature_info(Linters ASTRI_LINTERS "Enable static analysers")
add_feature_info(Formatters ASTRI_FORMATTERS "Enable formatters")

feature_summary(INCLUDE_QUIET_PACKAGES QUIET_ON_EMPTY
                WHAT PACKAGES_FOUND ENABLED_FEATURES DISABLED_FEATURES)

# }}}
