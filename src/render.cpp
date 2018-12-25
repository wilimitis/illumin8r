#include <stdio.h>
#include <time.h>
#include "render.h"

void Render(
  Camera camera,
  Image image,
  std::vector<Object> objects
)
{
  int	startTime =  (int) time(NULL);

  // TODO

  int endTime = (int) time(NULL);
  int t = endTime - startTime;
  int h = t / 3600;
  int m = (t % 3600) / 60;
  int s = t % 60;
  printf("\nRender time is %d:%02d:%02d.\n", h, m, s);
}