#ifndef INCLUDE_CORE_CACHE_HPP_
#define INCLUDE_CORE_CACHE_HPP_

#include <filesystem>
#include <fstream>
#include <functional>

#include <nlohmann/json.hpp>

#include "log.hpp"

namespace astri::core {

  template <typename T>
  concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
  };

  template <Hashable Key, typename Value> class Cache {
  public:
    Cache(std::filesystem::path path) : _path(path) {}

    bool has(const Key &key, std::optional<std::chrono::seconds> ttl = std::nullopt) {
      LOG_ZONE();
      std::filesystem::path entry = _path / fmt::format("{:016x}.cbor", std::hash<Key>{}(key));
      if (!std::filesystem::exists(entry)) {
        return false;
      } else if (ttl.has_value() && std::filesystem::last_write_time(entry) + ttl.value() <
                                        std::filesystem::file_time_type::clock::now()) {
        remove(key);
        return false;
      }
      return true;
    }

    void put(const Key &key, const Value &value) {
      LOG_ZONE();
      if (!std::filesystem::exists(_path)) {
        std::filesystem::create_directories(_path);
      }
      std::ofstream file(_path / fmt::format("{:016x}.cbor", std::hash<Key>{}(key)),
                         std::ios::binary);
      nlohmann::json jvalue = value;
      std::vector<std::uint8_t> bytes = nlohmann::json::to_cbor(jvalue);
      file.write(reinterpret_cast<const char *>(bytes.data()), bytes.size());
      file.close();
    }

    std::optional<Value> get(const Key &key,
                             std::optional<std::chrono::seconds> ttl = std::nullopt) {
      LOG_ZONE();

      std::filesystem::path entry = _path / fmt::format("{:016x}.cbor", std::hash<Key>{}(key));
      if (!std::filesystem::exists(entry)) {
        return std::nullopt;
      } else if (ttl.has_value() && std::filesystem::last_write_time(entry) + ttl.value() <
                                        std::filesystem::file_time_type::clock::now()) {
        remove(key);
        return std::nullopt;
      }

      std::ifstream file(entry);
      if (!file.is_open()) {
        return std::nullopt;
      }

      std::vector<std::uint8_t> bytes;
      file.seekg(0, std::ios::end);
      bytes.resize(file.tellg());
      file.seekg(0, std::ios::beg);
      file.read(reinterpret_cast<char *>(bytes.data()), bytes.size());
      file.close();

      nlohmann::json jvalue = nlohmann::json::from_cbor(bytes);
      return jvalue.get<Value>();
    }

    void remove(const Key &key) {
      LOG_ZONE();
      std::filesystem::remove(_path / fmt::format("{:016x}.cbor", std::hash<Key>{}(key)));
    }

    template <typename Rep = float, typename Period = std::ratio<1>>
    Value operator()(const Key &key, std::function<Value(const Key &)> generator,
                     std::chrono::duration<Rep, Period> ttl) {
      LOG_ZONE();
      if (const auto &value =
              get(key, std::make_optional(std::chrono::duration_cast<std::chrono::seconds>(ttl)));
          value.has_value()) {
        return value.value();
      }
      Value value = generator(key);
      put(key, value);
      return value;
    }

    template <typename Rep = float, typename Period = std::ratio<1>>
    std::optional<Value> operator()(const Key &key,
                                    std::function<std::optional<Value>(const Key &)> generator,
                                    std::chrono::duration<Rep, Period> ttl) {
      LOG_ZONE();
      if (const auto &value =
              get(key, std::make_optional(std::chrono::duration_cast<std::chrono::seconds>(ttl)));
          value.has_value()) {
        return value.value();
      }
      std::optional<Value> value = generator(key);
      if (value.has_value()) {
        put(key, value.value());
      }
      return value;
    }

  private:
    std::filesystem::path _path;
  };
} // namespace astri::core

#endif // INCLUDE_CORE_CACHE_HPP_
