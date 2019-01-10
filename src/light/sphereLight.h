#pragma once

#include "light.h"

class SphereLight : public Light {
  public:
    float radius;

    virtual Ray sampleDirection() const;
};
