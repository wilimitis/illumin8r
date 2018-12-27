#pragma once

#include <glm/glm.hpp>

class Ray
{
  public:
    glm::vec3 direction;
    glm::vec3 position;

    Ray(glm::vec3 direction, glm::vec3 position): direction(direction), position(position) {}
};
