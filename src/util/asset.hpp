#ifndef INCLUDE_UTIL_ASSET_HPP_
#define INCLUDE_UTIL_ASSET_HPP_

#include <string>

namespace astri::util {
  bool decode85(const char *src, char *dst);
  std::string decode85(const char *data);
} // namespace astri::util

#endif // INCLUDE_UTIL_ASSET_HPP_
