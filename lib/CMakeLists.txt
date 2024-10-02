# ---- Dependencies ----
CPMAddPackage(
  NAME fmt
  GITHUB_REPOSITORY fmtlib/fmt
  GIT_TAG 11.0.2
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE)

CPMAddPackage(
  NAME spdlog
  GITHUB_REPOSITORY gabime/spdlog
  GIT_TAG v1.14.1
  OPTIONS "SPDLOG_FMT_EXTERNAL ON"
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE)

# ---- Sources ----
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/include/astri/version.hpp" @ONLY)

# ---- Library ----
add_library(${PROJECT_NAME_LOWER}-lib ${headers} ${sources})
add_library(${PROJECT_NAME_LOWER}::${PROJECT_NAME_LOWER} ALIAS ${PROJECT_NAME_LOWER}-lib)

set_target_properties(${PROJECT_NAME_LOWER}-lib PROPERTIES OUTPUT_NAME ${PROJECT_NAME_LOWER})
target_compile_features(${PROJECT_NAME_LOWER}-lib PUBLIC cxx_std_17)
target_compile_definitions(${PROJECT_NAME_LOWER}-lib
                           PUBLIC -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
target_link_libraries(${PROJECT_NAME_LOWER}-lib PUBLIC fmt::fmt spdlog::spdlog)
target_include_directories(
  ${PROJECT_NAME_LOWER}-lib
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include> $<INSTALL_INTERFACE:include>)
