#pragma once

#include "material.h"

class Phong : public Material {
  public:
    int lobe;

    virtual glm::vec3 sampleDiffuseDirection() const;
    virtual glm::vec3 sampleSpecularDirection() const;
};
