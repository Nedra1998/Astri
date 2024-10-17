#ifndef INCLUDE_GL_OBJECT_HPP_
#define INCLUDE_GL_OBJECT_HPP_

#include <vector>

#include <glm/glm.hpp>

namespace astri::gl {
  struct Vertex {
    Vertex(float x, float y, float z) : position(x, y, z) {}
    Vertex(const glm::vec3 &position) : position(position) {}

    operator glm::vec3() const { return position; }

    glm::vec3 position;
  };

  class Object {
  public:
    Object(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
    Object(const std::vector<glm::vec3> &vertices, const std::vector<unsigned int> &indices)
        : Object(std::vector<Vertex>(vertices.begin(), vertices.end()), indices) {}

    void draw() const;

    static Object icosphere(std::uint8_t subdivide = 0);

  protected:
    unsigned int vbo, ebo, vao;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<glm::mat4> instances;
  };
} // namespace astri::gl

#endif // INCLUDE_GL_OBJECT_HPP_
