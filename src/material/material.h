#pragma once

#include "glm/glm.hpp"
#include <string>

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
    float refractiveIndex;

    glm::vec3 sampleWorld(const glm::vec3 &direction, const glm::vec3 &up) const;
    
    virtual Sample sampleDiffuse(const glm::vec3 &ki, const glm::vec3 &normal) const = 0;
    virtual Sample sampleSpecular(const glm::vec3 &ki, const glm::vec3 &normal) const = 0;
    
    bool isSpecular();
};
