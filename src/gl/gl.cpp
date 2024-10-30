#include "gl/gl.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "log.hpp"

#ifndef __EMSCRIPTEN__
#  include "fragment.h"
#  include "vertex.h"
#endif // !__EMSCRIPTEN__

namespace astri::gl {
  std::string GLSL_VERSION;
} // namespace astri::gl

static std::shared_ptr<spdlog::logger> GL_LOGGER = nullptr;

void glfw_callback(int error, const char *description) {
  LOGGER_ERROR(GL_LOGGER, "GLFW Error {}: {}", error, description);
}

GLFWwindow *astri::gl::initialize(uint32_t width, uint32_t height) {
  LOG_ZONE();

  GL_LOGGER = astri::log::get("gl");

  LOG_INFO("GLFW v{}", glfwGetVersionString());

  glfwSetErrorCallback(glfw_callback);
  if (!glfwInit()) {
    LOG_CRITICAL("Failed to initialize GLFW");
    return nullptr;
  }

  GLSL_VERSION = "#version 330";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  LOG_INFO("OpenGL v{} + GLSL {}", "3.3", 150);

  GLFWwindow *window = glfwCreateWindow(width, height, "Astri", nullptr, nullptr);
  if (!window) {
    LOG_CRITICAL("Failed to create GLFW window");
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (!gladLoadGL(glfwGetProcAddress)) {
    LOG_CRITICAL("GLAD failed to load OpenGL");
    glfwDestroyWindow(window);
    glfwTerminate();
    return nullptr;
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  TracyGpuContext;

  return window;
}

void astri::gl::terminate(GLFWwindow *window) {
  LOG_ZONE();

  if (window)
    glfwDestroyWindow(window);
  glfwTerminate();
}
