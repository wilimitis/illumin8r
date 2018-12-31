#pragma once

#include "object.h"

class Sphere : public Object {
  public:
    Hit intersects(const Ray &ray);
};
