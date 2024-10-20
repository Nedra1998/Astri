#include "gl/shader.hpp"

#include "gl/gl.hpp"
#include "log.hpp"
#include "util/asset.hpp"

astri::gl::Shader::~Shader() {
  LOG_ZONE();

  if (id_) {
    glDeleteProgram(id_);
  }

  for (const auto &[type, shader] : shaders_) {
    glDeleteShader(shader);
  }
}

bool astri::gl::Shader::add_shader(const char *source, GLenum type) {
  LOG_ZONE();

  char *decoded = nullptr;
  std::string code = astri::util::decode85(source);
  if (code.empty()) {
    LOG_ERROR("Failed to decode shader source");
    return false;
  }

  code.insert(0, fmt::format("{}\n", gl::GLSL_VERSION));

  const char *code_ptr = code.c_str();
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &code_ptr, nullptr);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar info_log[512];
    glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);
    LOG_ERROR("Failed to compile {} shader: {}", type == GL_VERTEX_SHADER ? "vertex" : "fragment",
              info_log);
    return false;
  }

  shaders_.emplace_back(type, shader);
  return true;
}

bool astri::gl::Shader::link() {
  LOG_ZONE();

  id_ = glCreateProgram();
  for (const auto &[type, shader] : shaders_) {
    glAttachShader(id_, shader);
  }
  glLinkProgram(id_);

  GLint success;
  glGetProgramiv(id_, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar info_log[512];
    glGetProgramInfoLog(id_, sizeof(info_log), nullptr, info_log);
    LOG_ERROR("Failed to link shader program: {}", info_log);
    return false;
  }

  for (const auto &[type, shader] : shaders_) {
    glDeleteShader(shader);
  }
  shaders_.clear();

  return true;
}
