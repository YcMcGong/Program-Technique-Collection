// Draw an Icosahedron
// ECE4893/8893 Project 4
// YOUR NAME HERE

#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

#define NFACE 20
#define NVERTEX 12

#define X .525731112119133606 
#define Z .850650808352039932

static int w = 1000;
static int h = 1000;

// These are the 12 vertices for the icosahedron
static GLfloat vdata[NVERTEX][3] = {    
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
};

// These are the 20 faces.  Each of the three entries for each 
// vertex gives the 3 vertices that make the face.
static GLint tindices[NFACE][3] = { 
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

int testNumber; // Global variable indicating which test number is desired

static GLfloat rotX = 0.0;
static GLfloat rotY = 0.0;
static GLfloat rotZ = 0.0;
static GLfloat angle = 0.0;
static GLfloat dX = 1.0;
static GLfloat dY = 0.8;
static GLfloat dZ = 0.6;
static GLfloat updateRate = 100.0;

static GLfloat maxDepth = 4;

void normalize(GLfloat v[3])
{
  GLfloat d = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
  if(d == 0.0) return;
  v[0] /=d;
  v[1] /=d;
  v[2] /=d;
}

void drawTriangle(GLfloat* v1, GLfloat* v2, GLfloat* v3)
{
  glBegin(GL_TRIANGLES);
  //glColor3f(0.0,0.0,1.0);
  //Color Drwaing
  GLfloat c1 =  v1[0] + v2[1] +v3[2];
  GLfloat c2 =  v1[1] + v2[2] +v3[3];
  GLfloat c3 =  v1[2] + v2[3] +v3[1];
  GLfloat Sum = c1 + c2 + c3;

  glColor3f(c1/Sum, c2/Sum, c3/Sum); 

  glNormal3fv(v1); glVertex3fv(v1);
  glNormal3fv(v2); glVertex3fv(v2);
  glNormal3fv(v3); glVertex3fv(v3);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glColor3f(1.0,1.0,1.0);
  glNormal3fv(v1); glVertex3fv(v1);
  glNormal3fv(v2); glVertex3fv(v2);
  glNormal3fv(v3); glVertex3fv(v3);
  glEnd();
}

void subDivide(GLfloat* v1, GLfloat* v2, GLfloat* v3, int depth)
{
  if(depth == 0)
  {
    drawTriangle(v1, v2, v3);
    return;
  }

  GLfloat v12[3];
  GLfloat v23[3];
  GLfloat v31[3];

  for(int i=0; i<3; ++i)
  {
    v12[i] = v1[i] +v2[i];
    v23[i] = v2[i] +v3[i];
    v31[i] = v3[i] +v1[i];
  }

  normalize(v12);
  normalize(v23);
  normalize(v31);

  subDivide(v1, v12, v31, depth-1);
  subDivide(v2, v23, v12, depth-1);
  subDivide(v3, v31, v23, depth-1);
  subDivide(v12, v23, v31, depth-1);
}

void drawIcosahedron(int maxDepth)
{
  for(int i=0; i<NFACE; i++)
  {
    subDivide(&vdata[tindices[i][0]][0],
              &vdata[tindices[i][1]][0],
              &vdata[tindices[i][2]][0], maxDepth);
  }
}


// Test cases.  Fill in your code for each test case
void Test1()
{
  maxDepth = 0;
}

void Test2()
{
  maxDepth = 0;
  rotX = 1.0;
  rotY = 1.0;

  if(angle >= 360.0f)
  angle = 0.0f;
  else angle++;
}

void Test3()
{
  maxDepth = 1;
}

void Test4()
{
  maxDepth = 1;
  rotX = 1.0;
  rotY = 1.0;

  if(angle >= 360.0f)
  angle = 0.0f;
  else angle++;
}

void Test5(int depth)
{

}

void Test6(int depth)
{
  rotX = 1.0;
  rotY = 1.0;

  if(angle >= 360.0f)
  angle = 0.0f;
  else angle++;
}

void display(void)
{
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

  switch (testNumber){
    case 1: Test1(); break;
    case 2: Test2(); break;
    case 3: Test3(); break;
    case 4: Test4(); break;
    case 5: Test5(maxDepth); break;
    case 6: Test6(maxDepth); break;
  }

  glRotatef(angle, rotX, rotY, rotZ);
  drawIcosahedron(maxDepth);
  glPopMatrix();
  //Test1();
  // Flush buffer
  //glFlush(); // If single buffering
  glutSwapBuffers(); // If double buffering
}

void init()
{
  //select clearing (background) color
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
}

void reshape(int w, int h)
{
  glViewport(0,0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, (GLdouble)w, (GLdouble)0.0, h, (GLdouble)-w, (GLdouble)w);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void timer(int)
{
  glutPostRedisplay();
  glutTimerFunc(1000.0 / updateRate, timer, 0);
}


int main(int argc, char** argv)
{
  if (argc < 2)
    {
      std::cout << "Usage: icosahedron testnumber" << endl;
      exit(1);
    }
  // Set the global test number
  testNumber = atol(argv[1]);

  if(argc == 3) maxDepth = atol(argv[2]);
  // Initialize glut  and create your window here
  // Set your glut callbacks here
  // Enter the glut main loop here

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(w, h);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("ICosahedron");
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutTimerFunc(1000.0 / updateRate, timer, 0);
  glutMainLoop();

  return 0;
}
