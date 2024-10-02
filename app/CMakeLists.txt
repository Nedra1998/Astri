# ---- Dependencies ----
CPMAddPackage(
  NAME cxxopts
  GITHUB_REPOSITORY jarro2783/cxxopts
  GIT_TAG v3.2.0
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE)

# ---- Sources ----
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# ---- Library ----
add_executable(${PROJECT_NAME_LOWER}-app ${headers} ${sources})
add_executable(${PROJECT_NAME_LOWER}::app ALIAS ${PROJECT_NAME_LOWER}-app)

set_target_properties(${PROJECT_NAME_LOWER}-app PROPERTIES OUTPUT_NAME ${PROJECT_NAME_LOWER})
target_compile_features(${PROJECT_NAME_LOWER}-app PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME_LOWER}-app PRIVATE ${PROJECT_NAME_LOWER}::${PROJECT_NAME_LOWER}
                                                        cxxopts::cxxopts)
target_include_directories(${PROJECT_NAME_LOWER}-app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
