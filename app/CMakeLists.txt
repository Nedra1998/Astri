# ---- Dependencies ----
CPMAddPackage(
  NAME cxxopts
  GITHUB_REPOSITORY jarro2783/cxxopts
  GIT_TAG v3.2.0
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
  find_program(GZIP_EXECUTABLE gzip)
else()
  CPMAddPackage(
    NAME GLFW
    GITHUB_REPOSITORY glfw/glfw
    GIT_TAG 3.3.8
    OPTIONS "GLFW_BUILD_DOCS OFF" "GLFW_BUILD_EXAMPLES OFF" "GLFW_BUILD_TESTS OFF"
    EXCLUDE_FROM_ALL TRUE
    SYSTEM TRUE)
endif()

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
if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
  message(STATUS "Building for Desktop")
  target_link_libraries(imgui PRIVATE glfw)
endif()

add_library(
  implot EXCLUDE_FROM_ALL ${implot_SOURCE_DIR}/implot.cpp ${implot_SOURCE_DIR}/implot_items.cpp
                          ${implot_SOURCE_DIR}/implot_demo.cpp)
target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot PRIVATE imgui)

# ---- Sources ----
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# ---- Executable ----
add_executable(${PROJECT_NAME_LOWER}-app ${headers} ${sources})
add_executable(${PROJECT_NAME_LOWER}::app ALIAS ${PROJECT_NAME_LOWER}-app)

set_target_properties(${PROJECT_NAME_LOWER}-app PROPERTIES OUTPUT_NAME ${PROJECT_NAME_LOWER})
target_compile_features(${PROJECT_NAME_LOWER}-app PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME_LOWER}-app PRIVATE ${PROJECT_NAME_LOWER}::${PROJECT_NAME_LOWER}
                                                        cxxopts::cxxopts imgui implot)
target_include_directories(${PROJECT_NAME_LOWER}-app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  target_link_options(
    ${PROJECT_NAME_LOWER}-app
    PRIVATE
    -SWASM=1
    -sUSE_GLFW=3
    -sALLOW_MEMORY_GROWTH=1
    -sNO_EXIT_RUNTIME=0
    -sGL_ENABLE_GET_PROC_ADDRESS=1
    -sASSERTIONS=1
    -sNO_DISABLE_EXCEPTION_CATCHING=1
    -sEXPORTED_FUNCTIONS=_main,_malloc,_free
    -sEXPORTED_RUNTIME_METHODS=ccall
    -lidbfs.js
    --shell-file
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/shell.html)
else()
  target_link_libraries(${PROJECT_NAME_LOWER}-app PRIVATE glfw)
endif()

# ---- Post Build Commands ----

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten" AND GZIP_EXECUTABLE)
  add_custom_command(
    TARGET ${PROJECT_NAME_LOWER}-app
    COMMENT "Compressing WebAssembly Binary"
    POST_BUILD
    COMMAND ${GZIP_EXECUTABLE} -9 --force --keep "${CMAKE_CURRENT_BINARY_DIR}/astri.wasm")
endif()
