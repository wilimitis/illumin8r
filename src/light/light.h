#pragma once

#include <glm/glm.hpp>
#include "../ray.h"

class Light {
  public:
    float area;
    glm::vec3 intensity;
    glm::vec3 position;
    glm::vec3 power;

    virtual Ray sampleDirection() const = 0;
};
