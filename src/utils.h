#pragma once

#include <glm/glm.hpp>

float map(float input, float inputStart, float inputEnd, float outputStart, float outputEnd);
float computeFresnel(const glm::vec3 &normal, const glm::vec3 &wi, float nt, bool isInside);
