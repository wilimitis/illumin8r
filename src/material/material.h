#pragma once

#include "glm/glm.hpp"
#include <string>

class Material {
  public:
    std::string key;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float refractiveIndex;

    virtual glm::vec3 sampleDiffuseDirection() const = 0;
    virtual glm::vec3 sampleSpecularDirection() const = 0;

    bool isSpecular();
};
