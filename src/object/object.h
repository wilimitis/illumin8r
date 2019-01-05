#pragma once

#include "glm/glm.hpp"
#include "box.h"
#include "surface.h"

class Object : public Surface {
  public:
    std::string key;
    glm::mat4 localTransformation;

    Object();

    // TODO: Precompute.
    virtual Box computeBox() = 0;

    Ray getLocal(const Ray &ray);
    Ray getWorld(const Ray &ray);
    void rotate(float angle, const glm::vec3 &axis);
    void scale(const glm::vec3 &scale);
    void translate(const glm::vec3 &translation);
};
