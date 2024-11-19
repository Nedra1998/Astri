#ifndef INCLUDE_CORE_CLIENT_HPP_
#define INCLUDE_CORE_CLIENT_HPP_

#include <filesystem>
#include <string_view>

#include <httplib.h>

#include "core/cache.hpp"
#include "core/model.hpp"

namespace astri::core {
  class Client {
  public:
    static constexpr std::string_view GROUPS[] = {"active"};

    Client(std::filesystem::path cache, std::string_view domain = "https://celestrak.org",
           std::chrono::duration<float> cache_duration = std::chrono::hours(1))
        : cache_(cache), client_(domain.data()), cache_duration_(cache_duration) {}

    bool load(std::vector<std::string_view> groups);
    inline bool load() {
      return load(std::vector<std::string_view>(std::begin(GROUPS), std::end(GROUPS)));
    }

    std::optional<std::vector<GeneralPertubations>> get(std::string_view group);

  private:
    std::chrono::duration<float> cache_duration_;
    httplib::Client client_;
    Cache<std::string_view, std::vector<GeneralPertubations>> cache_;
  };
} // namespace astri::core

#endif // INCLUDE_CORE_CLIENT_HPP_
