#include <glm/ext.hpp>
#include "pointLight.h"

Ray PointLight::sampleDirection()
{
  // TODO: Implement Shirley 14.4.1 with r2 in [-1, 1] to avoid cos(acos(phi)).
  return Ray(glm::sphericalRand(1), position);
}
