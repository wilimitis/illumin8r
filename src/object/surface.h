#pragma once

#include "../hit.h"
#include "../material/material.h"
#include "../ray.h"

class Surface {
  public:
    Material* material;
    
    virtual Hit intersects(const Ray &ray) = 0;
};
