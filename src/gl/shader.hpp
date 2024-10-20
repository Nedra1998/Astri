#ifndef INCLUDE_GL_SHADER_HPP_
#define INCLUDE_GL_SHADER_HPP_

#include <vector>

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

  private:
    bool compile_shader(const char *source, GLenum type, GLuint &shader);
    bool link_program(GLuint vertex, GLuint fragment, GLuint &program);

    unsigned int id_ = 0;
    std::vector<std::pair<GLenum, GLuint>> shaders_;
  };
} // namespace astri::gl

#endif // INCLUDE_GL_SHADER_HPP_
