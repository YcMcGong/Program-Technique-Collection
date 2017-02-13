// Calculate and display the Mandelbrot set
// ECE4893/8893 final project, Fall 2011

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "complex.h"

using namespace std;

// Min and max complex plane values
Complex  minC(-2.0, -1.2);
Complex  maxC( 1.0, 1.8);
int      maxIt = 2000;     // Max iterations for the set computations

static int w = 512;
static int h = 512;

void drawMBSet()
{
  glBegin(GL_TRIANGLES);
  glVertex3fv(3.0,3.0,3.0);
  glVertex3fv(13.0,13.0,13.0);
  glVertex3fv(9.0,9.0,9.0);
  glEnd();
}

void display(void)
{ // Your OpenGL display code here
  // clear all
  glEnable(GL_LINE_SMOOTH);
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);
  // Clear the matrix
  glEnable(GL_DEPTH_TEST);
  glColor3f(1.0,1.0,1.0);
  glLoadIdentity();
  // Set the viewing transformation
  gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glPushMatrix();
  glTranslatef(w/2, h/2, 0);
  glScalef(w/2, w/2, w/2);
  drawMBSet();
  glPopMatrix();
  glutSwapBuffers(); 
}

void init()
{ // Your OpenGL initialization code here
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
}

void reshape(int w, int h)
{ // Your OpenGL window reshape code here
  glViewport(0,0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, (GLdouble)w, (GLdouble)0.0, h, (GLdouble)-w, (GLdouble)w);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void mouse(int button, int state, int x, int y)
{ // Your mouse click processing here
  // state == 0 means pressed, state != 0 means released
  // Note that the x and y coordinates passed in are in
  // PIXELS, with y = 0 at the top.
}

void motion(int x, int y)
{ // Your mouse motion here, x and y coordinates are as above
}

void keyboard(unsigned char c, int x, int y)
{ // Your keyboard processing here
}

int main(int argc, char** argv)
{
  // Initialize OpenGL, but only on the "master" thread or process.
  // See the assignment writeup to determine which is "master" 
  // and which is slave.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(w, h);
  glutInitWindowPosition(w, h);
  glutCreateWindow("MBSet");
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  //glutTimerFunc(1000.0 / updateRate, timer, 0);
  glutMainLoop();
  return 0;
}

