#pragma once

#include <vector>
#include "accelerationStructure.h"
#include "../object/box.h"

class BVH : public AccelerationStructure {
  private:
    Object* left;
    Object* right;
    Box box;

    Box combine(const Box &box1, const Box &box2);

  public:
    BVH(std::vector<Object*> objects, int axis = 0);

    Box computeBox();
    Hit intersects(const Ray &ray);
};
