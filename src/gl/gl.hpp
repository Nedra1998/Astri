#ifndef INCLUDE_SRC_GL_HPP_
#define INCLUDE_SRC_GL_HPP_

#include <cstdint>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

#include "gl/glad/glad.hpp"

#ifdef __EMSCRIPTEN__
#  define glGenVertexArrays glGenVertexArraysOES;
#  define glBindVertexArray glBindVertexArrayOES;
#  define glDeleteVertexArrays glDeleteVertexArraysOES;
#endif

namespace astri::gl {
  GLFWwindow *initialize(std::uint32_t width, std::uint32_t height);
  void terminate(GLFWwindow *window);
} // namespace astri::gl

#endif // INCLUDE_SRC_GL_HPP_
