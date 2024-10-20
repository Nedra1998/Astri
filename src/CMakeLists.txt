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

if(EMSCRIPTEN)
  find_program(GZIP_EXECUTABLE gzip)

  CPMAddPackage(
    NAME emscripten_browser_clipboard
    GITHUB_REPOSITORY Armchair-Software/emscripten-browser-clipboard
    GIT_TAG main
    DOWNLOAD_ONLY YES)

  CPMAddPackage(
    NAME emscripten_browser_cursor
    GITHUB_REPOSITORY Armchair-Software/emscripten-browser-cursor
    GIT_TAG main
    DOWNLOAD_ONLY YES)

else()
  CPMAddPackage(
    NAME GLFW
    GITHUB_REPOSITORY glfw/glfw
    GIT_TAG 3.3.8
    OPTIONS "GLFW_BUILD_DOCS OFF" "GLFW_BUILD_EXAMPLES OFF" "GLFW_BUILD_TESTS OFF"
    EXCLUDE_FROM_ALL TRUE
    SYSTEM TRUE)
endif()

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

if(NOT EMSCRIPTEN)
  target_link_libraries(imgui PRIVATE glfw)

  add_executable(binary_to_compressed_c EXCLUDE_FROM_ALL
                 ${imgui_SOURCE_DIR}/misc/fonts/binary_to_compressed_c.cpp)
endif()

add_library(
  implot EXCLUDE_FROM_ALL ${implot_SOURCE_DIR}/implot.cpp ${implot_SOURCE_DIR}/implot_items.cpp
                          ${implot_SOURCE_DIR}/implot_demo.cpp)
target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot PRIVATE imgui)

if(EMSCRIPTEN)
  add_library(emscripten_browser_clipboard EXCLUDE_FROM_ALL INTERFACE)
  target_include_directories(emscripten_browser_clipboard
                             INTERFACE ${emscripten_browser_clipboard_SOURCE_DIR})

  add_library(emscripten_browser_cursor EXCLUDE_FROM_ALL INTERFACE)
  target_include_directories(emscripten_browser_cursor
                             INTERFACE ${emscripten_browser_cursor_SOURCE_DIR})
endif()

# ---- Assets ----
set(assets)
file(GLOB_RECURSE asset_sources CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/assets/*")

if(NOT EMSCRIPTEN)
  foreach(asset ${assets})
    get_filename_component(asset_name ${asset} NAME)
    get_filename_component(asset_namespace ${asset} NAME_WE)
    string(REGEX REPLACE "-" "_" asset_namespace ${asset_namespace})
    string(TOLOWER ${asset_namespace} ${asset_namespace})

    add_custom_command(
      OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/include/assets/${asset_name}.h"
      COMMAND binary_to_compressed_c -base85 "${asset}" "${asset_namespace}" >
              "${CMAKE_CURRENT_BINARY_DIR}/include/${asset_namespace}.h"
      DEPENDS binary_to_compressed_c "${asset}"
      COMMENT "Embedding ${asset_name}")

    list(APPEND assets "${CMAKE_CURRENT_BINARY_DIR}/include/assets/${asset_name}.h")
  endforeach()
endif()

# ---- Sources ----
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/include/version.hpp" @ONLY)
list(APPEND headers "${CMAKE_CURRENT_BINARY_DIR}/include/version.hpp")

# ---- Executable ----
add_executable(${PROJECT_NAME_LOWER} ${headers} ${sources} ${assets})
add_executable(${PROJECT_NAME_LOWER}::${PROJECT_NAME_LOWER} ALIAS ${PROJECT_NAME_LOWER})

target_compile_features(${PROJECT_NAME_LOWER} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME_LOWER} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include"
                                                         "${CURRENT_SOURCE_DIR}")

if(EMSCRIPTEN)
  target_link_libraries(${PROJECT_NAME_LOWER} PRIVATE emscripten_browser_clipboard
                                                      emscripten_browser_cursor)
else()
  target_link_libraries(${PROJECT_NAME_LOWER} PRIVATE glfw)
endif()

target_link_libraries(${PROJECT_NAME_LOWER} PRIVATE fmt::fmt spdlog::spdlog imgui implot glm::glm)
if(EMSCRIPTEN)
  target_compile_definitions(${PROJECT_NAME_LOWER} PRIVATE -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
else()
  target_compile_definitions(${PROJECT_NAME_LOWER} PRIVATE -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
endif()

if(EMSCRIPTEN)

  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  target_link_options(
    ${PROJECT_NAME_LOWER}
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
    -sMIN_WEBGL_VERSION=2
    -sMAX_WEBGL_VERSION=2
    -lidbfs.js
    --preload-file
    ${PROJECT_SOURCE_DIR}/assets@/assets
    --shell-file
    ${PROJECT_SOURCE_DIR}/assets/shell.html)
endif()

# ---- Post Build Commands ----
if(EMSCRIPTEN)
  add_custom_command(
    TARGET ${PROJECT_NAME_LOWER}
    COMMENT "Compressing WebAssembly Binary"
    POST_BUILD
    COMMAND ${GZIP_EXECUTABLE} -9 --force --keep "${CMAKE_CURRENT_BINARY_DIR}/astri.wasm")
endif()
