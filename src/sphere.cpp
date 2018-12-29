#include <glm/glm.hpp>
#include "sphere.h"

bool Sphere::intersects(const Ray &ray)
{
  glm::vec3 o = glm::vec3(0);
	float r = 1;
	float b = glm::dot(ray.direction, ray.position);
	float a = glm::dot(ray.direction, ray.direction);
	float c = glm::dot(ray.position, ray.position) - r * r;
	float d = b * b - a * c;
	if (d < 0) {
		return false;
	}

	d = sqrtf(d);
	float nddp = glm::dot(-ray.direction, ray.position);
	float t0 = (nddp - d) / a;
	float t1 = (nddp + d) / a;
	if (t0 < 0 && t1 < 0) {
		return false;
	}
	return true;
}
