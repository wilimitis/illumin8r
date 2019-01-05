#include <glm/ext.hpp>
#include "phong.h"

// http://mathinfo.univ-reims.fr/IMG/pdf/Using_the_modified_Phong_reflectance_model_for_Physically_based_rendering_-_Lafortune.pdf

Material::Sample Phong::sampleDiffuse(const glm::vec3 &ki, const glm::vec3 &normal) const {
  Material::Sample sample;
  float r1 = glm::linearRand(0.0f, 1.0f);
  float r2 = glm::linearRand(0.0f, 1.0f);
  float phi = 2 * float(M_PI) * r1;
  float theta = acosf(sqrtf(r2));
  glm::vec3 direction = glm::vec3(
      cosf(phi) * sqrtf(1 - r2),
      sinf(phi) * sqrtf(1 - r2),
      sqrtf(r2));
  sample.direction = sampleWorld(direction, normal);
  sample.brdf = diffuse * float(M_1_PI);
  sample.pdf = glm::dot(ki, normal) * float(M_1_PI);
  return sample;
}

Material::Sample Phong::sampleSpecular(const glm::vec3 &ki, const glm::vec3 &normal) const {
  Material::Sample sample;
  glm::vec3 reflection = glm::reflect(-ki, normal);
  float r1 = glm::linearRand(0.0f, 1.0f);
  float r2 = glm::linearRand(0.0f, 1.0f);
  float phi = 2.0f * float(M_PI) * r1;
  float theta = acosf(powf(r2, 1.0f / (lobe + 1.0f)));
  glm::vec3 direction = glm::vec3(
      cosf(phi) * sqrtf(1.0f - powf(r2, 2.0f / (lobe + 1.0f))),
      sinf(phi) * sqrtf(1.0f - powf(r2, 2.0f / (lobe + 1.0f))),
      powf(r2, 1.0f / (lobe + 1.0f)));
  // Center the sampled direction around the perfect specular reflection direction
  // instead of the surface normal.
  // http://ompf2.com/viewtopic.php?t=2013#p5090
  sample.direction = sampleWorld(direction, reflection);
  sample.brdf = specular *
    (lobe + 2.0f) *
    powf(glm::dot(sample.direction, reflection), lobe) /
    (2.0f * float(M_PI));
  sample.pdf = powf(cosf(theta), lobe) * (lobe + 1.0f) / (2.0f * float(M_PI));
  return sample;
}
