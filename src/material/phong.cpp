#include <glm/ext.hpp>
#include "phong.h"

glm::vec3 Phong::sampleDiffuseDirection() const {
  glm::vec3 d = glm::sphericalRand(1.0f);
  if (d.y < 0) {
    d.y *= -1;
  }
  return d;
}

glm::vec3 Phong::sampleSpecularDirection() const {
  float r1 = glm::linearRand(0.0f, 1.0f);
  float r2 = glm::linearRand(0.0f, 1.0f);
  float sinTheta = sqrtf(1 - (powf(r1, 2.0f / (lobe + 1))));
  float phi = M_2_PI * r2;
  glm::vec3 d = glm::vec3(
    sinTheta * cos(phi),
    sinTheta * sin(phi),
    powf(r1, 2.0f / (lobe + 1)));
  return d;
}
