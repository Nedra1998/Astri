file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE FONTS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/assets/*.ttf")

include(GenerateVersionHeader)
generate_version_header(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/include/version.hpp")
list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/include/version.hpp")

foreach(font ${FONTS})
  get_filename_component(name "${font}" NAME_WE)
  string(MAKE_C_IDENTIFIER "${name}" ident)
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/include/${name}.hpp"
    COMMAND binary_to_compressed_c "${font}" "${ident}" >
            "${CMAKE_CURRENT_BINARY_DIR}/include/${name}.hpp")
  list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/include/${name}.hpp")
endforeach()

# add_custom_command(
#   OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/include/changelog.hpp"
#   COMMAND
#     binary_to_compressed_c -nocompress "${PROJECT_SOURCE_DIR}/CHANGELOG.md"
#     "changelog" > "${CMAKE_CURRENT_BINARY_DIR}/include/changelog.hpp"
#   DEPENDS "${PROJECT_SOURCE_DIR}/CHANGELOG.md")
# list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/include/changelog.hpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION
                                                 ${ASTRI_IPO})
target_include_directories(
  ${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
                          "${CMAKE_CURRENT_BINARY_DIR}/include")
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog fmt::fmt glfw
                                              OpenGL::OpenGL imgui::imgui)

target_compile_definitions(${PROJECT_NAME} PRIVATE -DSPDLOG_FMT_EXTERNAL)

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug|RelWithDebInfo")
  target_compile_definitions(${PROJECT_NAME}
                             PRIVATE -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
else()
  target_compile_definitions(${PROJECT_NAME}
                             PRIVATE -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
endif()

target_precompile_headers(${PROJECT_NAME} PUBLIC)
