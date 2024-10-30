#ifndef INCLUDE_GL_GL_HPP_
#define INCLUDE_GL_GL_HPP_

#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

namespace astri::gl {
  extern std::string GLSL_VERSION;

  GLFWwindow *initialize(uint32_t width, uint32_t height);
  void terminate(GLFWwindow *window);
} // namespace astri::gl

#endif // INCLUDE_GL_GL_HPP_
