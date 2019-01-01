#include "material.h"

bool Material::isSpecular() {
  return specular != glm::vec3(0);
}
