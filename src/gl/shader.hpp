#ifndef INCLUDE_GL_SHADER_HPP_
#define INCLUDE_GL_SHADER_HPP_

#include <string_view>
#include <vector>

#include <glm/glm.hpp>

#include "gl/gl.hpp"

namespace astri::gl {
  class Shader {
  public:
    Shader(const char *vertex_shader, const char *fragment_shader);
    ~Shader();

    inline bool is_valid() const { return id_ != 0; }

    inline void use() { glUseProgram(id_); }
    inline void operator()() { use(); }

    inline void set(const std::string_view &name, bool value) const {
      glUseProgram(id_);
      glUniform1i(glGetUniformLocation(id_, name.data()), static_cast<int>(value));
      glUseProgram(0);
    }
    inline void set(const std::string_view &name, int value) const {
      glUseProgram(id_);
      glUniform1i(glGetUniformLocation(id_, name.data()), value);
      glUseProgram(0);
    }
    inline void set(const std::string_view &name, float value) const {
      glUseProgram(id_);
      glUniform1f(glGetUniformLocation(id_, name.data()), value);
      glUseProgram(0);
    }
    inline void set(const std::string_view &name, const glm::vec2 &value) const {
      glUseProgram(id_);
      glUniform2fv(glGetUniformLocation(id_, name.data()), 1, &value[0]);
      glUseProgram(0);
    }
    inline void set(const std::string_view &name, const glm::vec3 &value) const {
      glUseProgram(id_);
      glUniform3fv(glGetUniformLocation(id_, name.data()), 1, &value[0]);
      glUseProgram(0);
    }
    inline void set(const std::string_view &name, const glm::vec4 &value) const {
      glUseProgram(id_);
      glUniform4fv(glGetUniformLocation(id_, name.data()), 1, &value[0]);
      glUseProgram(0);
    }
    inline void set(const std::string_view &name, const glm::mat2 &value) const {
      glUseProgram(id_);
      glUniformMatrix2fv(glGetUniformLocation(id_, name.data()), 1, GL_FALSE, &value[0][0]);
      glUseProgram(0);
    }
    inline void set(const std::string_view &name, const glm::mat3 &value) const {
      glUseProgram(id_);
      glUniformMatrix3fv(glGetUniformLocation(id_, name.data()), 1, GL_FALSE, &value[0][0]);
      glUseProgram(0);
    }
    inline void set(const std::string_view &name, const glm::mat4 &value) const {
      glUseProgram(id_);
      glUniformMatrix4fv(glGetUniformLocation(id_, name.data()), 1, GL_FALSE, &value[0][0]);
      glUseProgram(0);
    }

    inline glm::mat4 get_mat4(const std::string_view &name) const {
      glm::mat4 value;
      glGetUniformfv(id_, glGetUniformLocation(id_, name.data()), &value[0][0]);
      return value;
    }

  private:
    bool compile(const char *source, GLenum type, GLuint &shader);
    bool link(std::vector<GLuint> shaders, GLuint &program);

    unsigned int id_ = 0;
  };
} // namespace astri::gl

#endif // INCLUDE_GL_SHADER_HPP_
