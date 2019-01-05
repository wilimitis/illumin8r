#pragma once

#include "material.h"

class Phong : public Material {
  public:
    int lobe;

    virtual Sample sampleDiffuse(const glm::vec3 &ki, const glm::vec3 &normal) const;
    virtual Sample sampleSpecular(const glm::vec3 &ki, const glm::vec3 &normal) const;
};
