#include <glm/ext.hpp>
#include "phong.h"

// http://mathinfo.univ-reims.fr/IMG/pdf/Using_the_modified_Phong_reflectance_model_for_Physically_based_rendering_-_Lafortune.pdf

glm::vec3 Phong::brdf(const glm::vec3 &wo, const glm::vec3 &wi, const Hit &hit) const {
  glm::vec3 d = diffuse * float(M_1_PI);
  glm::vec3 reflection = glm::reflect(-wo, hit.normal);
  glm::vec3 s = specular * (lobe + 2.0f) *
    powf(glm::dot(wo, reflection), lobe) /
    (2.0f * float(M_PI));
  return d + s;
}

Material::Sample Phong::sampleDiffuse(const glm::vec3 &wo, const Hit &hit) const {
  Material::Sample sample;
  float r1 = glm::linearRand(0.0f, 1.0f);
  float r2 = glm::linearRand(0.0f, 1.0f);
  float phi = 2.0f * float(M_PI) * r1;
  float theta = acosf(sqrtf(r2));
  glm::vec3 direction = glm::vec3(
      cosf(phi) * sqrtf(1.0f - r2),
      sinf(phi) * sqrtf(1.0f - r2),
      sqrtf(r2));
  sample.direction = sampleWorld(direction, hit.normal);
  sample.brdf = diffuse * float(M_1_PI);
  sample.pdf = glm::dot(wo, hit.normal) * float(M_1_PI);
  return sample;
}

Material::Sample Phong::sampleSpecular(const glm::vec3 &wo, const Hit &hit) const {
  Material::Sample sample;
  glm::vec3 reflection = glm::reflect(-wo, hit.normal);
  float r1 = glm::linearRand(0.0f, 1.0f);
  float r2 = glm::linearRand(0.0f, 1.0f);
  float phi = 2.0f * float(M_PI) * r1;
  float theta = acosf(powf(r2, 1.0f / (lobe + 1.0f)));
  glm::vec3 direction = glm::vec3(
      cosf(phi) * sqrtf(1.0f - powf(r2, 2.0f / (lobe + 1.0f))),
      sinf(phi) * sqrtf(1.0f - powf(r2, 2.0f / (lobe + 1.0f))),
      powf(r2, 1.0f / (lobe + 1.0f)));
  // Center the sampled direction around the perfect specular reflection direction.
  // http://ompf2.com/viewtopic.php?t=2013#p5090
  sample.direction = sampleWorld(direction, reflection);
  if (glm::dot(hit.normal, sample.direction) < 0) {
    // We've generated a sample in the lobe below the surface.
    return sample;
  }
  sample.brdf = specular *
    (lobe + 2.0f) *
    powf(glm::dot(sample.direction, reflection), lobe) /
    (2.0f * float(M_PI));
  sample.pdf = powf(cosf(theta), lobe) * (lobe + 1.0f) / (2.0f * float(M_PI));
  return sample;
}

Material::Sample Phong::sampleRefractive(const glm::vec3 &wo, const Hit &hit) const {
  throw std::logic_error("refractive not supported for phong");
}
