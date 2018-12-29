#pragma once

#include <vector>
#include "camera.h"
#include "image.h"
#include "light/light.h"
#include "object/object.h"

void Render(
  Camera camera,
  Image image,
  std::vector<Light*> lights,
  std::vector<Object*> objects
);
