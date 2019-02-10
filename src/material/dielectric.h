#pragma once

#include "material.h"

class Dielectric : public Material {
  public:
    virtual glm::vec3 brdf(const glm::vec3 &wo, const glm::vec3 &wi, const Hit &hit) const;
    virtual float pdf(const glm::vec3 &wo, const glm::vec3 &wi, const Hit &hit) const;
    virtual Sample sample(const glm::vec3 &wo, const Hit &hit, const Sample::Type &type) const;
    virtual Sample sampleRefractive(const glm::vec3 &wo, const Hit &hit) const;
};
