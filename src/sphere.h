#pragma once

#include "object.h"

class Sphere : public Object
{
  public:
    virtual Hit intersects(const Ray &ray);
};
