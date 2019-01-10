#pragma once

#include <string>
#include <glm/glm.hpp>

class Image {
  private:
    class Color {
      public:
        unsigned char r, g, b;
        Color();
        Color(unsigned char r, unsigned char g, unsigned char b);
    };

  public:
    int height;
    int width;
    float* buffer;
    Color* pixels;
    std::string type;

    int pixelSamples;
    int directSamples;
    int indirectSoftSamples;
    int indirectSpecularBounces;

    void init();
    void setBuffer(int x, int y, float value);
    void setPixel(int index, glm::vec3 color);
    void setPixel(int x, int y, glm::vec3 color);
};
