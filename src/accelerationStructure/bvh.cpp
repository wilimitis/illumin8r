#include <algorithm>
#include "bvh.h"

BVH::BVH(std::vector<Object*> objects, int axis) {
  // Shirley 12.3.2
  if (objects.size() == 1) {
    left = objects.at(0);
    right = NULL;
    box = objects.at(0)->computeBox();
  } else if (objects.size() == 2) {
    left = objects.at(0);
    right = objects.at(1);
    box = combine(left->computeBox(), right ->computeBox());
  } else {
    // Find the midpoint along the axis.
    float low = FLT_MAX, high = -FLT_MAX;
    for (int i = 0; i < objects.size(); i++) {
      float m = objects.at(i)->computeBox().computeMidpoint(axis);
      if (m < low) {
        low = m;
      }
      if (m > high) {
        high = m;
      }
    }
    float midpoint = (low + high) / 2;
    std::vector<Object*> lower, higher;
    for (int i = 0; i < objects.size(); i++) {
      float m = objects.at(i)->computeBox().computeMidpoint(axis);
      if (m < midpoint) {
        lower.push_back(objects.at(i));
      } else {
        higher.push_back(objects.at(i));
      }
    }
    left = lower.size() == 0 ? NULL : new BVH(lower, (axis + 1) % 3);
    right = higher.size() == 0 ? NULL : new BVH(higher, (axis + 1) % 3);
    if (left && right) {
      box = combine(left->computeBox(), right->computeBox());
    } else if (right) {
      box = right->computeBox();
    } else if (left) {
      box = left->computeBox();
    } else {
      assert(false);
    }
  }
}


Box BVH::combine(const Box &box1, const Box &box2) {
  Box b;
	b.min = glm::vec3(
		std::min({box1.min.x, box2.min.x}),
    std::min({box1.min.y, box2.min.y}),
    std::min({box1.min.z, box2.min.z}));
	b.max = glm::vec3(
		std::max({box1.max.x, box2.max.x}),
    std::max({box1.max.y, box2.max.y}),
    std::max({box1.max.z, box2.max.z}));
	return b;
}

Box BVH::computeBox() {
  return box;
}

Hit BVH::intersects(const Ray &ray) {
  Hit hit = box.intersects(ray);
  if (hit.isEmpty) {
    return hit;
  }
  hit.isEmpty = true;
  Hit leftHit, rightHit;
  if (left) {
    leftHit = left->intersects(ray);
  }
  if (right) {
    rightHit = right->intersects(ray);
  }
  if (!leftHit.isEmpty && !rightHit.isEmpty) {
    hit = leftHit.distance < rightHit.distance ? leftHit : rightHit;
    hit.isEmpty = false;
  } else if (!leftHit.isEmpty || !rightHit.isEmpty) {
    hit = leftHit.isEmpty ? rightHit : leftHit;
    hit.isEmpty = false;
  }
  return hit;
}
