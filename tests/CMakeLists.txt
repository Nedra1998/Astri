# ---- Dependencies ----
CPMAddPackage(
  NAME Catch2
  GITHUB_REPOSITORY catchorg/Catch2
  GIT_TAG v3.7.0
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE)

# ---- Sources ----
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# ---- Executable ----
add_executable(${PROJECT_NAME_LOWER}-tests ${headers} ${sources})
add_executable(${PROJECT_NAME_LOWER}::tests ALIAS ${PROJECT_NAME_LOWER}-tests)

set_target_properties(${PROJECT_NAME_LOWER}-tests PROPERTIES OUTPUT_NAME unit-tests)
target_compile_features(${PROJECT_NAME_LOWER}-tests PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME_LOWER}-tests
                      PRIVATE ${PROJECT_NAME_LOWER}::${PROJECT_NAME_LOWER} Catch2::Catch2)
target_include_directories(${PROJECT_NAME_LOWER}-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# ---- Add Tests ----
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(${PROJECT_NAME_LOWER}-tests)
