#include <json/json.hpp>
using json = nlohmann::json;
#include <glm/glm.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "src/camera.h"
#include "src/image.h"
#include "src/light/pointLight.h"
#include "src/render.h"
#include "src/object/mesh.h"
#include "src/object/sphere.h"
#include "src/window.h"

Camera camera;
Image image;
std::vector<Light*> lights;
std::vector<Object*> objects;

glm::vec3 getVec3(json node) {
  return glm::vec3(node["x"], node["y"], node["z"]);
}

void setupCamera(json node) {
  camera.direction = getVec3(node["direction"]);
  camera.fieldOfView = node["fieldOfView"];
  camera.position = getVec3(node["position"]);
  camera.up = getVec3(node["up"]);
}

void setupImage(json node) {
  image.height = node["height"];
  image.width = node["width"];
  image.render = node["render"];
  image.init();
}

void setupLights(json node) {
  for (auto& light : node) {
    if (light["type"] == "point") {
      PointLight* l = new PointLight();
      l->position = getVec3(light["position"]);
      lights.push_back(l);
    }
  }
}

void setupObjects(json node) {
  for (auto& object : node) {
    Object* o;
    if (object["type"] == "sphere") {
      o = new Sphere();
    } else if (object["type"] == "obj") {
      o = new Mesh();
      std::string file = object["file"];
      ((Mesh*) o)->init(file.c_str());
    }
    json rotate = object["rotate"];
    o->translate(getVec3(object["translate"]));
    o->scale(getVec3(object["scale"]));
    o->rotate(rotate["angle"], getVec3(rotate["axis"]));
    objects.push_back(o);
  }
}

void setup() {
  std::ifstream i("scene.json");
  nlohmann::json scene;
  i >> scene;
  i.close();
  
  setupCamera(scene["camera"]);
  setupImage(scene["image"]);
  setupLights(scene["lights"]);
  setupObjects(scene["objects"]);

  std::cout << "setup complete" << std::endl;
}

int main() {
  std::cout << "illumin8r" << std::endl;
  setup();
  Render(camera, image, lights, objects);
  Show();
  return 0;
}
