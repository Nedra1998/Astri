#include "gl/shader.hpp"

#include "log.hpp"

astri::Shader::Shader(std::filesystem::path vertex_path, std::filesystem::path fragment_path) {
  // Read the vertex and fragment shader sources to strings
  std::string vertex_source, fragment_source;

  if (!std::filesystem::exists(vertex_path)) {
    LOG_ERROR("Vertex shader file not found: {}", vertex_path.string());
    valid_ = false;
    return;
  }

  std::ifstream vertex_file(vertex_path);
  if (!vertex_file.is_open()) {
    LOG_ERROR("Failed to open vertex shader file: {}", vertex_path.string());
    valid_ = false;
    return;
  }

  vertex_source =
      std::string(std::istreambuf_iterator<char>(vertex_file), std::istreambuf_iterator<char>());

  if (!std::filesystem::exists(fragment_path)) {
    LOG_ERROR("Fragment shader file not found: {}", fragment_path.string());
    valid_ = false;
    return;
  }

  std::ifstream fragment_file(fragment_path);
  if (!fragment_file.is_open()) {
    LOG_ERROR("Failed to open fragment shader file: {}", fragment_path.string());
    valid_ = false;
    return;
  }

  fragment_source =
      std::string(std::istreambuf_iterator<char>(fragment_file), std::istreambuf_iterator<char>());

  GLuint vertex_shader, fragment_shader;
  if (!compile_shader(vertex_source.data(), GL_VERTEX_SHADER, vertex_shader) ||
      !compile_shader(fragment_source.data(), GL_FRAGMENT_SHADER, fragment_shader) ||
      !link_program(vertex_shader, fragment_shader, program_)) {
    valid_ = false;
  } else {
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    valid_ = true;
  }
}

astri::Shader::Shader(const std::string &vertex_source, const std::string &fragment_source) {
  GLuint vertex_shader, fragment_shader;
  if (!compile_shader(vertex_source.data(), GL_VERTEX_SHADER, vertex_shader) ||
      !compile_shader(fragment_source.data(), GL_FRAGMENT_SHADER, fragment_shader) ||
      !link_program(vertex_shader, fragment_shader, program_)) {
    valid_ = false;
  } else {
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    valid_ = true;
  }
}

bool astri::Shader::compile_shader(const char *source, GLenum type, GLuint &shader) {
  shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
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
  return true;
}

bool astri::Shader::link_program(GLuint vertex, GLuint fragment, GLuint &program) {
  program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);

  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar info_log[512];
    glGetProgramInfoLog(program, sizeof(info_log), nullptr, info_log);
    LOG_ERROR("Failed to link shader program: {}", info_log);
    return false;
  }

  return true;
}
