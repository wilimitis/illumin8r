#pragma once

#include <glm/glm.hpp>

class Camera {
  public:
    glm::vec3 direction;
    float fieldOfView;
    glm::vec3 position;
    glm::vec3 up;
};
