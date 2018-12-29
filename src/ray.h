#pragma once

#include <glm/glm.hpp>

class Ray {
  public:
    glm::vec3 direction;
    glm::vec3 position;

    Ray();
    Ray(const glm::vec3 &direction, const glm::vec3 &position);
};
