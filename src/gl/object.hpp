#ifndef INCLUDE_GL_OBJECT_HPP_
#define INCLUDE_GL_OBJECT_HPP_

#include <memory>

#include "gl/shader.hpp"

namespace astri::gl {
  struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
  };

  class Object {
  public:
    Object(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
    ~Object();

    inline std::shared_ptr<Shader> shader() const { return shader_; }
    inline void shader(std::shared_ptr<Shader> shader) { shader_ = shader; }

    inline glm::mat4 instance(std::size_t index) { return instance_models_[index]; }
    inline void instance(std::size_t index, const glm::mat4 &model) {
      instance(index, model, instance_colors_[index]);
    }
    void instance(std::size_t index, const glm::mat4 &model, const glm::vec4 &color);
    void instances(const std::vector<glm::mat4> &models, const std::vector<glm::vec4> &colors);

    void draw();

    static Object Square(float width = 1.0f, float height = 1.0f);
    static Object Cube(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
    static Object Icosphere(float radius = 1.0f, std::uint8_t subdivisions = 1);

  protected:
    void update_instances();

    unsigned int vao_ = 0, vbo_ = 0, ebo_ = 0;
    unsigned int instanced_model_vbo_ = 0, instanced_color_vbo_ = 0, instance_count_ = 0;
    unsigned int index_count_ = 0;
    bool ditry_instances_ = false;

    std::shared_ptr<Shader> shader_;
    std::vector<glm::mat4> instance_models_;
    std::vector<glm::vec4> instance_colors_;
  };
} // namespace astri::gl

#endif // INCLUDE_GL_OBJECT_HPP_
