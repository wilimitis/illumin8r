#pragma once

#include "glm/glm.hpp"
#include "../hit.h"
#include "../ray.h"

class Object {
  public:
    glm::mat4 localTransformation;

    virtual Hit intersects(const Ray &ray) = 0;

    Object();

    Ray getLocal(const Ray &ray);
    Ray getWorld(const Ray &ray);
    void rotate(float angle, const glm::vec3 &axis);
    void scale(const glm::vec3 &scale);
    void translate(const glm::vec3 &translation);
};
