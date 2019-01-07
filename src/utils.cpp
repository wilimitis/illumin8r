#include "utils.h"

float map(float input, float inputStart, float inputEnd, float outputStart, float outputEnd) {
  float inputRange = inputEnd - inputStart;
  float outputRange = outputEnd - outputStart;
  return (input - inputStart) * outputRange / inputRange + outputStart;
}

float schlick(const glm::vec3 &normal, const glm::vec3 &incident, float n2) {
  float n1 = 1.0f;
  float Ro = ((n1 - n2) * (n1 - n2)) / ((n1 + n2) * (n1 + n2));
  float c = glm::dot(normal, incident);
	return Ro + (1.0f - Ro) * powf(1.0f - c, 5.0f);
}
