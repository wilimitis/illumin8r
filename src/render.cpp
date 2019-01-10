#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_query.hpp>
#include <iostream>
#include <time.h>
#include "light/photonMap.h"
#include "ray.h"
#include "render.h"
#include "utils.h"

#define WHITE 1
#define BLACK 0

PhotonMap causticPhotonMap, globalPhotonMap;

Ray getRay(const Scene &scene, int x, int y) {
  // Compute aspect ratio.
  // Shirley 7.5
  float arw = scene.image.width > scene.image.height
    ? float(scene.image.width) / scene.image.height
    : 1;
  float arh = scene.image.height > scene.image.width
    ? float(scene.image.height) / scene.image.width
    : 1;
  
  // Construct the orthographic view volume.
  // Shirley 7.2
  float bottom = -1, far = -1, left = -1;
  float top =  1, near =  1, right =  1;

  // Compute field of view.
  // Shirley 7.5
  float tf = tan(glm::radians(scene.camera.fieldOfView / 2)) * abs(near);

  // Compute screen coordinates in camera space.
  // Shirley 10.2
  glm::vec3 sc = glm::vec3(
    (left + (right - left) * ((x + 0.5) / scene.image.width)) * arw * tf,
    (bottom + (top - bottom) * ((y + 0.5) / scene.image.height)) * arh * tf,
    -1
  );

  // Construct a coordinate system (orthonormal frame).
  // Shirley 7.2.1
  glm::vec3 w = -glm::normalize(scene.camera.direction);
  glm::vec3 u = glm::normalize(glm::cross(scene.camera.up, w));
  glm::vec3 v = glm::normalize(glm::cross(w, u));

  // Compute screen position in world space.
  // Shirley 10.2
  glm::vec3 sw = scene.camera.position + sc.x * u + sc.y * v + sc.z * w;
  glm::vec3 rd = glm::normalize(sw - scene.camera.position);

  return Ray(rd, scene.camera.position);
}

Hit cast(const Ray &ray, const std::vector<Object*> &objects) {
  Hit result;
  for (int i = 0; i < objects.size(); i++) {
    Object* object = objects.at(i);
    Ray localRay = object->getLocal(ray);
    Hit hit = object->intersects(localRay);
    if (hit.isEmpty) {
      continue;
    }

    Ray intersectionRay = object->getWorld(Ray(hit.normal, hit.position));
    hit.position = intersectionRay.position;
    hit.distance = glm::length(hit.position - ray.position);
    if (result.isEmpty || hit.distance < result.distance) {
      hit.material = object->material;
      hit.normal = intersectionRay.direction;
      assert(glm::isNormalized(intersectionRay.direction, 0.1f));
      if (glm::dot(ray.direction, hit.normal) > 0) {
        // We've hit the back.
        if (hit.material->refractiveIndex != 0) {
          hit.isInside = true;
        }
        hit.normal *= -1;
      }
      hit.objectKey = object->key;
      result = hit;
    }
  }
  return result;
}

void preRender(const Scene &scene) {
  if (scene.image.type == "photon") {
    causticPhotonMap.init(scene.lights, scene.objects, true /* requiresSpecularHit */);
    // globalPhotonMap.init(lights, objects, false /* requiresSpecularHit */);
  }
}

glm::vec3 renderDepth(Scene &scene, const Ray &ray, int x, int y) {
  // TODO: Move into pre render.
  Hit hit = cast(ray, scene.objects);
  scene.image.setBuffer(x, y, hit.isEmpty ? -1 : hit.distance);
  return glm::vec3(BLACK);
}

glm::vec3 renderHit(Scene &scene, const Ray &ray, int x, int y) {
  Hit hit = cast(ray, scene.objects);
  glm::vec3 color = glm::vec3(hit.isEmpty ? BLACK : WHITE);
  return color;
}

glm::vec3 renderNormal(Scene &scene, const Ray &ray, int x, int y) {
  Hit hit = cast(ray, scene.objects);
  glm::vec3 color;
  if (hit.isEmpty) {
    color = glm::vec3(BLACK);
  } else {
    // https://en.wikipedia.org/wiki/Normal_mapping
    color = glm::vec3(
      map(hit.normal.x, -1, 1, 0, 1),
      map(hit.normal.y, -1, 1, 0, 1),
      hit.normal.z
    );
  }
  return color;
}

