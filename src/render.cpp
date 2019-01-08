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

Ray getRay(int x, int y, Camera camera, Image image) {
  // Compute aspect ratio.
  // Shirley 7.5
  float arw = image.width > image.height
    ? float(image.width) / image.height
    : 1;
  float arh = image.height > image.width
    ? float(image.height) / image.width
    : 1;
  
  // Construct the orthographic view volume.
  // Shirley 7.2
  float bottom = -1, far = -1, left = -1;
  float top =  1, near =  1, right =  1;

  // Compute field of view.
  // Shirley 7.5
  float tf = tan(glm::radians(camera.fieldOfView / 2)) * abs(near);

  // Compute screen coordinates in camera space.
  // Shirley 10.2
  glm::vec3 sc = glm::vec3(
    (left + (right - left) * ((x + 0.5) / image.width)) * arw * tf,
    (bottom + (top - bottom) * ((y + 0.5) / image.height)) * arh * tf,
    -1
  );

  // Construct a coordinate system (orthonormal frame).
  // Shirley 7.2.1
  glm::vec3 w = -glm::normalize(camera.direction);
  glm::vec3 u = glm::normalize(glm::cross(camera.up, w));
  glm::vec3 v = glm::normalize(glm::cross(w, u));

  // Compute screen position in world space.
  // Shirley 10.2
  glm::vec3 sw = camera.position + sc.x * u + sc.y * v + sc.z * w;
  glm::vec3 rd = glm::normalize(sw - camera.position);

  return Ray(rd, camera.position);
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

void preRender(
  Image image,
  const std::vector<Light*> &lights,
  const std::vector<Object*> &objects
) {
  if (image.render == "photon") {
    causticPhotonMap.init(lights, objects, true /* requiresSpecularHit */);
    // globalPhotonMap.init(lights, objects, false /* requiresSpecularHit */);
  }
}

// TODO: Move into pre render.
void renderDepth(
  int x,
  int y,
  Image &image,
  const Ray &ray,
  const std::vector<Object*> &objects
) {
  Hit hit = cast(ray, objects);
  image.setBuffer(x, y, hit.isEmpty ? -1 : hit.distance);
}

void renderHit(
  int x,
  int y,
  Image &image,
  const Ray &ray,
  const std::vector<Object*> &objects
) {
  Hit hit = cast(ray, objects);
  glm::vec3 color = glm::vec3(hit.isEmpty ? BLACK : WHITE);
  image.setPixel(x, y, color);
}

void renderNormal(
  int x,
  int y,
  Image &image,
  const Ray &ray,
  const std::vector<Object*> &objects
) {
  Hit hit = cast(ray, objects);
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
  image.setPixel(x, y, color);
}

glm::vec3 computeDirect(
  const std::vector<Light*> &lights,
  const std::vector<Object*> &objects,
  const Hit &hit
) {
  // TODO: Iterate after moving beyond point lights, right now there is only one sample to
  // prevent useless iterations since each sample will be the path to the point light.
  float bias = 0.001f;
  glm::vec3 color = glm::vec3(BLACK);
  if (hit.isEmpty || !hit.material->isDiffuse()) {
    return color;
  }
  for (int i = 0; i < lights.size(); i++) {
    // Shadow.
    glm::vec3 lightDirection = glm::normalize(lights.at(i)->position - hit.position);
    Hit occlusion = cast(Ray(lightDirection, hit.position + bias * lightDirection), objects);
    float lightDistance = glm::distance(lights.at(i)->position, hit.position);
    float visibility = !occlusion.isEmpty && occlusion.distance <= lightDistance ? 0 : 1;
    
    // TODO: Extract method to compute direct light sample so that e.g. Phong can have its
    // specular component represented properly and e.g. Dielectrics, etc. can do as they please.
    color += lights.at(i)->intensity *
      hit.material->diffuse * 
      abs(glm::dot(lightDirection, hit.normal)) *
      visibility;
  }
  return color;
}

glm::vec3 computeIndirectSpecular(
  const std::vector<Light*> &lights,
  const std::vector<Object*> &objects,
  const Ray &ray,
  const Hit &hit,
  int bounces = 0
) {
  float bias = 0.001f;
  float bounceMax = 3;
  // TODO: Iterate after moving beyond ideal dialectrics, right now there is only one sample to
  // prevent useless iterations since each pure dialectric sample will be of the perfect path.
  glm::vec3 color = glm::vec3(BLACK);
  if (bounces > bounceMax || hit.material->isDiffuse()) {
    return color;
  }
  // Reflection.
  Material::Sample reflectionSample = hit.material->sampleSpecular(-ray.direction, hit);
  Ray reflectionRay = Ray(reflectionSample.direction, hit.position + bias * reflectionSample.direction);
  Hit reflectionHit = cast(reflectionRay, objects);
  glm::vec3 reflectionColor =
    computeIndirectSpecular(lights, objects, reflectionRay, reflectionHit, bounces + 1) +
    computeDirect(lights, objects, reflectionHit);
    computeIndirectSoft(scene, reflectionRay, reflectionHit) +
  reflectionColor *= (reflectionSample.brdf / reflectionSample.pdf);
  // Refraction.
  Material::Sample refractionSample = hit.material->sampleRefractive(-ray.direction, hit);
  glm::vec3 refractionColor = glm::vec3(BLACK);
  if (refractionSample.direction != glm::vec3(0) /* TIR */) {
    Ray refractionRay = Ray(refractionSample.direction, hit.position + bias * refractionSample.direction);
    Hit refractionHit = cast(refractionRay, objects);
    refractionColor = 
      computeIndirectSpecular(lights, objects, refractionRay, refractionHit, bounces + 1) +
      computeDirect(lights, objects, refractionHit);
      computeIndirectSoft(scene, refractionRay, refractionHit) +
    refractionColor *= (refractionSample.brdf / refractionSample.pdf);
  }
  return reflectionColor + refractionColor;
}

glm::vec3 computeIndirectCaustic(const Hit &hit) {
  std::vector<PhotonMap::Photon*> nearest;
  causticPhotonMap.getNearest(nearest, causticPhotonMap.photonNode, hit.position);
  glm::vec3 power = glm::vec3(0);
  for (int i = 0; i < nearest.size(); i++) {
    power += (nearest.at(i)->power) / float(PhotonMap::photonCount);
  }
  power /= (M_PI * PhotonMap::photonSearchDistanceSquared);
  return power;
}

glm::vec3 computeIndirectSoft(
  const std::vector<Light*> &lights,
  const std::vector<Object*> &objects,
  const Ray &ray,
  const Hit &hit
) {
  float bias = 0.001f;
  int indirectSoftSamples = 4;
  glm::vec3 color = glm::vec3(BLACK);
  if (hit.isEmpty || !hit.material->isDiffuse()) {
    return color;
  }
  for (int i = 0; i < indirectSoftSamples; i++) {
    Material::Sample diffuseSample = hit.material->sampleDiffuse(-ray.direction, hit);
    Ray diffuseRay = Ray(diffuseSample.direction, hit.position + bias * diffuseSample.direction);
    Hit diffuseHit = cast(diffuseRay, objects);
    color += computeDirect(lights, objects, diffuseHit) *
      diffuseSample.brdf /
      diffuseSample.pdf;
  }
  return hit.material->diffuse * color / float(indirectSoftSamples);
}

// TODO: Refactor to return color at pixel.
void renderPhoton(
  int x,
  int y,
  Image &image,
  const Ray &ray,
  const std::vector<Light*> &lights,
  const std::vector<Object*> &objects
) {
  Hit hit = cast(ray, objects);
  glm::vec3 color = glm::vec3(BLACK);
  if (!hit.isEmpty) {
    color = computeDirect(lights, objects, hit) +
      computeIndirectSpecular(lights, objects, ray, hit) +
      computeIndirectCaustic(hit) +
      computeIndirectSoft(lights, objects, ray, hit);
  }
  image.setPixel(x, y, color);
}

void renderPixel(
  int x,
  int y,
  Image &image,
  const Ray &ray,
  const std::vector<Light*> &lights,
  const std::vector<Object*> &objects
) {
  // TODO: Extract Renderer class.
  if (image.render == "depth") {
    renderDepth(x, y, image, ray, objects);
  } else if (image.render == "hit") {
    renderHit(x, y, image, ray, objects);
  } else if (image.render == "normal") {
    renderNormal(x, y, image, ray, objects);
  } else if (image.render == "photon") {
    renderPhoton(x, y, image, ray, lights, objects);
  }
}

void render(
  Camera camera,
  Image image,
  const std::vector<Light*> &lights,
  const std::vector<Object*> &objects
) {
  for (int x = 0; x < image.width; x++) {
    for (int y = 0; y < image.height; y++) {
      Ray ray = getRay(x, y, camera, image);
      renderPixel(x, y, image, ray, lights, objects);
    }
  }
}

void postRender(Image image) {
  // TODO: Extract Renderer class.
  if (image.render == "depth") {
    int size = image.height * image.width;
    float min = FLT_MAX;
    float max = 0;
    for (int i = 0; i < size; i++) {
      if (image.buffer[i] == -1) {
        continue;
      }
      if (min > image.buffer[i]) {
        min = image.buffer[i];
      }
      if (max < image.buffer[i]) {
        max = image.buffer[i];
      }
    }
    for (int i = 0; i < size; i++) {
      if (image.buffer[i] == -1) {
        image.setPixel(i, glm::vec3(BLACK));
      } else {
        float depth = (max - image.buffer[i]) / (max - min);
        image.setPixel(i, glm::vec3(depth));
      }
    }
  }
}

void Render(
  Camera camera,
  Image image,
  const std::vector<Light*> &lights,
  const std::vector<Object*> &objects
) {
  int	startTime =  (int) time(NULL);

  preRender(image, lights, objects);
  render(camera, image, lights, objects);
  postRender(image);

  int endTime = (int) time(NULL);
  int runTime = endTime - startTime;
  int hours = runTime / 3600;
  int minutes = (runTime % 3600) / 60;
  int seconds = runTime % 60;
  printf("Render time is %d:%02d:%02d.\n", hours, minutes, seconds);
}
