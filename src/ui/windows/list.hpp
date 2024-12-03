#ifndef INCLUDE_WINDOWS_LIST_HPP_
#define INCLUDE_WINDOWS_LIST_HPP_

#include <vector>

#include "core/model.hpp"
#include "ui/window.hpp"
namespace astri::ui {
  class ListWindow : public Window {
  public:
    ListWindow() : Window("  Satelite List") {}

    void draw() override;

  private:
    std::vector<GeneralPertubations> satelites_;
  };
} // namespace astri::ui

#endif // INCLUDE_WINDOWS_LIST_HPP_
