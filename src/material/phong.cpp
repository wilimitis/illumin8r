#include <glm/ext.hpp>
#include "phong.h"

const int diffuseLobe = 1;

// http://mathinfo.univ-reims.fr/IMG/pdf/Using_the_modified_Phong_reflectance_model_for_Physically_based_rendering_-_Lafortune.pdf

float alpha(const glm::vec3 &wo, const glm::vec3 &wi, const Hit &hit) {
  glm::vec3 reflection = glm::reflect(-wi, hit.normal);
  float dot = glm::max(0.0f, glm::dot(wo, reflection));
  return dot;
}

float pdfHemisphere(float cosAlpha, int lobe) {
  return powf(cosAlpha, lobe) * (lobe + 1.0f) / (2.0f * float(M_PI));
}

struct HemisphereSample {
  glm::vec3 direction;
  float pdf;
  // TODO: Consider having samplers use this instead of alpha.
  // float theta;
};

HemisphereSample sampleHemisphere(const glm::vec3 &wo, const Hit &hit, int lobe) {
  float r1 = glm::linearRand(0.0f, 1.0f);
  float r2 = glm::linearRand(0.0f, 1.0f);
  float phi = 2.0f * float(M_PI) * r1;
  HemisphereSample sample;
  sample.direction = glm::vec3(
    cosf(phi) * sqrtf(1.0f - powf(r2, 2.0f / (lobe + 1.0f))),
    sinf(phi) * sqrtf(1.0f - powf(r2, 2.0f / (lobe + 1.0f))),
    powf(r2, 1.0f / (lobe + 1.0f)));
  // Embeds cos(arccos(...)).
  float cosTheta = powf(r2, 1.0f / (lobe + 1.0f));
  sample.pdf = pdfHemisphere(glm::max(0.0f, cosTheta), lobe);
  return sample;
}

glm::vec3 Phong::brdf(const glm::vec3 &wo, const glm::vec3 &wi, const Hit &hit) const {
  glm::vec3 total = diffuse * float(M_1_PI);
  if (specular != glm::vec3(0)) {
    glm::vec3 s = specular *
      (lobe + 2.0f) *
      powf(alpha(wo, wi, hit), lobe) /
      (2.0f * float(M_PI));
    s = glm::clamp(s, glm::vec3(0), specular);
    total += s;
  }
  return total;
}

float Phong::pdf(const glm::vec3 &wo, const glm::vec3 &wi, const Hit &hit) const {
  // TODO: Consider alternate heuristic.
  int count = 1;
  float total = pdfHemisphere(glm::max(0.0f, glm::dot(wi, hit.normal)), diffuseLobe);
  if (specular != glm::vec3(0)) {
    total += pdfHemisphere(alpha(wo, wi, hit), lobe);
    count++;
  } 
  return total / count;
}

Material::Sample Phong::sample(
  const glm::vec3 &wo,
  const Hit &hit,
  const float &random,
  const Sample::Type &type
) const {
  if (type == Sample::Type::diffuse) {
    HemisphereSample hemisphereSample = sampleHemisphere(wo, hit, diffuseLobe);
    Material::Sample sample;
    // TODO: Extract helper.
    sample.brdf = diffuse * float(M_1_PI);
    sample.direction = sampleWorld(hemisphereSample.direction, hit.normal);
    sample.pdf = hemisphereSample.pdf;
    return sample;
  }
  
  HemisphereSample hemisphereSample = sampleHemisphere(wo, hit, lobe);
  glm::vec3 reflection = glm::reflect(-wo, hit.normal);
  // Center the sampled direction around the perfect specular reflection direction.
  // http://ompf2.com/viewtopic.php?t=2013#p5090
  Material::Sample sample;
  sample.direction = sampleWorld(hemisphereSample.direction, reflection);
  if (glm::dot(hit.normal, sample.direction) < 0) {
    // We've generated a sample in the lobe below the surface.
    sample.direction = glm::vec3(0);
    return sample;
  }
  // TODO: Extract helper.
  sample.brdf = specular *
    (lobe + 2.0f) *
    powf(alpha(wo, sample.direction, hit), lobe) /
    (2.0f * float(M_PI));
  sample.pdf = hemisphereSample.pdf;
  return sample;
}
