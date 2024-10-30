#ifndef INCLUDE_WINDOWS_LIST_HPP_
#define INCLUDE_WINDOWS_LIST_HPP_

#include "ui/window.hpp"
namespace astri::ui {
  class ListWindow : public Window {
  public:
    ListWindow() : Window("  Satelite List") {}

    void draw() override;
  };
} // namespace astri::ui

#endif // INCLUDE_WINDOWS_LIST_HPP_
