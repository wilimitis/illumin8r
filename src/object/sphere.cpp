#include <algorithm>
#include <glm/glm.hpp>
#include "sphere.h"

Box Sphere::computeBox() {
	Box b;
	b.min = glm::vec3(-1);
	b.max = glm::vec3(1);
	return b;
}

Hit Sphere::intersects(const Ray &ray) {
	Hit h;

  glm::vec3 o = glm::vec3(0);
	float r = 1;
	float b = glm::dot(ray.direction, ray.position);
	float a = glm::dot(ray.direction, ray.direction);
	float c = glm::dot(ray.position, ray.position) - r * r;
	float d = b * b - a * c;
	if (d < 0) {
		return h;
	}

	d = sqrtf(d);
	float nddp = glm::dot(-ray.direction, ray.position);
	float t0 = (nddp - d) / a;
	float t1 = (nddp + d) / a;
	if (t0 < 0 && t1 < 0) {
		return h;
	}
	
	if (t0 == t1 || (t0 > 0 && t1 > 0)) {
		h.distance = std::min(t0, t1);	
	} else {
		h.distance = std::max(t0, t1);
	}
	h.position = ray.position + h.distance * ray.direction;
	h.normal = glm::normalize(h.position - o);
	h.isEmpty = false;
	return h;
}