glm::vec3 computeDirect(Scene &scene, const Ray &ray, const Hit &hit) {
  // TODO: Iterate after moving beyond point lights, right now there is only one sample to
  // prevent useless iterations since each sample will be the path to the point light.
  float bias = 0.001f;
  glm::vec3 color = glm::vec3(BLACK);
  if (hit.isEmpty || !hit.material->isDiffuse()) {
    return color;
  }
  for (int i = 0; i < scene.lights.size(); i++) {
    for (int j = 0; j < scene.image.directSamples; j++) {
      glm::vec3 lightSamplePosition = scene.lights.at(i)->sampleDirection().position;
      glm::vec3 lightVector = lightSamplePosition - hit.position;
      glm::vec3 lightDirection = glm::normalize(lightVector);
      float lightDistance = glm::distance(lightSamplePosition, hit.position);
      Hit occlusion = cast(Ray(lightDirection, hit.position + bias * lightDirection), scene.objects);
      if (occlusion.isEmpty || occlusion.distance > lightDistance) {
        // TODO: Extract method to compute direct light sample so that e.g. Phong can have its
        // specular component represented properly and e.g. Dielectrics, etc. can do as they please.
        color +=
          hit.material->brdf(-ray.direction, lightDirection, hit) *
          scene.lights.at(i)->intensity *
          glm::dot(lightVector, hit.normal);
          // TODO: Implement dot(lightVector, -lightNormal) for non-spherical luminaires.
          // TODO: This blacks out the image. powf(lightDistance, 0.0f);
      }
    }
  }
  return color / float(scene.image.directSamples);
}

glm::vec3 computeIndirectCaustic(const Hit &hit) {
  if (hit.isEmpty) {
    return glm::vec3(BLACK);
  }
  std::vector<PhotonMap::Photon*> nearest;
  causticPhotonMap.getNearest(nearest, causticPhotonMap.photonNode, hit.position);
  glm::vec3 power = glm::vec3(0);
  if (nearest.size() < PhotonMap::photonSearchCountMinimum) {
    return power;
  }
  for (int i = 0; i < nearest.size(); i++) {
    power += (nearest.at(i)->power) / float(PhotonMap::photonCount);
  }
  power /= (M_PI * PhotonMap::photonSearchDistanceSquared);
  return power;
}

glm::vec3 computeIndirectSoft(Scene &scene, const Ray &ray, const Hit &hit) {
  // TODO: Recurse bounded by bounces.
  float bias = 0.001f;
  glm::vec3 color = glm::vec3(BLACK);
  if (scene.image.indirectSoftSamples == 0 || hit.isEmpty || !hit.material->isDiffuse()) {
    return color;
  }
  for (int i = 0; i < scene.image.indirectSoftSamples; i++) {
    Material::Sample diffuseSample = hit.material->sampleDiffuse(-ray.direction, hit);
    Ray diffuseRay = Ray(diffuseSample.direction, hit.position + bias * diffuseSample.direction);
    Hit diffuseHit = cast(diffuseRay, scene.objects);
    color += computeDirect(scene, diffuseRay, diffuseHit) *
      diffuseSample.brdf *
      abs(glm::dot(diffuseHit.normal, diffuseSample.direction)) /
      diffuseSample.pdf;
  }
  return hit.material->diffuse * color / float(scene.image.indirectSoftSamples);
}

