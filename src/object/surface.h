#pragma once

#include "../hit.h"
#include "../ray.h"

class Surface {
  public:
    virtual Hit intersects(const Ray &ray) = 0;
};
