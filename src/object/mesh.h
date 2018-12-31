#pragma once

#include "tinyobjloader/tiny_obj_loader.h"
#include "object.h"

class Mesh : public Object {
  private:
    virtual Hit intersects(
      const Ray &ray,
      const glm::vec3 &v0,
      const glm::vec3 &v1,
      const glm::vec3 &v2,
      const glm::vec3 &n0,
      const glm::vec3 &n1,
      const glm::vec3 &n2,
      bool cull = true);

  public:
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    virtual Hit intersects(const Ray &ray);

    void init(const char* file);
};
