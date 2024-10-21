#include "gl/gl.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "gl/camera.hpp"
#include "gl/object.hpp"
#include "gl/shader.hpp"
#include "log.hpp"

#ifndef __EMSCRIPTEN__
#  include "fragment.h"
#  include "vertex.h"
#endif // !__EMSCRIPTEN__

namespace astri::gl {
  std::string GLSL_VERSION;
} // namespace astri::gl

static astri::gl::Shader *shader = nullptr;
static astri::gl::Object *obj = nullptr;
static astri::gl::Camera *camera = nullptr;

void glfw_callback(int error, const char *description) {
  LOG_ERROR("GLFW Error {}: {}", error, description);
}

GLFWwindow *astri::gl::initialize(uint32_t width, uint32_t height) {
  LOG_ZONE();

  LOG_INFO("GLFW v{}", glfwGetVersionString());

  glfwSetErrorCallback(glfw_callback);
  if (!glfwInit()) {
    LOG_CRITICAL("Failed to initialize GLFW");
    return nullptr;
  }

#ifdef __EMSCRIPTEN__
  GLSL_VERSION = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  LOG_INFO("OpenGL ES v{} + GLSL {}", "2.0", 100);
#else
  GLSL_VERSION = "#version 330";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  LOG_INFO("OpenGL v{} + GLSL {}", "3.3", 150);
#endif // __EMSCRIPTEN__

  GLFWwindow *window = glfwCreateWindow(width, height, "Astri", nullptr, nullptr);
  if (!window) {
    LOG_CRITICAL("Failed to create GLFW window");
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);

#ifndef __EMSCRIPTEN__
  glfwSwapInterval(1);
#endif // !__EMSCRIPTEN__

#ifdef __EMSCRIPTEN__
  if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
    LOG_CRITICAL("GLAD failed to load OpenGL ES");
    glfwDestroyWindow(window);
    glfwTerminate();
    return nullptr;
  }
#else
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG_CRITICAL("GLAD failed to load OpenGL");
    glfwDestroyWindow(window);
    glfwTerminate();
    return nullptr;
  }
#endif // __EMSCRIPTEN__

  glEnable(GL_CULL_FACE);

  shader = new astri::gl::Shader();
  shader->add_vertex(vertex_compressed_data_base85);
  shader->add_fragment(fragment_compressed_data_base85);
  shader->link();
  if (!shader->valid()) {
    LOG_CRITICAL("Failed to compile shader");
    return nullptr;
  }

  camera = new astri::gl::Camera();

  shader->use();
  shader->set("projection", camera->projection(width, height));
  shader->set("sunPos", glm::vec3(10.0f, 0.0f, 2.0f));
  LOG_INFO("Projection matrix: {}", glm::to_string(camera->projection(width, height)));

  obj = new astri::gl::Object(astri::gl::Object::icosphere(3));

  return window;
}

void astri::gl::terminate(GLFWwindow *window) {
  LOG_ZONE();

  if (window)
    glfwDestroyWindow(window);
  glfwTerminate();
}

void astri::gl::startup(GLFWwindow *window) {}

void astri::gl::frame(GLFWwindow *window) {
  LOG_ZONE();

  shader->use();
  shader->set("view", camera->view());
  obj->draw(shader);
}
