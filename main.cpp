#include <iostream>
#include <vector>
#include "src/camera.h"
#include "src/image.h"
#include "src/render.h"
#include "src/window.h"

Camera camera;
Image image;
std::vector<Object> objects; 

void setup()
{
  // Read json.
  // Setup image.
  image.height = 400;
  image.width = 400;
  // Setup camera.
  // Setup objects.
}

int main()
{
  std::cout << "illumin8r" << std::endl;
  setup();
  Render(camera, image, objects);
  Show(image);
  return 0;
}