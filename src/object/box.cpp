#include "box.h"

Hit Box::intersects(const Ray &ray) {
  Hit h;

  // http://www.cs.utah.edu/~awilliam/box/box.pdf	
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	float dx = 1.0 / ray.direction.x;
	if (dx >= 0) {
		tmin = (min.x - ray.position.x) * dx;
		tmax = (max.x - ray.position.x) * dx;
	} else {
		tmin = (max.x - ray.position.x) * dx;
		tmax = (min.x - ray.position.x) * dx;
	}

	float dy = 1.0 / ray.direction.y;
	if (dy >= 0) {
		tymin = (min.y - ray.position.y) * dy;
		tymax = (max.y - ray.position.y) * dy;
	} else {
		tymin = (max.y - ray.position.y) * dy;
		tymax = (min.y - ray.position.y) * dy;
	}
	if (tmin > tymax || tymin > tmax) {
		return h;
	}
	if (tymin > tmin) {
		tmin = tymin;
	}
	if (tymax < tmax) {
		tmax = tymax;
	}

	float dz = 1.0 / ray.direction.z;
	if (dz >= 0) {
		tzmin = (min.z - ray.position.z) * dz;
		tzmax = (max.z - ray.position.z) * dz;
	} else {
		tzmin = (max.z - ray.position.z) * dz;
		tzmax = (min.z - ray.position.z) * dz;
	}
	if (tmin > tzmax || tzmin > tmax) {
		return h;
	}
	if (tzmin > tmin) {
		tmin = tzmin;
	}
	if (tzmax < tmax) {
		tmax = tzmax;
	}

	if(tmax > 0) {
    h.isEmpty = false;
  }
  return h;
}

float Box::computeMidpoint(int axis) {
	return min[axis] - max[axis] / 2;
}
