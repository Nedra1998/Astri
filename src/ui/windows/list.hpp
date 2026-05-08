#ifndef INCLUDE_WINDOWS_LIST_HPP_
#define INCLUDE_WINDOWS_LIST_HPP_

#include <map>
#include <vector>

#include <sgp4/element_set.hpp>

#include "state.hpp"
#include "ui/window.hpp"

namespace astri::ui {
  class ListWindow : public Window {
  public:
    ListWindow() : Window("  Satelite List") {}

    void draw() override;

  private:
    char search_buffer_[256] = {};

    bool cache_dirty_ = true;
    std::string search_string_;
    std::vector<std::pair<sgp4::ElementSet, SatelliteState>> sorted_satelites_;
    std::map<std::string, std::pair<bool, std::vector<std::size_t>>> group_cache_;
    std::vector<std::string> group_order_;
  };
} // namespace astri::ui

#endif // INCLUDE_WINDOWS_LIST_HPP_
