#include <glm/ext.hpp>
#include "dielectric.h"
#include "../utils.h"

glm::vec3 Dielectric::brdf(const glm::vec3 &wo, const glm::vec3 &wi, const Hit &hit) const {
  throw std::logic_error("not supported for dielectrics");
}

float Dielectric::pdf(const glm::vec3 &wo, const glm::vec3 &wi, const Hit &hit) const {
  throw std::logic_error("not supported for dielectrics");
}

Material::Sample Dielectric::sample(const glm::vec3 &wo, const Hit &hit, const Sample::Type &type) const {
  if (type == Sample::Type::diffuse) {
    throw std::logic_error("not supported for dielectrics");
  }
  Material::Sample sample;
  sample.direction = glm::reflect(-wo, hit.normal);
  float fresnel = computeFresnel(hit.normal, wo, refractiveIndex, hit.isInside);
  float dot = glm::dot(hit.normal, sample.direction);
  assert(dot > 0);
  sample.brdf = specular * fresnel / dot;
  sample.pdf = 1; // TODO: if inside one sample func then fresnel
  return sample;
}

Material::Sample Dielectric::sampleRefractive(const glm::vec3 &wo, const Hit &hit) const {
  // NOTE: If this function is used to compute radiance then we may need to account for
  // "non-symmetry with transmission to different medium".
  // http://www.pbr-book.org/3ed-2018/Light_Transport_I_Surface_Reflection/Sampling_Reflection_Functions.html#SpecularReflectionandTransmission
  Material::Sample sample;
  float n = hit.isInside ? refractiveIndex : 1.0f / refractiveIndex;
  sample.direction = glm::refract(-wo, hit.normal, n);
  if (sample.direction == glm::vec3(0)) {
    // TIR, bail early and let caller deal with zero direction.
    return sample;
  }
  float fresnel = computeFresnel(hit.normal, wo, refractiveIndex, hit.isInside);
  sample.brdf = refractive * (1.0f - fresnel) / abs(glm::dot(hit.normal, sample.direction));
  sample.pdf = 1; // TODO: if inside one sample func then (1.0f - fresnel)
  return sample;
}
