#include <algorithm>
#include "utils.h"

float map(float input, float inputStart, float inputEnd, float outputStart, float outputEnd) {
  float inputRange = inputEnd - inputStart;
  float outputRange = outputEnd - outputStart;
  return (input - inputStart) * outputRange / inputRange + outputStart;
}

float computeFresnel(const glm::vec3 &normal, const glm::vec3 &wi, float nt, bool isInside) {
  http://www.pbr-book.org/3ed-2018/Reflection_Models/Specular_Reflection_and_Transmission.html#FrDielectric
  float ci = glm::dot(normal, wi);
  float ni = 1.0f;
  if (isInside) {
    std::swap(ni, nt);
  }
  float si = sqrtf(std::max(0.0f, 1 - ci * ci));
  float st = ni / nt * si;
  if (st >= 1) {
    // TIR.
    return 1;
  }
  float ct = sqrtf(std::max(0.0f, 1 - st * st));
  float rl = ((nt * ci) - (ni * ct)) / ((nt * ci) + (ni * ct));
  float rp = ((ni * ci) - (nt * ct)) / ((ni * ci) + (nt * ct));
  return (rl * rl + rp * rp) / 2;
}
