# ---- Dependencies ----

# Common Dependencies
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

CPMAddPackage(
  NAME tracy
  GITHUB_REPOSITORY wolfpld/tracy
  GIT_TAG v0.11.1
  OPTIONS "TRACY_ENABLE ${ASTRI_ENABLE_PROFILING}"
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE)

# Astri Core Dependencies
CPMAddPackage(
  NAME cpp-httplib
  GITHUB_REPOSITORY yhirose/cpp-httplib
  GIT_TAG v0.18.1
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE)

CPMAddPackage(
  NAME json
  GITHUB_REPOSITORY nlohmann/json
  GIT_TAG v3.11.3
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE)

CPMAddPackage(
  NAME flecs
  GITHUB_REPOSITORY SanderMertens/flecs
  GIT_TAG v4.0.2
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE)

# Graphics Dependencies
CPMAddPackage(
  NAME glm
  GITHUB_REPOSITORY g-truc/glm
  GIT_TAG 1.0.1
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE)

CPMAddPackage(
  NAME imgui
  GITHUB_REPOSITORY ocornut/imgui
  GIT_TAG v1.91.2-docking
  DOWNLOAD_ONLY TRUE
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE)

CPMAddPackage(
  NAME implot
  GITHUB_REPOSITORY epezent/implot
  GIT_TAG v0.16
  DOWNLOAD_ONLY TRUE
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE)

CPMAddPackage(
  NAME GLFW
  GITHUB_REPOSITORY glfw/glfw
  GIT_TAG 3.3.8
  OPTIONS "GLFW_BUILD_DOCS OFF" "GLFW_BUILD_EXAMPLES OFF" "GLFW_BUILD_TESTS OFF"
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE)

# ---- Dependency Targets ----
add_library(
  imgui EXCLUDE_FROM_ALL
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends
                                        ${imgui_SOURCE_DIR}/misc/cpp)

target_link_libraries(imgui PRIVATE glfw)

add_executable(binary_to_compressed_c EXCLUDE_FROM_ALL
               ${imgui_SOURCE_DIR}/misc/fonts/binary_to_compressed_c.cpp)

add_library(
  implot EXCLUDE_FROM_ALL ${implot_SOURCE_DIR}/implot.cpp ${implot_SOURCE_DIR}/implot_items.cpp
                          ${implot_SOURCE_DIR}/implot_demo.cpp)
target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot PRIVATE imgui)

# ---- Assets ----
set(assets)
file(GLOB_RECURSE asset_sources CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/assets/*")

foreach(asset ${asset_sources})
  get_filename_component(asset_name ${asset} NAME)
  get_filename_component(asset_namespace ${asset} NAME_WE)
  string(REGEX REPLACE "-" "_" asset_namespace ${asset_namespace})
  string(TOLOWER ${asset_namespace} asset_namespace)

  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/include/${asset_namespace}.h"
    COMMAND binary_to_compressed_c -base85 "${asset}" "${asset_namespace}" >
            "${CMAKE_CURRENT_BINARY_DIR}/include/${asset_namespace}.h"
    DEPENDS binary_to_compressed_c "${asset}"
    COMMENT "Embedding ${asset_name}")

  list(APPEND assets "${CMAKE_CURRENT_BINARY_DIR}/include/${asset_namespace}.h")
endforeach()

# ---- Sources ----
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/include/version.hpp" @ONLY)
list(APPEND headers "${CMAKE_CURRENT_BINARY_DIR}/include/version.hpp")

# ---- Executable ----
add_executable(${PROJECT_NAME_LOWER} ${headers} ${sources} ${assets})
add_executable(${PROJECT_NAME_LOWER}::${PROJECT_NAME_LOWER} ALIAS ${PROJECT_NAME_LOWER})

target_compile_features(${PROJECT_NAME_LOWER} PRIVATE cxx_std_20)
target_include_directories(${PROJECT_NAME_LOWER} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include"
                                                         "${CURRENT_SOURCE_DIR}")

target_link_libraries(
  ${PROJECT_NAME_LOWER}
  PRIVATE fmt::fmt
          spdlog::spdlog
          imgui
          implot
          glm::glm
          httplib::httplib
          nlohmann_json::nlohmann_json
          glfw
          flecs::flecs_static
          TracyClient)
target_compile_definitions(${PROJECT_NAME_LOWER} PRIVATE -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
