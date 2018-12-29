#include "ray.h"

Ray::Ray() {}

Ray::Ray(const glm::vec3 &direction, const glm::vec3 &position)
  : direction(direction), position(position) {}
