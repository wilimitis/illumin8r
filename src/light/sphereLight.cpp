#include <glm/ext.hpp>
#include "sphereLight.h"

Ray SphereLight::sampleDirection() const {
  glm::vec3 direction = glm::sphericalRand(1.0f);
  return Ray(direction, position + radius * direction);
}
