#include "gl/object.hpp"

#include <unordered_map>

#include "gl/gl.hpp"

using namespace glm;

astri::gl::Object::Object(const std::vector<Vertex> &vertices,
                          const std::vector<unsigned int> &indices)
    : vertices(vertices), indices(indices) {
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)0);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(sizeof(glm::vec4)));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void *)(2 * sizeof(glm::vec4)));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void *)(3 * sizeof(glm::vec4)));

  // glVertexAttribDivisor(3, 1);
  // glVertexAttribDivisor(4, 1);
  // glVertexAttribDivisor(5, 1);
  // glVertexAttribDivisor(6, 1);

  glBindVertexArray(0);
}

void astri::gl::Object::draw() const {
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

int add_vertex(std::vector<vec3> &vertices, const vec3 &v) {
  vertices.push_back(glm::normalize(v));
  return static_cast<unsigned int>(vertices.size() - 1);
}

// Helper function to get a middle point between two vertices.
// Caches midpoints to avoid duplicate vertices.
int get_middle_point(unsigned int p1, unsigned int p2, std::vector<vec3> &vertices,
                     std::unordered_map<int64_t, unsigned int> &cache) {
  // Sort indices to ensure uniqueness
  int64_t smaller = std::min(p1, p2);
  int64_t greater = std::max(p1, p2);
  int64_t key = (smaller << 32) + greater;

  // Check if we've already calculated this midpoint
  if (cache.find(key) != cache.end()) {
    return cache[key];
  }

  // Calculate midpoint
  vec3 midpoint = (vertices[p1] + vertices[p2]) * 0.5f;
  int idx = add_vertex(vertices, midpoint);

  // Store it in cache
  cache[key] = idx;
  return idx;
}

astri::gl::Object astri::gl::Object::icosphere(std::uint8_t subdivisions) {

  const float t = (1.0f + sqrt(5.0f)) / 2.0f;

  std::vector<vec3> vertices = {
      normalize(vec3(-1, t, 0)),  normalize(vec3(1, t, 0)),   normalize(vec3(-1, -t, 0)),
      normalize(vec3(1, -t, 0)),  normalize(vec3(0, -1, t)),  normalize(vec3(0, 1, t)),
      normalize(vec3(0, -1, -t)), normalize(vec3(0, 1, -t)),  normalize(vec3(t, 0, -1)),
      normalize(vec3(t, 0, 1)),   normalize(vec3(-t, 0, -1)), normalize(vec3(-t, 0, 1)),
  };

  std::vector<unsigned int> indices = {
      0, 11, 5,  0, 5,  1, 0, 1, 7, 0, 7,  10, 0, 10, 11, 1, 5, 9, 5, 11,
      4, 11, 10, 2, 10, 7, 6, 7, 1, 8, 3,  9,  4, 3,  4,  2, 3, 2, 6, 3,
      6, 8,  3,  8, 9,  4, 9, 5, 2, 4, 11, 6,  2, 10, 8,  6, 7, 9, 8, 1,
  };

  std::unordered_map<int64_t, unsigned int> cache;
  for (int i = 0; i < subdivisions; ++i) {
    std::vector<unsigned int> new_indices;
    for (size_t j = 0; j < indices.size(); j += 3) {
      int v1 = indices[j];
      int v2 = indices[j + 1];
      int v3 = indices[j + 2];

      // Split each edge and create new points
      int a = get_middle_point(v1, v2, vertices, cache);
      int b = get_middle_point(v2, v3, vertices, cache);
      int c = get_middle_point(v3, v1, vertices, cache);

      // Create four new triangles
      new_indices.push_back(v1);
      new_indices.push_back(a);
      new_indices.push_back(c);
      new_indices.push_back(v2);
      new_indices.push_back(b);
      new_indices.push_back(a);
      new_indices.push_back(v3);
      new_indices.push_back(c);
      new_indices.push_back(b);
      new_indices.push_back(a);
      new_indices.push_back(b);
      new_indices.push_back(c);
    }
    indices = std::move(new_indices);
  }

  return astri::gl::Object(vertices, indices);
}
