#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include "src/camera.h"
#include "src/image.h"
#include "src/render.h"
#include "src/sphere.h"
#include "src/window.h"

Camera camera;
Image image;
std::vector<Object*> objects; 

void setup()
{
  // TODO: Read json.
  
  camera.direction = glm::vec3(0, 0, -1);
  camera.fieldOfView = 30;
  camera.position = glm::vec3(0, 0, 10);
  camera.up = glm::vec3(0, 1, 0);

  image.height = 400;
  image.width = 400;
  image.init();
  
  Sphere *s = new Sphere();
  objects.push_back(s);
  std::cout << "setup complete" << std::endl;
}

int main()
{
  std::cout << "illumin8r" << std::endl;
  setup();
  Render(camera, image, objects);
  Show();
  return 0;
}