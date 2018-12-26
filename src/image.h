#pragma once

#include <glm/glm.hpp>

class Image
{
  private:
    class Color
    {
      public:
        unsigned char r, g, b;
        Color() {}
        Color(unsigned char r, unsigned char g, unsigned char b)
          : r(r), g(g), b(b) {}
    };

  public:
    int height;
    int width;
    Color *pixels;

    void init();
    void setPixel(int x, int y, glm::vec3 color);
};