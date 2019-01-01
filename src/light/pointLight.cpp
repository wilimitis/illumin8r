#include <glm/ext.hpp>
#include "pointLight.h"

Ray PointLight::sampleDirection() const {
  // TODO: Implement Shirley 14.4.1
  // http://mathworld.wolfram.com/SpherePointPicking.html
  return Ray(glm::sphericalRand(1.0f), position);
}
