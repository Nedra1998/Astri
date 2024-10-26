#ifndef INCLUDE_UI_UI_HPP_
#define INCLUDE_UI_UI_HPP_

#include "gl/gl.hpp"

namespace astri::ui {
  bool initialize(GLFWwindow *window);
  void terminate();

  void frame(GLFWwindow *window);

  void main_menu();

} // namespace astri::ui

#endif // INCLUDE_UI_UI_HPP_
