#ifndef INCLUDE_GL_SHADER_HPP_
#define INCLUDE_GL_SHADER_HPP_

#include <vector>

#include <glm/glm.hpp>

#include "gl/gl.hpp"

namespace astri::gl {
  class Shader {
  public:
    Shader() {}
    ~Shader();

    bool add_shader(const char *source, GLenum type);

    inline bool add_vertex(const char *source) { return add_shader(source, GL_VERTEX_SHADER); }
    inline bool add_fragment(const char *source) { return add_shader(source, GL_FRAGMENT_SHADER); }

    bool link();

    inline bool valid() const { return id_ != 0; }

    inline void use() const { glUseProgram(id_); }
    inline void operator()() const { use(); }

    inline void set(const std::string_view &name, bool value) const {
      glUniform1i(glGetUniformLocation(id_, name.data()), static_cast<int>(value));
    }
    inline void set(const std::string_view &name, int value) const {
      glUniform1i(glGetUniformLocation(id_, name.data()), value);
    }
    inline void set(const std::string_view &name, float value) const {
      glUniform1f(glGetUniformLocation(id_, name.data()), value);
    }
    inline void set(const std::string_view &name, const glm::vec2 &value) const {
      glUniform2fv(glGetUniformLocation(id_, name.data()), 1, &value[0]);
    }
    inline void set(const std::string_view &name, const glm::vec3 &value) const {
      glUniform3fv(glGetUniformLocation(id_, name.data()), 1, &value[0]);
    }
    inline void set(const std::string_view &name, const glm::vec4 &value) const {
      glUniform4fv(glGetUniformLocation(id_, name.data()), 1, &value[0]);
    }
    inline void set(const std::string_view &name, const glm::mat2 &value) const {
      glUniformMatrix2fv(glGetUniformLocation(id_, name.data()), 1, GL_FALSE, &value[0][0]);
    }
    inline void set(const std::string_view &name, const glm::mat3 &value) const {
      glUniformMatrix3fv(glGetUniformLocation(id_, name.data()), 1, GL_FALSE, &value[0][0]);
    }
    inline void set(const std::string_view &name, const glm::mat4 &value) const {
      glUniformMatrix4fv(glGetUniformLocation(id_, name.data()), 1, GL_FALSE, &value[0][0]);
    }

    inline glm::mat4 get_mat4(const std::string_view &name) const {
      glm::mat4 value;
      glGetUniformfv(id_, glGetUniformLocation(id_, name.data()), &value[0][0]);
      return value;
    }

  private:
    bool compile_shader(const char *source, GLenum type, GLuint &shader);
    bool link_program(GLuint vertex, GLuint fragment, GLuint &program);

    unsigned int id_ = 0;
    std::vector<std::pair<GLenum, GLuint>> shaders_;
  };
} // namespace astri::gl

#endif // INCLUDE_GL_SHADER_HPP_
