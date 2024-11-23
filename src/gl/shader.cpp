#include "gl/shader.hpp"

#include "gl/gl.hpp"
#include "log.hpp"
#include "util/asset.hpp"

astri::gl::Shader::Shader(const char *vertex_shader, const char *fragment_shader) {
  LOG_ZONE();
  GLuint vertex = 0;
  GLuint fragment = 0;
  if (!compile(vertex_shader, GL_VERTEX_SHADER, vertex) ||
      !compile(fragment_shader, GL_FRAGMENT_SHADER, fragment)) {
    return;
  }
  if (!link({vertex, fragment}, id_)) {
    return;
  }
}

astri::gl::Shader::~Shader() {
  if (id_ != 0) {
    glDeleteProgram(id_);
  }
}

bool astri::gl::Shader::compile(const char *source, GLenum type, GLuint &shader) {
  LOG_ZONE();

  std::string decoded = astri::util::decode85(source);
  if (decoded.empty()) {
    LOG_ERROR("Failed to decode shader source");
    return false;
  }

  decoded.insert(0, fmt::format("{}\n", GLSL_VERSION));
  const char *cstr = decoded.c_str();

  shader = glCreateShader(type);
  glShaderSource(shader, 1, &cstr, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info[512];
    glGetShaderInfoLog(shader, sizeof(info), nullptr, info);
    LOG_ERROR("Failed to compile {} shader: {}", type == GL_VERTEX_SHADER ? "vertex" : "fragment",
              info);
    return false;
  }

  return true;
}

bool astri::gl::Shader::link(std::vector<GLuint> shaders, GLuint &program) {
  LOG_ZONE();

  program = glCreateProgram();
  for (const auto &shader : shaders) {
    glAttachShader(program, shader);
  }
  glLinkProgram(program);

  for (const auto &shader : shaders) {
    glDeleteShader(shader);
  }

  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char info[512];
    glGetProgramInfoLog(program, sizeof(info), nullptr, info);
    LOG_ERROR("Failed to link shader program: {}", info);
    return false;
  }

  return true;
}
