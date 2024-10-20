#include "gl/gl.hpp"

#include "log.hpp"

namespace astri::gl {
  std::string GLSL_VERSION;
} // namespace astri::gl

void glfw_callback(int error, const char *description) {
  LOG_ERROR("GLFW Error {}: {}", error, description);
}

GLFWwindow *astri::gl::initialize(uint32_t width, uint32_t height) {
  LOG_ZONE();

  LOG_INFO("GLFW v{}", glfwGetVersionString());

  glfwSetErrorCallback(glfw_callback);
  if (!glfwInit()) {
    LOG_CRITICAL("Failed to initialize GLFW");
    return nullptr;
  }

#ifdef __EMSCRIPTEN__
  GLSL_VERSION = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  LOG_INFO("OpenGL ES v{} + GLSL {}", "2.0", 100);
#else
  GLSL_VERSION = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  LOG_INFO("OpenGL v{} + GLSL {}", "3.3", 150);
#endif // __EMSCRIPTEN__

  GLFWwindow *window = glfwCreateWindow(width, height, "Astri", nullptr, nullptr);
  if (!window) {
    LOG_CRITICAL("Failed to create GLFW window");
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);

#ifndef __EMSCRIPTEN__
  glfwSwapInterval(1);
#endif // !__EMSCRIPTEN__

#ifdef __EMSCRIPTEN__
  if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
    LOG_CRITICAL("GLAD failed to load OpenGL ES");
    glfwDestroyWindow(window);
    glfwTerminate();
    return nullptr;
  }
#else
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG_CRITICAL("GLAD failed to load OpenGL");
    glfwDestroyWindow(window);
    glfwTerminate();
    return nullptr;
  }
#endif // __EMSCRIPTEN__

  return window;
}

void astri::gl::terminate(GLFWwindow *window) {
  LOG_ZONE();

  if (window)
    glfwDestroyWindow(window);
  glfwTerminate();
}
