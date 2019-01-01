#pragma once

#include <glm/glm.hpp>
#include "material/material.h"

class Hit {
  public:
    float distance;
    bool isEmpty;
    Material* material;
    glm::vec3 normal;
    glm::vec3 position;

    Hit();
};
