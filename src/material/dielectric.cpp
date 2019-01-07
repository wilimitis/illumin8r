#include <glm/ext.hpp>
#include "dielectric.h"
#include "../utils.h"

Material::Sample Dielectric::sampleDiffuse(const glm::vec3 &ki, const Hit &hit) const {
  throw std::logic_error("diffuse not supported for dielectrics");
}

Material::Sample Dielectric::sampleSpecular(const glm::vec3 &ki, const Hit &hit) const {
  Material::Sample sample;
  sample.direction = glm::reflect(-ki, hit.normal);
  float fresnel = schlick(hit.normal, ki, refractiveIndex);
  sample.brdf = specular *
    fresnel /
    abs(glm::dot(hit.normal, sample.direction));
  sample.pdf = 1;
  return sample;
}

Material::Sample Dielectric::sampleRefractive(const glm::vec3 &ki, const Hit &hit) const {
  // NOTE: If this function is used to compute radiance then we may need to account for
  // "non-symmetry with transmission to different medium".
  // http://www.pbr-book.org/3ed-2018/Light_Transport_I_Surface_Reflection/Sampling_Reflection_Functions.html#SpecularReflectionandTransmission
  Material::Sample sample;
  float n = hit.isInside ? refractiveIndex : 1.0f / refractiveIndex;
  sample.direction = glm::refract(-ki, hit.normal, n);
  if (sample.direction == glm::vec3(0)) {
    // TIR, bail early and let caller deal with zero direction.
    return sample;
  }
  float fresnel = schlick(hit.normal, ki, refractiveIndex);
  sample.brdf = refractive *
    (1.0f - fresnel) /
    abs(glm::dot(hit.normal, sample.direction));
  sample.pdf = 1;
  return sample;
}
