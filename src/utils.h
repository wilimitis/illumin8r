#pragma once

#include <glm/glm.hpp>

float map(float input, float inputStart, float inputEnd, float outputStart, float outputEnd);
float schlick(const glm::vec3 &normal, const glm::vec3 &incident, float n2);
