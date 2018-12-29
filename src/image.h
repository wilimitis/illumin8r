#pragma once

#include <string>
#include <glm/glm.hpp>

class Image
{
  private:
    class Color
    {
      public:
        unsigned char r, g, b;
        Color();
        Color(unsigned char r, unsigned char g, unsigned char b);
    };

  public:
    int height;
    int width;
    Color* pixels;
    std::string render;

    void init();
    void setPixel(int x, int y, glm::vec3 color);
};
