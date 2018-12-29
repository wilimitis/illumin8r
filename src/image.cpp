#include "image.h"

Image::Color::Color() {}
Image::Color::Color(unsigned char r, unsigned char g, unsigned char b)
  : r(r), g(g), b(b) {}

void Image::init() {
  buffer = new float[height * width];
  pixels = new Color[height * width];
}

unsigned char convert(float f) {
  return (unsigned char) glm::clamp(int(f * 255), 0, 255);
}

void Image::setBuffer(int x, int y, float value) {
  int position = y * width + x;
  buffer[position] = value;
}

void Image::setPixel(int index, glm::vec3 color) {
  unsigned char r = (unsigned char) int(color.x * 255);
  unsigned char g = (unsigned char) int(color.y * 255);
  unsigned char b = (unsigned char) int(color.z * 255);
  pixels[index] = Color(
    convert(color.x),
    convert(color.y),
    convert(color.z)
  );
}

void Image::setPixel(int x, int y, glm::vec3 color) {
  int position = y * width + x;
  setPixel(position, color);
}
