#pragma once

#include <glm/glm.hpp>

class Hit {
  public:
    float distance;
    bool isEmpty;
    glm::vec3 normal;
    glm::vec3 position;

    Hit();
};
