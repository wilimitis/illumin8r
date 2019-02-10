#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_query.hpp>
#include <iostream>
#include <thread>
#include "../render.h"
#include "photonMap.h"

int PhotonMap::photonCount = 100;
int PhotonMap::photonSearchCountMinimum = 4;
float PhotonMap::photonSearchDistanceSquared = 4;

void PhotonMap::emitPhoton(
  Ray &ray,
  const std::vector<Object*> &objects,
  Photon* photon,
  bool requiresSpecularHit
) {
  Hit hit = cast(ray, objects);
  if (hit.isEmpty) {
    return;
  }

  // Store hits on diffuse surfaces only.
  if (!requiresSpecularHit && hit.material->isDiffuse()) {
    Photon* p = new Photon();
    p->position = hit.position;
    p->power = photon->power;
    photons.push_back(p);
  }

  // Flip a coin.
  // TODO: Coin flip inside sampleSpecular to determine reflect/refract?
  // TODO: Maybe just coin flip inside a single sample function?
  float random = glm::linearRand(0.0f, 1.0f);
  float Pmax = std::max({photon->power.x, photon->power.y, photon->power.z});
  float Pd = glm::compMax(hit.material->diffuse * photon->power) / Pmax;
  float Ps = glm::compMax(hit.material->specular * photon->power) / Pmax;
  float Pr = glm::compMax(hit.material->refractive * photon->power) / Pmax;
  
  // Choose a path.
  if (random < Pd) {
    Material::Sample sample = hit.material->sample(-ray.direction, hit, Material::Sample::Type::diffuse);
    assert(glm::dot(hit.normal, sample.direction) >= 0);
    photon->power = photon->power *
      glm::dot(sample.direction, hit.normal) *
      sample.brdf /
      sample.pdf /
      Pd;
    ray.direction = sample.direction;
    assert(glm::dot(ray.direction, hit.normal) > 0);
  } else if (random < Pd + Ps) {
    requiresSpecularHit = false;
    Material::Sample sample = hit.material->sample(-ray.direction, hit, Material::Sample::Type::specular);
    if (glm::dot(hit.normal, sample.direction) <= 0) {
      return;
    }
    photon->power = photon->power *
      glm::dot(sample.direction, hit.normal) *
      sample.brdf /
      sample.pdf /
      Ps;
    ray.direction = sample.direction;
    assert(glm::dot(ray.direction, hit.normal) > 0);
  } else if (random < Pd + Ps + Pr) {
    requiresSpecularHit = false;
    Material::Sample sample = hit.material->sampleRefractive(-ray.direction, hit);
    if (sample.direction == glm::vec3(0)) {
      // TIR.
      return;
    }
    photon->power = photon->power *
      abs(glm::dot(sample.direction, hit.normal)) *
      sample.brdf /
      sample.pdf /
      Pr;
    ray.direction = sample.direction;
    assert(glm::dot(ray.direction, hit.normal) < 0);
  } else {
    // Absorption.
    return;
  }
  assert(glm::isNormalized(ray.direction, 0.1f));
  // TODO: Parameterize bias.
  float bias = 0.0001f;
  ray.position = hit.position + bias * ray.direction;
  emitPhoton(ray, objects, photon, requiresSpecularHit);
}

