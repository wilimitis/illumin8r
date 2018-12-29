#pragma once

#include <glm/glm.hpp>
#include "../ray.h"

class Light {
  public:
    glm::vec3 position;
    
    virtual Ray sampleDirection() = 0;
};
