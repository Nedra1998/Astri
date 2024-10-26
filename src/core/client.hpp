#ifndef INCLUDE_CORE_CLIENT_HPP_
#define INCLUDE_CORE_CLIENT_HPP_

#include <chrono>
#include <filesystem>
#include <optional>

#include <httplib.h>

#include "core/model.hpp"

namespace astri::core {
  class Client {
  public:
    static constexpr std::string_view GROUPS[] = {"active"};

    Client(std::filesystem::path cache, std::string_view domain = "https://celestrak.org")
        : cache_(cache), client_(domain.data()) {}

    std::optional<std::vector<GeneralPertubations>> group(std::string_view group);

  private:
    std::optional<std::vector<GeneralPertubations>> request(std::string_view group);
    std::optional<std::vector<GeneralPertubations>> get(std::string_view group) const;
    void put(std::string_view group, std::vector<GeneralPertubations> &satellites) const;

    httplib::Client client_;
    std::filesystem::path cache_;
    std::chrono::seconds ttl_ = std::chrono::hours(24);
  };
} // namespace astri::core

#endif // INCLUDE_CORE_CLIENT_HPP_