// TODO: Reconsider api.
PhotonMap::PhotonNode::PhotonNode(std::vector<Photon*> &photons) {
  if (photons.size() == 1) {
    photon = photons.at(0);
    photon->axis = -1;
    left = NULL;
    right = NULL;
    return;
  }

  // Find the cube surrounding the points.
  glm::vec3 min = glm::vec3(FLT_MAX), max = glm::vec3(-FLT_MAX);
  int photonsSize = photons.size();
  for (int i = 0; i < photonsSize; i++) {
    if (photons.at(i)->position.x < min.x) min.x = photons.at(i)->position.x;
    if (photons.at(i)->position.y < min.y) min.y = photons.at(i)->position.y;
    if (photons.at(i)->position.z < min.z) min.z = photons.at(i)->position.z;
    if (photons.at(i)->position.x > max.x) max.x = photons.at(i)->position.x;
    if (photons.at(i)->position.y > max.y) max.y = photons.at(i)->position.y;
    if (photons.at(i)->position.z > max.z) max.z = photons.at(i)->position.z;
  }
  
  // Select the dimension which is largest.
  float a[3] = {abs(max.x - min.x), abs(max.y - min.y), abs(max.z - min.z)};
  int n = sizeof(a) / sizeof(float);
  int axis = std::distance(a, std::max_element(a, a + n));
  
  // Find the median of the points along the dimension.
  std::sort(photons.begin(), photons.end(), [axis](const Photon* p1, const Photon* p2) {
    return p1->position[axis] < p2->position[axis];
  });
  int median = photons.size() / 2;
  photon = photons.at(median);
  photon->axis = axis;

  // Partition the points by the median.
  std::vector<Photon*> lower(photons.begin(), photons.begin() + median);
  std::vector<Photon*> higher(photons.begin() + median + 1, photons.end());
  left = lower.size() == 0 ? NULL : new PhotonNode(lower);
  right = higher.size() == 0 ? NULL : new PhotonNode(higher);
}

void PhotonMap::getNearest(
  std::vector<Photon*> &photons,
  PhotonNode* photonNode,
  glm::vec3 position
) {
  if (!photonNode) {
    return;
  }

  int axis = photonNode->photon->axis;
  if (axis != -1) {
    float splitDistance = position[axis] - photonNode->photon->position[axis];
    float splitDistanceSquared = splitDistance * splitDistance;
    if (splitDistance < 0) {
      PhotonMap::getNearest(photons, photonNode->left, position);
      if (splitDistanceSquared < photonSearchDistanceSquared) {
        PhotonMap::getNearest(photons, photonNode->right, position);
      }
    } else {
      PhotonMap::getNearest(photons, photonNode->right, position);
      if (splitDistanceSquared < photonSearchDistanceSquared) {
        PhotonMap::getNearest(photons, photonNode->left, position);
      }
    }
  }

  float trueDistanceSquared = glm::distance2(position, photonNode->photon->position);
  if (trueDistanceSquared < photonSearchDistanceSquared) {
    photons.push_back(photonNode->photon);
  }
}

void PhotonMap::emitPhotons(
  const std::vector<Light*> &lights,
  const std::vector<Object*> &objects,
  bool requiresSpecularHit,
  int count
) {
  for (int lightIndex = 0; lightIndex < lights.size(); lightIndex++) {
    for (int i = 0; i < photonCount / lights.size(); i++) {
      Ray ray = lights.at(lightIndex)->sampleDirection();
      Photon p;
      // TODO: Determine point light pdf and if other terms are missing.
      // https://github.com/mmp/pbrt-v3/blob/master/src/integrators/sppm.cpp#L337
      p.power = lights.at(lightIndex)->power;
      emitPhoton(ray, objects, &p, requiresSpecularHit);
    }
  }
}

void PhotonMap::init(
  const std::vector<Light*> &lights,
  const std::vector<Object*> &objects,
  bool requiresSpecularHit
) {
  // int threadCount = 8;
  // int threadCountSqrt = sqrt(threadCount);
  // std::thread threads[threadCount];
  // for (int i = 0; i < threadCountSqrt; i++) {
  //   for (int j = 0; j < threadCountSqrt; j++) {
  //     threads[i * threadCountSqrt + j] = std::thread(
  //       &PhotonMap::emitPhotons,
  //       std::ref(lights),
  //       std::ref(objects),
  //       requiresSpecularHit,
  //       photonCount / threadCount
  //     );
  //   }
  // }
  // for (int i = 0; i < threadCount; i++) {
  //   threads[i].join();
  // }
  emitPhotons(lights, objects, requiresSpecularHit, photonCount);
  std::cout << "photon emission complete: " << photons.size() << " / " << photonCount << std::endl;

  if (photons.size() > 0) {
    photonNode = new PhotonNode(photons);
  }
  std::cout << "photon map complete" << std::endl;
}
