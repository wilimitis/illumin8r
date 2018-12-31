#pragma once

#include "tinyobjloader/tiny_obj_loader.h"
#include "object.h"

class Mesh : public Object {
  private:
    class Triangle : public Object {
      public:
        static const bool cull = true;
        static const int vertexCount = 3;
        
        glm::vec3 verticies[vertexCount];
        glm::vec3 normals[vertexCount];

        Hit intersects(const Ray &ray);
    };

    std::vector<Object*> objects;

  public:
    void init(const char* file);
    Hit intersects(const Ray &ray);
};
