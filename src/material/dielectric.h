#pragma once

#include "material.h"

class Dielectric : public Material {
  public:
    virtual Sample sampleDiffuse(const glm::vec3 &ki, const Hit &hit) const;
    virtual Sample sampleSpecular(const glm::vec3 &ki, const Hit &hit) const;
    virtual Sample sampleRefractive(const glm::vec3 &ki, const Hit &hit) const;
};
