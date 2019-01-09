#include "material.h"

bool Material::isDiffuse() {
  return diffuse != glm::vec3(0);
}

glm::vec3 Material::sampleWorld(const glm::vec3 &direction, const glm::vec3 &normal) const {
	// Assumes direction.z is oriented towards the "top of the lobe".
  glm::vec3 w = normal;
	glm::vec3 u = glm::normalize(glm::vec3(normal.z, 0, -normal.x));
	glm::vec3 v = glm::normalize(glm::cross(w, u));
	glm::vec3 d = direction.x * u + direction.y * v + direction.z * w;
  return d;
}
