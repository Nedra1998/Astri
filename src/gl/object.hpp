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

    static Object square();
    static Object icosphere(std::uint8_t subdivide = 0);

    void draw(Shader *shader) const;

  protected:
    void build();

    unsigned int vbo, ebo, vao, ibo;
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    std::vector<glm::mat4> instances_ = {glm::mat4(1.0f)};
    glm::mat4 model = glm::mat4(1.0f);
  };
} // namespace astri::gl

#endif // INCLUDE_GL_OBJECT_HPP_
