#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <stdlib.h>
# ifdef __APPLE__
#  include <GLUT/glut.h>
# else
#  include <GL/glut.h>
# endif
#include "image.h"
#include "window.h"

#define ESC 27

extern Image image;

void glutKeyboard(unsigned char key, int x, int y)
{
	switch (ESC) {
    case 27:
      exit(0);
      break;
  }
}

void glutDisplay()
{
	glDrawPixels(
    image.width,
    image.height,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    image.pixels
  );
	glutSwapBuffers();
}

void Show()
{
	int argc = 1;
	char argstr[] = "raytrace";
	char *argv = argstr;
	glutInit(&argc,&argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(
		(glutGet(GLUT_SCREEN_WIDTH) - image.width) / 2,
		(glutGet(GLUT_SCREEN_HEIGHT) - image.height) /2
	);
	glutInitWindowSize(image.width, image.height);
	glutCreateWindow("illumin8r");
  glutKeyboardFunc(glutKeyboard);
	glutDisplayFunc(glutDisplay);
	glutMainLoop();
}