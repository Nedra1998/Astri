# ---- Dependencies ----
CPMAddPackage(
  NAME cxxopts
  GITHUB_REPOSITORY jarro2783/cxxopts
  GIT_TAG v3.2.0
  EXCLUDE_FROM_ALL TRUE
  SYSTEM TRUE)

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

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
  find_program(GZIP_EXECUTABLE gzip)

  CPMAddPackage(
    NAME emscripen_browser_clipboard
    GITHUB_REPOSITORY Armchair-Software/emscripten-browser-clipboard
    GIT_TAG main
    DOWNLOAD_ONLY YES)

  CPMAddPackage(
    NAME emscripen_browser_cursor
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

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
  add_library(emscripen_browser_clipboard EXCLUDE_FROM_ALL INTERFACE)
  target_include_directories(emscripen_browser_clipboard
                             INTERFACE ${emscripen_browser_clipboard_SOURCE_DIR})

  add_library(emscripen_browser_cursor EXCLUDE_FROM_ALL INTERFACE)
  target_include_directories(emscripen_browser_cursor
                             INTERFACE ${emscripen_browser_cursor_SOURCE_DIR})
else()
  add_executable(binary_to_compressed_c EXCLUDE_FROM_ALL
                 ${imgui_SOURCE_DIR}/misc/fonts/binary_to_compressed_c.cpp)
endif()

add_library(
  implot EXCLUDE_FROM_ALL ${implot_SOURCE_DIR}/implot.cpp ${implot_SOURCE_DIR}/implot_items.cpp
                          ${implot_SOURCE_DIR}/implot_demo.cpp)
target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot PRIVATE imgui)

# ---- Sources ----
set(assets)
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in"
               "${CMAKE_CURRENT_BINARY_DIR}/include/version.hpp" @ONLY)

if(NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/vertex.h
           ${CMAKE_CURRENT_BINARY_DIR}/include/fragment.h
    COMMAND binary_to_compressed_c -nocompress ${PROJECT_SOURCE_DIR}/assets/shaders/fragment.glsl
            fragment > ${CMAKE_CURRENT_BINARY_DIR}/include/fragment.h
    COMMAND binary_to_compressed_c -nocompress ${PROJECT_SOURCE_DIR}/assets/shaders/vertex.glsl
            vertex > ${CMAKE_CURRENT_BINARY_DIR}/include/vertex.h
    DEPENDS ${PROJECT_SOURCE_DIR}/assets/shaders/fragment.glsl
            ${PROJECT_SOURCE_DIR}/assets/shaders/vertex.glsl)

  list(APPEND assets ${CMAKE_CURRENT_BINARY_DIR}/include/fragment.h
       ${CMAKE_CURRENT_BINARY_DIR}/include/vertex.h)

  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/iosevka.h
           ${CMAKE_CURRENT_BINARY_DIR}/include/iosevka_bold.h
           ${CMAKE_CURRENT_BINARY_DIR}/include/iosevka_italic.h
           ${CMAKE_CURRENT_BINARY_DIR}/include/iosevka_bold_italic.h
    COMMAND
      binary_to_compressed_c -base85 ${PROJECT_SOURCE_DIR}/assets/fonts/IosevkaNerdFont-Regular.ttf
      iosevka > ${CMAKE_CURRENT_BINARY_DIR}/include/iosevka.h
    COMMAND
      binary_to_compressed_c -base85 ${PROJECT_SOURCE_DIR}/assets/fonts/IosevkaNerdFont-Bold.ttf
      iosevka_bold > ${CMAKE_CURRENT_BINARY_DIR}/include/iosevka_bold.h
    COMMAND
      binary_to_compressed_c -base85 ${PROJECT_SOURCE_DIR}/assets/fonts/IosevkaNerdFont-Italic.ttf
      iosevka_italic > ${CMAKE_CURRENT_BINARY_DIR}/include/iosevka_italic.h
    COMMAND
      binary_to_compressed_c -base85
      ${PROJECT_SOURCE_DIR}/assets/fonts/IosevkaNerdFont-BoldItalic.ttf iosevka_bold_italic >
      ${CMAKE_CURRENT_BINARY_DIR}/include/iosevka_bold_italic.h)

  list(
    APPEND
    assets
    ${CMAKE_CURRENT_BINARY_DIR}/include/iosevka.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/iosevka_bold.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/iosevka_italic.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/iosevka_bold_italic.h)
endif()

# ---- Executable ----
add_executable(${PROJECT_NAME_LOWER} ${headers} ${sources} ${assets})
add_executable(${PROJECT_NAME_LOWER}::${PROJECT_NAME_LOWER} ALIAS ${PROJECT_NAME_LOWER})

set_target_properties(${PROJECT_NAME_LOWER} PROPERTIES OUTPUT_NAME ${PROJECT_NAME_LOWER})
target_compile_features(${PROJECT_NAME_LOWER} PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME_LOWER} PRIVATE cxxopts::cxxopts fmt::fmt spdlog::spdlog imgui
                                                    implot)
target_include_directories(${PROJECT_NAME_LOWER} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                                         ${CMAKE_CURRENT_BINARY_DIR}/include)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
  target_compile_definitions(${PROJECT_NAME_LOWER} PRIVATE -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
  target_link_libraries(${PROJECT_NAME_LOWER} PRIVATE emscripen_browser_clipboard
                                                      emscripen_browser_cursor)

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
    --preload-file ${PROJECT_SOURCE_DIR}/assets@/assets
    --shell-file
    ${PROJECT_SOURCE_DIR}/assets/shell.html)
else()
  target_compile_definitions(${PROJECT_NAME_LOWER} PRIVATE -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
  target_link_libraries(${PROJECT_NAME_LOWER} PRIVATE glfw)
endif()

# ---- Post Build Commands ----

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten" AND GZIP_EXECUTABLE)
  add_custom_command(
    TARGET ${PROJECT_NAME_LOWER}
    COMMENT "Compressing WebAssembly Binary"
    POST_BUILD
    COMMAND ${GZIP_EXECUTABLE} -9 --force --keep "${CMAKE_CURRENT_BINARY_DIR}/astri.wasm")
endif()
