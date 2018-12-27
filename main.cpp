#include <json/json.hpp>
using json = nlohmann::json;
#include <glm/glm.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "src/camera.h"
#include "src/image.h"
#include "src/render.h"
#include "src/sphere.h"
#include "src/window.h"

Camera camera;
Image image;
std::vector<Object*> objects; 

glm::vec3 getVec3(json node)
{
  return glm::vec3(node["x"], node["y"], node["z"]);
}

void setupCamera(json node)
{
  camera.direction = getVec3(node["direction"]);
  camera.fieldOfView = node["fieldOfView"];
  camera.position = getVec3(node["position"]);
  camera.up = getVec3(node["up"]);
}

void setupImage(json node)
{
  image.height = node["height"];
  image.width = node["width"];
  image.init();
}

void setupObjects(json node)
{
  for (auto& object : node) {
    if (object["type"] == "sphere") {
      Sphere* s = new Sphere();
      objects.push_back(s);
    }
  }
}

void setup()
{
  std::ifstream i("scene.json");
  nlohmann::json scene;
  i >> scene;
  i.close();
  
  setupCamera(scene["camera"]);
  setupImage(scene["image"]);
  setupObjects(scene["objects"]);

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
