#pragma once

#include <vector>
#include "camera.h"
#include "image.h"
#include "light/light.h"
#include "material/material.h"
#include "object/object.h"

class Scene {
  public:
    Camera camera;
    Image image;
    std::vector<Light*> lights;
    std::vector<Material*> materials;
    std::vector<Object*> objects;
};
