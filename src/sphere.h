#pragma once

#include "object.h"

class Sphere : public Object
{
  public:
    virtual bool intersects(Ray ray);
};