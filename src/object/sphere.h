#pragma once

#include "object.h"

class Sphere : public Object {
  public:
    Box computeBox();
    Hit intersects(const Ray &ray);
};
