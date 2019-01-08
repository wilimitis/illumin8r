#pragma once

#include "scene.h"

Hit cast(const Ray &ray, const std::vector<Object*> &objects);

void Render(Scene &scene);
