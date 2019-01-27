#pragma once

#include "glm/glm.hpp"
#include <string>
#include "../hit.h"

class Hit;

class Material {
  public:
    struct Sample {
      glm::vec3 brdf;
      glm::vec3 direction;
      float pdf;
    };

    std::string key;
    glm::vec3 diffuse;
    glm::vec3 specular;
    // TODO: Maybe move into Dielectric?
    glm::vec3 refractive;
    float refractiveIndex;

    glm::vec3 sampleWorld(const glm::vec3 &direction, const glm::vec3 &up) const;
    
    virtual Sample sampleDiffuse(const glm::vec3 &wo, const Hit &hit) const = 0;
    virtual Sample sampleSpecular(const glm::vec3 &wo, const Hit &hit) const = 0;
    virtual Sample sampleRefractive(const glm::vec3 &wo, const Hit &hit) const = 0;
    
    bool isDiffuse();
};
