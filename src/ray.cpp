#include "ray.h"

Ray::Ray() {}

Ray::Ray(glm::vec3 direction, glm::vec3 position) : direction(direction), position(position) {}
