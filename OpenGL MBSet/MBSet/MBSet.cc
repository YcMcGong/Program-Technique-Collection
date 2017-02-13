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

/*Mouse Actions */
// GLsizei MOUSEx=0, MOUSEy=0;
// GLfloat SIDE=30;
GLfloat RED[3] = {1.0,0.0,0.0};
int HEIGHT = 256;
float cx;
float cy;
float mx;
float my;
int cx_int;
int cy_int;
int mx_int;
int my_int;
//-------------------------------------Done Mouse Action Definition

using namespace std;
bool draw =false;
static GLfloat updateRate = 100.0;
// Min and max complex plane values
Complex  minC(-2.0, -1.2);
Complex  maxC( 1.0, 1.8);
double minC_real = -2.0;
double minC_imag = -1.2;
double maxC_real = 1.0;
double maxC_imag = 1.8;
int      maxIt = 2000;     // Max iterations for the set computations

static int w = 512;
static int h = 512;

static float w_f = 512.0/2;
static float h_f = 512.0/2;

Complex C_p[512][512];
Complex C_p_last[512][512];
float Color_Map[512][512][3];
float Color_Map_last[512][512][3];

float color_set[2000][3];

void init_color()
{
  for(int i=0; i<5; ++i)
  {
    color_set[i][0]=1.0;
    color_set[i][1]=1.0;
    color_set[i][2]=1.0;
  }
  for(int i=5; i<2000; ++i)
  {
    color_set[i][0]=(float)(rand()%100)/100.0;
    color_set[i][1]=(float)(rand()%100)/100.0;
    color_set[i][2]=(float)(rand()%100)/100.0;
  }
}

void init_C_p()
{
  double step = (maxC_real-minC_real)/512.0;
  for(int i=0; i<w; ++i)
  {
    for(int k=0; k<h; ++k)
    {
      C_p[i][k].real = ((double) i)*step + (minC_real);
      C_p[i][k].imag = ((double) k)*step + (minC_imag);
    }
  }
}

void color_deside(int i,int k)
{
  // float a = (i + k)/(1024.0);
  // float b = (i + 2*k)/(512.0*3);
  // float c = (2*i + k)/(512.0*3);
  // int iter = 0;
  Complex c = C_p[i][k];
  Complex Zn = c;
  for(int iter = 0; iter<2000; ++iter)
  {
    Zn = Zn * Zn + c;
    if(Zn.Mag2()>4.0) 
    {
      Color_Map[i][k][0] = color_set[iter][0];
      Color_Map[i][k][1] = color_set[iter][1];
      Color_Map[i][k][2] = color_set[iter][2];
      return;
    }
  }
  Color_Map[i][k][0] = 0;
  Color_Map[i][k][1] = 0;
  Color_Map[i][k][2] = 0;
  return;
}

void* color_select_16_threads(void* v)
{
  unsigned long n = (unsigned long)v; 
  int num = (int) n;
  for (int i=num*32; i<((num+1)*32); ++i)
  {
    for (int k=0; k<h; ++k)
    {
      color_deside(i,k);
    }
  }

}

void CreateMBSet()
{
  pthread_t pt[16];
  for (int i=0; i<16; ++i)
  {
    pthread_create(&pt[i], 0, color_select_16_threads, (void*)i);
  }
  for (int i = 0; i < 16; i++) pthread_join(pt[i], NULL);
}

void drawSquare()
{
    glColor3f(1.0,0.0,0.0);
    glBegin(GL_LINE_LOOP);
        glVertex2f((float)mx/w_f, (float)my/h_f);
        glVertex2f((float)cx/w_f, (float)my/h_f);
        glVertex2f((float)cx/w_f, (float)cy/h_f);
        glVertex2f((float)mx/w_f, (float)cy/h_f);
    glEnd();
    glFlush();
}   


void drawMBSet()
{
  glBegin(GL_POINTS);
  for (int i=0; i<w; ++i)
  {
    for (int k=0; k<h; ++k)
    {
      glColor3f(Color_Map[i][k][0],Color_Map[i][k][1],Color_Map[i][k][2]);
      glVertex2f(((float)i-w_f)/w_f,((float)k-h_f)/h_f);
    }
  }
  glEnd();
}


