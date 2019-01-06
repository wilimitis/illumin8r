#pragma once

#include <glm/glm.hpp>
#include "material/material.h"

class Hit {
  public:
    float distance;
    bool isEmpty;
    bool isInside;
    Material* material;
    glm::vec3 normal;
    std::string objectKey;
    glm::vec3 position;

    Hit();
};
