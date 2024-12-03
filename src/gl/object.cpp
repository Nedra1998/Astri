#include "gl/object.hpp"

#include "log.hpp"

astri::gl::Object::Object(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
    : index_count_(indices.size()) {
  LOG_ZONE();

  // clang-format off

  // Generate and bind Vertex Array Object (VAO)
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  // Generate and bind Vertex Buffer Object (VBO)
  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

  // Generate and bind Element Buffer Object (EBO) for indices
  glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

  // Set vertex attribute pointers
  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(0);

  // Normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  // clang-format on
}

astri::gl::Object::~Object() {
  LOG_ZONE();

  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
  if (instanced_model_vbo_ != 0)
    glDeleteBuffers(1, &instanced_model_vbo_);
  if (instanced_color_vbo_ != 0)
    glDeleteBuffers(1, &instanced_color_vbo_);
  glDeleteVertexArrays(1, &vao_);
}

void astri::gl::Object::instance(std::size_t index, const glm::mat4 &model,
                                 const glm::vec4 &color) {
  LOG_ZONE();

  ditry_instances_ = true;
  if (index < instance_count_) {
    instance_models_[index] = model;
    instance_colors_[index] = color;
  } else {
    instance_count_++;
    instance_models_.push_back(model);
    instance_colors_.push_back(color);
  }
}

void astri::gl::Object::instances(const std::vector<glm::mat4> &models,
                                  const std::vector<glm::vec4> &colors) {
  LOG_ZONE();

  ditry_instances_ = true;
  instance_count_ = instance_models_.size();
  instance_models_ = models;
  instance_colors_ = colors;
}

void astri::gl::Object::draw() {
  LOG_ZONE();
  if (shader_)
    shader_->use();

  if (ditry_instances_) {
    update_instances();
    ditry_instances_ = false;
  }

  glBindVertexArray(vao_);
  glLineWidth(3);
  glPolygonMode(GL_FRONT, GL_LINE);
  if (instance_count_ != 0)
    glDrawElementsInstanced(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, 0, instance_count_);
  else
    glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
}

void astri::gl::Object::update_instances() {
  LOG_ZONE();

  instance_count_ = instance_models_.size();

  if (instanced_model_vbo_ == 0) {
    glGenBuffers(1, &instanced_model_vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, instanced_model_vbo_);
    glBufferData(GL_ARRAY_BUFFER, instance_models_.size() * sizeof(glm::mat4),
                 instance_models_.data(), GL_STATIC_DRAW);
    glBindVertexArray(vao_);
    for (unsigned int i = 0; i < 4; i++) {
      glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                            (void *)(sizeof(glm::vec4) * i));
      glEnableVertexAttribArray(2 + i);
      glVertexAttribDivisor(2 + i, 1);
    }
    glBindVertexArray(0);
  } else {
    glBindBuffer(GL_ARRAY_BUFFER, instanced_model_vbo_);
    glBufferData(GL_ARRAY_BUFFER, instance_models_.size() * sizeof(glm::mat4),
                 instance_models_.data(), GL_STATIC_DRAW);
  }

  if (instanced_color_vbo_ == 0) {
    glGenBuffers(1, &instanced_color_vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, instanced_color_vbo_);
    glBufferData(GL_ARRAY_BUFFER, instance_colors_.size() * sizeof(glm::vec4),
                 instance_colors_.data(), GL_STATIC_DRAW);
    glBindVertexArray(vao_);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void *)0);
    glEnableVertexAttribArray(6);
    glVertexAttribDivisor(6, 1);
    glBindVertexArray(0);
  } else {
    glBindBuffer(GL_ARRAY_BUFFER, instanced_color_vbo_);
    glBufferData(GL_ARRAY_BUFFER, instance_colors_.size() * sizeof(glm::vec4),
                 instance_colors_.data(), GL_STATIC_DRAW);
  }
}

