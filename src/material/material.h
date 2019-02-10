#pragma once

#include "glm/glm.hpp"
#include <string>
#include "../hit.h"

class Hit;

class Material {
  public:
    class Sample {
      public:
        enum Type { 
          diffuse,
          specular
        };

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
    bool isPure;

    glm::vec3 sampleWorld(const glm::vec3 &direction, const glm::vec3 &up) const;
    
    virtual glm::vec3 brdf(const glm::vec3 &wo, const glm::vec3 &wi, const Hit &hit) const = 0;
    virtual float pdf(const glm::vec3 &wo, const glm::vec3 &wi, const Hit &hit) const = 0;
    virtual Sample sample(const glm::vec3 &wo, const Hit &hit, const Sample::Type &type) const = 0;
    virtual Sample sampleRefractive(const glm::vec3 &wo, const Hit &hit) const = 0;
    
    bool isDiffuse();
    bool isSpecular();
};
