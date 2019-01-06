#include "material.h"

bool Material::isDiffuse() {
  return specular == glm::vec3(0) && refractive == glm::vec3(0);
}

glm::vec3 Material::sampleWorld(const glm::vec3 &direction, const glm::vec3 &normal) const {
  glm::vec3 w = normal;
	glm::vec3 u = glm::normalize(glm::vec3(normal.z, 0, -normal.x));
	glm::vec3 v = glm::normalize(glm::cross(w, u));
  glm::vec3 d = glm::vec3(
		direction.x * u.x + direction.y * v.x + direction.z * w.x,
		direction.x * u.y + direction.y * v.y + direction.z * w.y,
		direction.x * u.z + direction.y * v.z + direction.z * w.z
	);
  return d;
}
