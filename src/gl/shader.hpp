#ifndef INCLUDE_SRC_SHADER_HPP_
#define INCLUDE_SRC_SHADER_HPP_

#include <filesystem>

#include "gl/gl.hpp"

namespace astri {
  class Shader {
  public:
    Shader(std::filesystem::path vertex_path, std::filesystem::path fragment_path);
    Shader(const std::string &vertex_source, const std::string &fragment_source);

    inline bool valid() const { return valid_; }

    inline void use() const { glUseProgram(program_); }
    inline void operator()() const { use(); }

  private:
    bool compile_shader(const char *source, GLenum type, GLuint &shader);
    bool link_program(GLuint vertex, GLuint fragment, GLuint &program);

    bool valid_;
    unsigned int program_;
  };
} // namespace astri

#endif // INCLUDE_SRC_SHADER_HPP_
