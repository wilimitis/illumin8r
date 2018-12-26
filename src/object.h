#pragma once

#include "ray.h"

class Object
{
  public:
    virtual ~Object() {}
    virtual bool intersects(Ray ray) = 0;
};