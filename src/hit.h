#pragma once

#include <glm/glm.hpp>

class Hit
{
  public:
    float distance;
    glm::vec3 normal;

    Hit(float distance, glm::vec3 normal);
};
