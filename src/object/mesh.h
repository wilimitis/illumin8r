#pragma once

#include "tinyobjloader/tiny_obj_loader.h"
#include "../accelerationStructure/accelerationStructure.h"
#include "object.h"

class Mesh : public Object {
  private:
    class Triangle : public Object {
      public:
        static const bool cull = false;
        static const int vertexCount = 3;
        
        glm::vec3 vertices[vertexCount];
        glm::vec3 normals[vertexCount];

        Box computeBox();
        Hit intersects(const Ray &ray);
    };

    AccelerationStructure* accelerationStructure;
    std::vector<Object*> objects;

  public:
    Box computeBox();
    Hit intersects(const Ray &ray);
    
    void init(const char* file);
};
