
#include <iostream>

#include "core/client.hpp"
#include "ecs.hpp"
#include "gl/gl.hpp"
#include "log.hpp"
#include "ui/ui.hpp"
#include "version.hpp"

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

  glClearColor(0.2, 0.2, 0.2, 1.0);
  static double last_time = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {

    glfwPollEvents();
    GPU_ZONE("Render");

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
