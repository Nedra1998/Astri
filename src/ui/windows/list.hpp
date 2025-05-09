#ifndef INCLUDE_WINDOWS_LIST_HPP_
#define INCLUDE_WINDOWS_LIST_HPP_

#include <vector>

#include <sgp4/element_set.hpp>

#include "ui/window.hpp"
namespace astri::ui {
  class ListWindow : public Window {
  public:
    ListWindow() : Window("  Satelite List") {}

    void draw() override;

  private:
    std::vector<sgp4::ElementSet> satelites_;
  };
} // namespace astri::ui

#endif // INCLUDE_WINDOWS_LIST_HPP_
