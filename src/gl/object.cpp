#include "gl/object.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <cstdint>
#include <unordered_map>

#include <glm/gtx/string_cast.hpp>

#include "gl/gl.hpp"
#include "gl/shader.hpp"
#include "log.hpp"

using namespace glm;

astri::gl::Object::Object(const std::vector<Vertex> &vertices,
                          const std::vector<unsigned int> &indices)
    : vao(0), vbo(0), nbo(0), ebo(0), ivbo(0), cvbo(0), vertices_(vertices), indices_(indices) {
  build();
}

astri::gl::Object::Object(const std::vector<vec3> &vertices,
                          const std::vector<unsigned int> &indices) {
  for (size_t i = 0; i < indices.size(); i += 3) {
    vec3 a = vertices[indices[i]];
    vec3 b = vertices[indices[i + 1]];
    vec3 c = vertices[indices[i + 2]];
    vec3 normal = normalize(cross(b - a, c - a));

    size_t vindex = vertices_.size();
    vertices_.emplace_back(a, normal);
    vertices_.emplace_back(b, normal);
    vertices_.emplace_back(c, normal);
    indices_.push_back(vindex);
    indices_.push_back(vindex + 1);
    indices_.push_back(vindex + 2);
  }

  build();
}

void astri::gl::Object::add_instance(const glm::mat4 &instance, const glm::vec4 &color) {
  LOG_ZONE();

  instance_models_.push_back(instance);
  instance_colors_.push_back(color);

  glBindVertexArray(vao);

  // clang-format off
  if (!ivbo)
    glGenBuffers(1, &ivbo);
  glBindBuffer(GL_ARRAY_BUFFER, ivbo);
  glBufferData(GL_ARRAY_BUFFER, instance_models_.size() * sizeof(glm::mat4), instance_models_.data(), GL_STATIC_DRAW);

  for(std::uint8_t i = 0; i < 4; ++i) {
    glEnableVertexAttribArray(2 + i);
    glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
    glVertexAttribDivisor(2 + i, 1);
  }

  if(!cvbo)
    glGenBuffers(1, &cvbo);
  glBindBuffer(GL_ARRAY_BUFFER, cvbo);
  glBufferData(GL_ARRAY_BUFFER, instance_colors_.size() * sizeof(glm::vec4), instance_colors_.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
  glVertexAttribDivisor(6, 1);
  // clang-format on

  glBindVertexArray(0);
}

void astri::gl::Object::build() {
  LOG_ZONE();

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // clang-format off
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);
  // clang-format on

  glBindVertexArray(0);
}

void astri::gl::Object::draw(Shader *shader) const {
  shader->use();

  glBindVertexArray(vao);
  if (instance_models_.empty()) {
    shader->set("model", model_);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
  } else {
    glDrawElementsInstanced(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0,
                            instance_models_.size());
  }
  glBindVertexArray(0);
}

astri::gl::Object astri::gl::Object::square() {
  LOG_ZONE();

  return Object(
      std::vector<vec3>{
          vec3(-0.5f, -0.5f, 0.0f),
          vec3(0.5f, -0.5f, 0.0f),
          vec3(0.5f, 0.5f, 0.0f),
          vec3(-0.5f, 0.5f, 0.0f),
      },
      {0, 1, 2, 2, 3, 0});
}

astri::gl::Object astri::gl::Object::icosphere(std::uint8_t subdivisions) {
  LOG_ZONE();

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

  const auto add_vertex = [](std::vector<vec3> &vertices, const vec3 &v) -> int {
    vertices.push_back(glm::normalize(v));
    return static_cast<unsigned int>(vertices.size() - 1);
  };

  const auto get_middle_point =
      [&add_vertex](unsigned int p1, unsigned int p2, std::vector<vec3> &vertices,
                    std::unordered_map<int64_t, unsigned int> &cache) -> int {
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
