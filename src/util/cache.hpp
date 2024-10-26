#ifndef INCLUDE_UTIL_CACHE_HPP_
#define INCLUDE_UTIL_CACHE_HPP_

#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>

namespace astri::util {
  class Cache {
  public:
    using Clock = std::chrono::system_clock;
    using TimePoint = std::chrono::time_point<Clock, std::chrono::seconds>;

    static Cache &get() {
      static Cache instance("./.cache");
      return instance;
    }

    Cache(std::filesystem::path path) : path(path) {
      if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
      }
    }

    template <typename T> void put(const std::string_view &key, const T &value) {
      std::ofstream file(path / key, std::ios_base::binary);
      file.write(reinterpret_cast<const char *>(&value), sizeof(value));
      file.close();
    }

    template <typename T> T get(const std::string_view &key) {
      T value;
      std::ifstream file(path / key, std::ios_base::binary);
      file.read(reinterpret_cast<char *>(&value), sizeof(value));
      file.close();
      return value;
    }

    template <typename R>
    bool contains(const std::string_view &key, const std::chrono::duration<R> ttl) {
      std::filesystem::path file = path / key;
      if (!std::filesystem::exists(file)) {
        return false;
      }

      if (std::chrono::clock_cast<Clock>(std::filesystem::last_write_time(file)) <
          Clock::now() - ttl) {
        std::filesystem::remove(file);
        return false;
      }
      return true;
    }

    void remove(const std::string_view &key) { std::filesystem::remove(path / key); }

    template <typename T, typename R>
    T operator()(const std::string_view &key, std::function<T()> func,
                 const std::chrono::duration<R> ttl) {
      if (!contains(key, ttl)) {
        const T val = func();
        put(key, val);
        return val;
      }
      return get<T>(key);
    }

  private:
    Cache() = default;
    ~Cache() = default;
    Cache(const Cache &) = delete;
    Cache &operator=(const Cache &) = delete;

    std::filesystem::path path;
  };
} // namespace astri::util

#endif // INCLUDE_UTIL_CACHE_HPP_
