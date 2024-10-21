#ifndef INCLUDE_GL_CAMERA_HPP_
#define INCLUDE_GL_CAMERA_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace astri::gl {
  class Camera {
  public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 15.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           glm::vec3 look_at = glm::vec3(0.0f, 0.0f, 0.0f), float fov = 45.0f)
        : position(position), up(up), look_at(look_at), fov(fov) {}

    inline glm::mat4 view() const { return glm::lookAt(position, look_at, up); }
    inline glm::mat4 projection(size_t width, size_t height, float near = 0.1f,
                                float far = 100.0f) const {
      return glm::perspective(glm::radians(fov), (float)width / (float)height, near, far);
    }

    float fov;
    glm::vec3 position, up, look_at;
  };
} // namespace astri::gl

#endif // INCLUDE_GL_CAMERA_HPP_
