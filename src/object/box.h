#pragma once

#include "surface.h"

class Box : public Surface {
  public:
    glm::vec3 min;
    glm::vec3 max;
    
    Hit intersects(const Ray &ray);

    float computeMidpoint(int axis);
};
