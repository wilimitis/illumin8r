#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <iostream>
#include <stdlib.h>
# ifdef __APPLE__
#  include <GLUT/glut.h>
# else
#  include <GL/glut.h>
# endif
#include "render.h"
#include "scene.h"
#include "window.h"

#define ESC 27

extern Scene scene;

void glutKeyboard(unsigned char key, int x, int y) {
	switch (key) {
    case ESC:
      exit(0);
      break;
		case ' ':
			Render(scene);
			break;
		default:
			// Do nothing.
			break;
  }
}

void glutDisplay() {
	glDrawPixels(
    scene.image.width,
    scene.image.height,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    scene.image.pixels
  );
	glutSwapBuffers();
}

void glutMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		std::cout << x << ", " << y << std::endl;
	}
}

void Show() {
	int argc = 1;
	char argstr[] = "raytrace";
	char* argv = argstr;
	glutInit(&argc,&argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(
		(glutGet(GLUT_SCREEN_WIDTH) - scene.image.width) / 2,
		(glutGet(GLUT_SCREEN_HEIGHT) - scene.image.height) /2
	);
	glutInitWindowSize(scene.image.width, scene.image.height);
	glutCreateWindow("illumin8r");
  glutKeyboardFunc(glutKeyboard);
	glutDisplayFunc(glutDisplay);
	glutMouseFunc(glutMouse);
	glutMainLoop();
}
