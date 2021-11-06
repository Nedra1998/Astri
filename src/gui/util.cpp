#include "gui/util.hpp"

#include <string>
#include <string_view>

#if defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include "Shellapi.h"
#  include <Windows.h>
#endif

#include "logging.hpp"

void astri::gui::open_url(std::string_view url) {
  LDEBUG("imgui", "Opening url \"{}\" in default browser", url);
#if defined(_WIN32)
  ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
#else
  system(("xdg-open " + std::string(url) + "&").c_str());
#endif
}