glm::vec3 computeIndirectSpecular(
  Scene &scene,
  const Ray &ray,
  const Hit &hit,
  int bounces = 0
) {
  float bias = 0.001f;
  // TODO: Iterate after moving beyond ideal dialectrics, right now there is only one sample to
  // prevent useless iterations since each pure dialectric sample will be of the perfect path.
  glm::vec3 color = glm::vec3(BLACK);
  if (bounces > scene.image.indirectSpecularBounces || hit.isEmpty || hit.material->isDiffuse()) {
    return color;
  }

  // Reflection.
  Material::Sample reflectionSample = hit.material->sampleSpecular(-ray.direction, hit);
  Ray reflectionRay = Ray(reflectionSample.direction, hit.position + bias * reflectionSample.direction);
  Hit reflectionHit = cast(reflectionRay, scene.objects);
  glm::vec3 reflectionColor =
    computeDirect(scene, reflectionRay, reflectionHit) +
    computeIndirectSpecular(scene, reflectionRay, reflectionHit, bounces + 1) +
    computeIndirectCaustic(reflectionHit) +
    computeIndirectSoft(scene, reflectionRay, reflectionHit);
  reflectionColor *= reflectionSample.brdf;
  reflectionColor *= abs(glm::dot(hit.normal, reflectionSample.direction));
  reflectionColor /= reflectionSample.pdf;

  // Refraction.
  Material::Sample refractionSample = hit.material->sampleRefractive(-ray.direction, hit);
  glm::vec3 refractionColor = glm::vec3(BLACK);
  if (refractionSample.direction != glm::vec3(0) /* TIR */) {
    Ray refractionRay = Ray(refractionSample.direction, hit.position + bias * refractionSample.direction);
    Hit refractionHit = cast(refractionRay, scene.objects);
    refractionColor =
      computeDirect(scene, refractionRay, refractionHit) +
      computeIndirectSpecular(scene, refractionRay, refractionHit, bounces + 1) +
      computeIndirectCaustic(refractionHit) +
      computeIndirectSoft(scene, refractionRay, refractionHit);
    refractionColor *= refractionSample.brdf;
    refractionColor *= abs(glm::dot(hit.normal, refractionSample.direction));
    refractionColor /= refractionSample.pdf;
  }

  return reflectionColor + refractionColor;
}

// TODO: Refactor to return color at pixel.
glm::vec3 renderPhoton(Scene &scene, const Ray &ray,int x, int y) {
  Hit hit = cast(ray, scene.objects);
  glm::vec3 color = glm::vec3(BLACK);
  if (!hit.isEmpty) {
    color = 
      computeDirect(scene, ray, hit) +
      computeIndirectSpecular(scene, ray, hit) +
      computeIndirectCaustic(hit) +
      computeIndirectSoft(scene, ray, hit);
  }
  return color; 
}

glm::vec3 renderPixel(Scene &scene, const Ray &ray,int x, int y) {
  // TODO: Extract Renderer class.
  if (scene.image.type == "depth") {
    return renderDepth(scene, ray, x, y);
  } else if (scene.image.type == "hit") {
    return renderHit(scene, ray, x, y);
  } else if (scene.image.type == "normal") {
    return renderNormal(scene, ray, x, y);
  } else if (scene.image.type == "photon") {
    return renderPhoton(scene, ray, x, y);
  }
}

void render(Scene &scene) {
  for (int x = 0; x < scene.image.width; x++) {
    for (int y = 0; y < scene.image.height; y++) {
      Ray ray = getRay(scene, x, y);
      glm::vec3 color = renderPixel(scene, ray, x, y);
      scene.image.setPixel(x, y, color);
    }
  }
}

void postRender(Scene &scene) {
  // TODO: Extract Renderer class.
  if (scene.image.type == "depth") {
    int size = scene.image.height * scene.image.width;
    float min = FLT_MAX;
    float max = 0;
    for (int i = 0; i < size; i++) {
      if (scene.image.buffer[i] == -1) {
        continue;
      }
      if (min > scene.image.buffer[i]) {
        min = scene.image.buffer[i];
      }
      if (max < scene.image.buffer[i]) {
        max = scene.image.buffer[i];
      }
    }
    for (int i = 0; i < size; i++) {
      if (scene.image.buffer[i] == -1) {
        scene.image.setPixel(i, glm::vec3(BLACK));
      } else {
        float depth = (max - scene.image.buffer[i]) / (max - min);
        scene.image.setPixel(i, glm::vec3(depth));
      }
    }
  }
}

void Render(Scene &scene) {
  int	startTime =  (int) time(NULL);

  preRender(scene);
  render(scene);
  postRender(scene);

  int endTime = (int) time(NULL);
  int runTime = endTime - startTime;
  int hours = runTime / 3600;
  int minutes = (runTime % 3600) / 60;
  int seconds = runTime % 60;
  printf("Render time is %d:%02d:%02d.\n", hours, minutes, seconds);
}