astri::gl::Object astri::gl::Object::Square(float width, float height) {
  LOG_ZONE();

  std::vector<Vertex> vertices = {
      Vertex{glm::vec3(-width / 2, -height / 2, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
      Vertex{glm::vec3(width / 2, -height / 2, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
      Vertex{glm::vec3(width / 2, height / 2, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
      Vertex{glm::vec3(-width / 2, height / 2, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
  };
  std::vector<uint32_t> indices = {
      0, 1, 2, 2, 3, 0,
  };

  return Object(vertices, indices);
}

astri::gl::Object astri::gl::Object::Cube(float width, float height, float depth) {
  LOG_ZONE();

  std::vector<Vertex> vertices = {
      Vertex{glm::vec3(-width / 2, -height / 2, -depth / 2), glm::vec3(0.0f, 0.0f, -1.0f)},
      Vertex{glm::vec3(width / 2, -height / 2, -depth / 2), glm::vec3(0.0f, 0.0f, -1.0f)},
      Vertex{glm::vec3(width / 2, height / 2, -depth / 2), glm::vec3(0.0f, 0.0f, -1.0f)},
      Vertex{glm::vec3(-width / 2, height / 2, -depth / 2), glm::vec3(0.0f, 0.0f, -1.0f)},
      Vertex{glm::vec3(-width / 2, -height / 2, depth / 2), glm::vec3(0.0f, 0.0f, 1.0f)},
      Vertex{glm::vec3(width / 2, -height / 2, depth / 2), glm::vec3(0.0f, 0.0f, 1.0f)},
      Vertex{glm::vec3(width / 2, height / 2, depth / 2), glm::vec3(0.0f, 0.0f, 1.0f)},
      Vertex{glm::vec3(-width / 2, height / 2, depth / 2), glm::vec3(0.0f, 0.0f, 1.0f)},
      Vertex{glm::vec3(-width / 2, -height / 2, -depth / 2), glm::vec3(-1.0f, 0.0f, 0.0f)},
      Vertex{glm::vec3(-width / 2, height / 2, -depth / 2), glm::vec3(-1.0f, 0.0f, 0.0f)},
      Vertex{glm::vec3(-width / 2, height / 2, depth / 2), glm::vec3(-1.0f, 0.0f, 0.0f)},
      Vertex{glm::vec3(-width / 2, -height / 2, depth / 2), glm::vec3(-1.0f, 0.0f, 0.0f)},
      Vertex{glm::vec3(width / 2, -height / 2, -depth / 2), glm::vec3(1.0f, 0.0f, 0.0f)},
      Vertex{glm::vec3(width / 2, height / 2, -depth / 2), glm::vec3(1.0f, 0.0f, 0.0f)},
      Vertex{glm::vec3(width / 2, height / 2, depth / 2), glm::vec3(1.0f, 0.0f, 0.0f)},
      Vertex{glm::vec3(width / 2, -height / 2, depth / 2), glm::vec3(1.0f, 0.0f, 0.0f)},
      Vertex{glm::vec3(-width / 2, -height / 2, -depth / 2), glm::vec3(0.0f, -1.0f, 0.0f)},
      Vertex{glm::vec3(-width / 2, -height / 2, depth / 2), glm::vec3(0.0f, -1.0f, 0.0f)},
      Vertex{glm::vec3(width / 2, -height / 2, depth / 2), glm::vec3(0.0f, -1.0f, 0.0f)},
      Vertex{glm::vec3(width / 2, -height / 2, -depth / 2), glm::vec3(0.0f, -1.0f, 0.0f)},
      Vertex{glm::vec3(-width / 2, height / 2, -depth / 2), glm::vec3(0.0f, 1.0f, 0.0f)},
      Vertex{glm::vec3(-width / 2, height / 2, depth / 2), glm::vec3(0.0f, 1.0f, 0.0f)},
      Vertex{glm::vec3(width / 2, height / 2, depth / 2), glm::vec3(0.0f, 1.0f, 0.0f)},
      Vertex{glm::vec3(width / 2, height / 2, -depth / 2), glm::vec3(0.0f, 1.0f, 0.0f)},
  };
  std::vector<uint32_t> indices = {
      0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
      12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20,
  };

  return Object(vertices, indices);
}

astri::gl::Object astri::gl::Object::Icosphere(float radius, std::uint8_t subdivisions) {
  LOG_ZONE();

  const float t = (1.0f + sqrt(5.0f)) / 2.0f;

  std::vector<glm::vec3> verticies = {
      glm::normalize(glm::vec3(-1, t, 0)),  glm::normalize(glm::vec3(1, t, 0)),
      glm::normalize(glm::vec3(-1, -t, 0)), glm::normalize(glm::vec3(1, -t, 0)),
      glm::normalize(glm::vec3(0, -1, t)),  glm::normalize(glm::vec3(0, 1, t)),
      glm::normalize(glm::vec3(0, -1, -t)), glm::normalize(glm::vec3(0, 1, -t)),
      glm::normalize(glm::vec3(t, 0, -1)),  glm::normalize(glm::vec3(t, 0, 1)),
      glm::normalize(glm::vec3(-t, 0, -1)), glm::normalize(glm::vec3(-t, 0, 1)),
  };

  std::vector<std::uint32_t> indices = {
      0, 11, 5,  0, 5,  1, 0, 1, 7, 0, 7,  10, 0, 10, 11, 1, 5, 9, 5, 11,
      4, 11, 10, 2, 10, 7, 6, 7, 1, 8, 3,  9,  4, 3,  4,  2, 3, 2, 6, 3,
      6, 8,  3,  8, 9,  4, 9, 5, 2, 4, 11, 6,  2, 10, 8,  6, 7, 9, 8, 1,
  };

  std::unordered_map<std::uint64_t, std::uint32_t> cache;

  const auto add_vert = [&](const glm::vec3 &v) {
    verticies.push_back(glm::normalize(v) * radius);
    return verticies.size() - 1;
  };

  const auto get_middle_point = [&](std::uint64_t p1, std::uint64_t p2) {
    if (p1 > p2)
      std::swap(p1, p2);
    const auto key = (p1 << 32) | p2;

    if (const auto it = cache.find(key); it != cache.end())
      return it->second;

    const glm::vec3 middle = (verticies[p1] + verticies[p2]) / 2.0f;
    std::uint32_t i = add_vert(middle);
    cache[key] = i;
    return i;
  };

  for (std::uint8_t i = 0; i < subdivisions; ++i) {
    std::vector<unsigned int> new_indices;
    for (std::size_t j = 0; j < indices.size(); j += 3) {
      const std::uint32_t a = get_middle_point(indices[j], indices[j + 1]);
      const std::uint32_t b = get_middle_point(indices[j + 1], indices[j + 2]);
      const std::uint32_t c = get_middle_point(indices[j + 2], indices[j]);

      new_indices.insert(new_indices.end(),
                         {indices[j], a, c, indices[j + 1], b, a, indices[j + 2], c, b, a, b, c});
    }
    indices = std::move(new_indices);
  }

  std::vector<Vertex> verticies_with_normals;
  std::vector<std::uint32_t> new_indices;
  for (std::size_t i = 0; i < indices.size(); i += 3) {
    const std::uint32_t i0 = indices[i];
    const std::uint32_t i1 = indices[i + 1];
    const std::uint32_t i2 = indices[i + 2];

    const glm::vec3 &v0 = verticies[i0];
    const glm::vec3 &v1 = verticies[i1];
    const glm::vec3 &v2 = verticies[i2];
    const glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

    const std::uint32_t idx = verticies_with_normals.size();
    verticies_with_normals.push_back({v0, normal});
    verticies_with_normals.push_back({v1, normal});
    verticies_with_normals.push_back({v2, normal});
    new_indices.push_back(idx);
    new_indices.push_back(idx + 1);
    new_indices.push_back(idx + 2);
  }

  return Object(verticies_with_normals, new_indices);
}
