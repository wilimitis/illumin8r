#include "image.h"

Image::Color::Color() {}
Image::Color::Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}

void Image::init()
{
  pixels = new Color[height * width];
}

unsigned char convert(float f)
{
  return (unsigned char) glm::clamp(int(f * 255), 0, 255);
}

void Image::setPixel(int x, int y, glm::vec3 color)
{
  int position = y * width + x;
  unsigned char r = (unsigned char) int(color.x * 255);
  unsigned char g = (unsigned char) int(color.y * 255);
  unsigned char b = (unsigned char) int(color.z * 255);
  pixels[position] = Color(
    convert(color.x),
    convert(color.y),
    convert(color.z)
  );
}
