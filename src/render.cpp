#include <glm/glm.hpp>
#include <stdio.h>
#include <time.h>
#include "ray.h"
#include "render.h"

#define WHITE 1
#define BLACK 0

Ray getRay(int x, int y, Camera camera, Image image)
{
  // Compute for aspect ratio.
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

glm::vec3 getColor(int x, int y, Ray ray, std::vector<Object*> objects)
{
  for (int i = 0; i < objects.size(); i++) {
    if (objects.at(i)->intersects(ray)) {
      return glm::vec3(WHITE);
    }
  }
  return glm::vec3(BLACK);
}

void render(
  Camera camera,
  Image image,
  std::vector<Object*> objects
)
{
  for (int x = 0; x < image.width; x++) {
    for (int y = 0; y < image.height; y++) {
      Ray ray = getRay(x, y, camera, image);
      glm::vec3 color = getColor(x, y, ray, objects);
      image.setPixel(x, y, color);
    }
  }
}

void Render(
  Camera camera,
  Image image,
  std::vector<Object*> objects
)
{
  int	startTime =  (int) time(NULL);

  render(camera, image, objects);

  int endTime = (int) time(NULL);
  int runTime = endTime - startTime;
  int hours = runTime / 3600;
  int minutes = (runTime % 3600) / 60;
  int seconds = runTime % 60;
  printf("Render time is %d:%02d:%02d.\n", hours, minutes, seconds);
}