#pragma once

#include <vector>
#include "camera.h"
#include "image.h"
#include "object.h"

void Render(
  Camera camera,
  Image image,
  std::vector<Object*> objects
);