#pragma once

#include <vector>
#include "camera.h"
#include "image.h"
#include "light/light.h"
#include "object/object.h"

Hit cast(const Ray &ray, const std::vector<Object*> &objects);

void Render(
  Camera camera,
  Image image,
  const std::vector<Light*> &lights,
  const std::vector<Object*> &objects
);
