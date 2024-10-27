#ifndef INCLUDE_GL_OBJECT_HPP_
#define INCLUDE_GL_OBJECT_HPP_

#include <vector>

#include <glm/glm.hpp>

#include "gl/shader.hpp"

namespace astri::gl {
  struct Vertex {
    Vertex(float x, float y, float z) : position(x, y, z) {}
    Vertex(const glm::vec3 &position, const glm::vec3 &normal)
        : position(position), normal(normal) {}

    operator glm::vec3() const { return position; }

    glm::vec3 position, normal;
  };

  class Object {
  public:
    Object(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
    Object(const std::vector<glm::vec3> &vertices, const std::vector<unsigned int> &indices);

    void transform(const glm::mat4 &transform) { model_ = transform; }
    void add_instance(const glm::mat4 &instance, const glm::vec4 &color);

    static Object square();
    static Object icosphere(std::uint8_t subdivide = 0);

    void draw(Shader *shader) const;

  protected:
    void build();

    unsigned int vao, vbo, nbo, ebo, ivbo, cvbo;
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;

    glm::mat4 model_ = glm::mat4(1.0f);

    std::vector<glm::mat4> instance_models_ = {};
    std::vector<glm::vec4> instance_colors_ = {};
  };
} // namespace astri::gl

#endif // INCLUDE_GL_OBJECT_HPP_