void display(void)
{ // Your OpenGL display code here
  // clear all
  glEnable(GL_LINE_SMOOTH);
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);
  // Clear the matrix
  //glEnable(GL_DEPTH_TEST);
  glColor3f(1.0,1.0,1.0);
  glLoadIdentity();
  // Set the viewing transformation
  gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glPushMatrix();
  glTranslatef(w/2, h/2, 0);
  glScalef(w/2, w/2, 0);
  drawMBSet();
  if(draw == true) drawSquare();
  glPopMatrix();
  glutSwapBuffers(); 
}

void init()
{ // Your OpenGL initialization code here
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  init_color();
  init_C_p();
  CreateMBSet();
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

void spindisplay(void)
{       
    glutPostRedisplay();
}

void Re_Calculate_Color_Map()
{ 
  float x1 = mx/w_f;
  float x2 = cx/w_f;
  float y1 = my/h_f;
  float y2 = cy/h_f;

  float x_min = min(x1,x2);
  float y_min = min(y1,y2);
  float x_max = max(x1,x2);
  float y_max = max(y1,y2);

  int x_min_int = (int) (x_min*256+256);
  int y_min_int = (int) (y_min*256+256);
  int x_max_int = (int) (x_max*256+256);
  int y_max_int = (int) (y_max*256+256);

  minC_real = C_p[x_min_int][y_min_int].real;
  minC_imag = C_p[x_min_int][y_min_int].imag;
  maxC_real = C_p[x_max_int][y_max_int].real;

  cout<<x_min_int<<endl;
  cout<<y_min_int<<endl;
  cout<<x_max_int<<endl;
  cout<<y_max_int<<'\n'<<endl;

  init_C_p();
  CreateMBSet();
}

void Save_Backup()
{
   for(int i=0; i<512; i++)
   {
     for(int k=0; k<512; k++)
     {
       C_p_last[i][k] = C_p[i][k];
       Color_Map_last[i][k][0] = Color_Map[i][k][0];
       Color_Map_last[i][k][1] = Color_Map[i][k][1];
       Color_Map_last[i][k][2] = Color_Map[i][k][2];
     }
   }

}

void Reverse_Backup()
{
  for(int i=0; i<512; i++)
  {
     for(int k=0; k<512; k++)
     {
       C_p[i][k] = C_p_last[i][k];
       Color_Map[i][k][0] = Color_Map_last[i][k][0];
       Color_Map[i][k][1] = Color_Map_last[i][k][1];
       Color_Map[i][k][2] = Color_Map_last[i][k][2];
     }
  }
  
}

void mouse(int button, int state, int x, int y)
{ // Your mouse click processing here
  // state == 0 means pressed, state != 0 means released
  // Note that the x and y coordinates passed in are in
  // PIXELS, with y = 0 at the top.
  if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)   
    {
        draw =true;
        mx = (float)(x - w_f);
        my = (float)(h_f - y);
    }

    else if(button==GLUT_LEFT_BUTTON && state==GLUT_UP)   
    {
      draw = false;
      Save_Backup();
      Re_Calculate_Color_Map();
    }

}

void motion(int x, int y)
{ // Your mouse motion here, x and y coordinates are as above
  cx = (float)(x - w_f);
  cy = (float)(h_f - y);
  if(abs(my-cy)>abs(mx-cx)) {cy = my + mx -cx;}
  else if (abs(my-cy)<abs(mx-cx)) {cx = mx + my -cy;}
}

void keyboard(unsigned char c, int x, int y)
{ // Your keyboard processing here
  if((c == 'B')||(c=='b')) {Reverse_Backup();}
}

void timer(int)
{
  glutPostRedisplay();
  glutTimerFunc(1000.0 / updateRate, timer, 0);
}


int main(int argc, char** argv)
{
  // Initialize OpenGL, but only on the "master" thread or process.
  // See the assignment writeup to determine which is "master" 
  // and which is slave.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(w, h);
  // glutInitWindowPosition(w, h);
  glutCreateWindow("MBSet");
  init();
  glutDisplayFunc(display);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(spindisplay);
  glutReshapeFunc(reshape);
  glutTimerFunc(1000.0 / updateRate, timer, 0);
  glutMainLoop();
  return 0;
}

