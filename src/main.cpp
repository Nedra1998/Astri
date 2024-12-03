
#include <iostream>
#include <random>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "core/client.hpp"
#include "ecs.hpp"
#include "fragment.h"
#include "gl/camera.hpp"
#include "gl/gl.hpp"
#include "gl/object.hpp"
#include "gl/shader.hpp"
#include "log.hpp"
#include "ui/ui.hpp"
#include "version.hpp"
#include "vertex.h"

#ifdef TRACY_ENABLE
void *operator new(std ::size_t count) {
  auto ptr = malloc(count);
  TracyAlloc(ptr, count);
  return ptr;
}
void operator delete(void *ptr) noexcept {
  TracyFree(ptr);
  free(ptr);
}
#endif

int main(int argc, char *argv[]) {
  APP_INFO("Version: v{}", ASTRI_VERSION);
  APP_INFO("Commit: {}", ASTRI_VERSION_COMMIT);
  APP_INFO("Build Type: {}", ASTRI_BUILD_TYPE);
  APP_INFO("Build Date: {}", __DATE__);

  if (!astri::log::initialize()) {
    std::cerr << "Failed to initialize logging" << std::endl;
    return -1;
  }

  LOG_INFO("Astri v{} ({})", ASTRI_VERSION, ASTRI_VERSION_COMMIT);

  if (!astri::ecs::initialize()) {
    LOG_CRITICAL("Failed to initialize ECS");
    return 1;
  }

  GLFWwindow *window = astri::gl::initialize(800, 600);
  if (!window) {
    LOG_CRITICAL("Failed to initialize OpenGL");
    return 1;
  }

  if (!astri::ui::initialize(window)) {
    LOG_CRITICAL("Failed to initialize UI");
    return 1;
  }

  astri::core::Client client("_cache");
  client.load();

  astri::gl::Camera camera;

  std::shared_ptr<astri::gl::Shader> shader = std::make_shared<astri::gl::Shader>(
      vertex_compressed_data_base85, fragment_compressed_data_base85);
  if (!shader->is_valid()) {
    LOG_CRITICAL("Failed to compile required shader");
    return 1;
  }

  shader->set("sunPos", glm::vec3(0.0f, 0.0f, 0.0f));
  shader->set("ambient", glm::vec3(0.05f));
  shader->set("projection", camera.projection(800, 600));

  astri::gl::Object object = astri::gl::Object::Icosphere(1.0, 0);
  object.shader(shader);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(-50.0f, 50.0f);
  std::uniform_real_distribution<float> cdis(0.0f, 1.0f);
  std::uniform_real_distribution<float> sdist(-0.01f, 0.01f);

  std::size_t i = 0;
  astri::ecs::WORLD.query<astri::GeneralPertubations>().each(
      [&](const astri::GeneralPertubations &gp) {
        object.instance(i++,
                        glm::translate(glm::mat4(1.0), glm::vec3(dis(gen), dis(gen), dis(gen))),
                        glm::vec4(cdis(gen), cdis(gen), cdis(gen), 1.0f));
      });

  glClearColor(0.0, 0.0, 0.0, 1.0);
  static double last_time = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {

    glfwPollEvents();
    GPU_ZONE("Render");

    // Orbit the camera around the origin based on the key pressed
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      camera.orbit(-0.05f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      camera.orbit(0.05f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      camera.orbit(0.0f, 0.05f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      camera.orbit(0.0f, -0.05f);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
      camera.orbit(0.0f, 0.0f, -0.1f);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
      camera.orbit(0.0f, 0.0f, 0.1);

    i = 0;
    astri::ecs::WORLD.query<astri::GeneralPertubations>().each(
        [&](const astri::GeneralPertubations &gp) {
          const auto model = object.instance(i);
          object.instance(i, glm::translate(model, glm::vec3(sdist(gen), sdist(gen), sdist(gen))));
          ++i;
          //           glm::vec4(cdis(gen), cdis(gen), cdis(gen), 1.0f));
          // object.instance(i++,
          //                 glm::translate(glm::mat4(1.0), glm::vec3(dis(gen), dis(gen),
          //                 dis(gen))), glm::vec4(cdis(gen), cdis(gen), cdis(gen), 1.0f));
        });

    // // Move the camera left and right based on the key pressed
    // if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    //   camera.position.x -= 0.1f;
    // if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    //   camera.position.x += 0.1f;
    // if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    //   camera.position.y += 0.1f;
    // if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    //   camera.position.y -= 0.1f;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->set("projection", camera.projection(width, height));
    shader->set("view", camera.view());

    object.draw();

    // double current_time = glfwGetTime();
    // astri::ecs::WORLD.progress(current_time - last_time);
    // last_time = current_time;

    astri::ui::frame(window);

    glfwSwapBuffers(window);

    TracyGpuCollect;
    MARK_FRAME();
  }

  astri::ui::terminate();
  astri::gl::terminate(window);

  return 0;
}
