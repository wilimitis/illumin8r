#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <stdlib.h>
# ifdef __APPLE__
#  include <GLUT/glut.h>
# else
#  include <GL/glut.h>
# endif
#include "window.h"

#define ESC 27

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
	// glDrawPixels(
  //   renderImage.GetWidth(),
  //   renderImage.GetHeight(),
  //   GL_RGB,
  //   GL_UNSIGNED_BYTE,
  //   renderImage.GetPixels()
  // );
}

void Show(Image image)
{
	int argc = 1;
	char argstr[] = "raytrace";
	char *argv = argstr;
	glutInit(&argc,&argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	// if (glutGet(GLUT_SCREEN_WIDTH) > 0 && glutGet(GLUT_SCREEN_HEIGHT) > 0){
	// 	glutInitWindowPosition( (glutGet(GLUT_SCREEN_WIDTH) - camera.imgWidth)/2, (glutGet(GLUT_SCREEN_HEIGHT) - camera.imgHeight)/2 );
	// }
	// else glutInitWindowPosition( 50, 50 );
	// glutInitWindowSize(camera.imgWidth, camera.imgHeight);
	glutCreateWindow("illumin8r");
  glutKeyboardFunc(glutKeyboard);
	glutDisplayFunc(glutDisplay);
	glutMainLoop();
}