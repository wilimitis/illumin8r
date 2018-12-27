#include "light.h"

class PointLight : public Light
{
  public:
    virtual Ray sampleDirection();
};
