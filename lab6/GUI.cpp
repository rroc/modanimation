/*************************************************************************************************
 *
 * Modeling and animation (TNM079) 2007
 * Code base for lab assignments. Copyright:
 *   Gunnar Johansson (gunnar.johansson@itn.liu.se)
 *   Ken Museth (ken.museth@itn.liu.se)
 *   Michael Bang Nielsen (bang@daimi.au.dk)
 *   Ola Nilsson (ola.nilsson@itn.liu.se)
 *   Andreas Sderstrm (andreas.soderstrom@itn.liu.se)
 *
 *************************************************************************************************/
#include "GUI.h"
#include <cstdlib>
#include <fstream>
#include <cmath>
#include "LevelSetOperator.h"
#include "OperatorDilateErode.h"
#include "OperatorReinitialize.h"
#include "OperatorAdvect.h"
#include "OperatorMeanCurvatureFlow.h"
#include "SignedDistanceSphere.h"
#include "ImplicitGradientField.h"
#include "ImplicitValueField.h"
#include "IsoContourColorMap.h"
#include "ConstantVectorField.h"
#include "LoopSubdivisionMesh.h"
#include "LineStrip.h"
#include "UniformCubicSpline.h"
#include "UniformCubicSplineSubdivisionCurve.h"

//GUI defines
#define DEFAULT_WINDOW_HEIGHT 768
#define DEFAULT_WINDOW_WIDTH 1024
#define X 0
#define Y 1
#define Z 2

#define NEAR_PLANE 0.05f
#define FAR_PLANE 100.0f

using namespace std;

//-----------------------------------------------------------------------------
GUI::GUI()
{
  mPlayback = false;
  mPlaybackIndex = 0;
  mDrawWireframe = false;
  mDrawXZPlane = true;
  mCurrentFPS = 0.0;
  mTimeSinceLastFPS = 0.0;
  mFramecounter = 0;

  // Starting global clock
  mClockArray[GLOBAL_CLOCK].start();
  mClockArray[ANIMATION_CLOCK].highPrecision(true);
  mClockArray[ANIMATION_CLOCK].start();
  mFrameTimestamp = mClockArray[ANIMATION_CLOCK].read();

  mCam = Camera(Vector3<float>(2, 1, -.1));

  mMousePos[X] = -1;
  mMousePos[Y] = -1;

  mWindowWidth = DEFAULT_WINDOW_WIDTH;
  mWindowHeight = DEFAULT_WINDOW_HEIGHT;

  mShowMenu = false;

  mMenu.addMenuLine("(q)   Exit");
  mMenu.addMenuLine("(p/P) Draw Plane");
  mMenu.addMenuLine("(W)   Camera Up");
  mMenu.addMenuLine("(S)   Camera Down");
  mMenu.addMenuLine("(w)   Accelerate Forwards");
  mMenu.addMenuLine("(s)   Accelerate Backwards");
  mMenu.addMenuLine("(a/A) Accelerate Left");
  mMenu.addMenuLine("(d/D) Accelerate Right");
  mMenu.addMenuLine("(o/O) Look at Origin");
  mMenu.addMenuLine("(x/X) Reset Camera");
  mMenu.addMenuLine("( )   Freeze");
  mMenu.addMenuLine("(.)   Dolly Pos.");
  mMenu.addMenuLine("(,)   Dolly Neg.");

  mMenu.addMenuLine("(m/M) Wireframe Render");

  mMenu.addMenuLine(" - - - - - - - - - ");
  mMenu.addMenuLine("(1)   Create a linestrip");
  mMenu.addMenuLine("(2)   Create a uniform cubic spline");
  mMenu.addMenuLine("(3)   Create a uniform subdivision cubic spline");
  mMenu.addMenuLine("(4)   Load Subdivision mesh (cube)");
  mMenu.addMenuLine("(5)   Loop subdivision");
  mMenu.addMenuLine("(6)   Load Adaptive subdivision mesh (cube)");
  mMenu.addMenuLine("(7)   Adaptive Loop Subdivision");
  mMenu.addMenuLine("(8)   ");
  mMenu.addMenuLine("(9)   ");

}

//-----------------------------------------------------------------------------
GUI::~GUI()
{
  std::vector<Object>::iterator iter = mGeometryList.begin();
  std::vector<Object>::iterator iend = mGeometryList.end();
  while (iter != iend) {
    delete (*iter).geometry;
    iter++;
  }
}

//-----------------------------------------------------------------------------
void GUI::init()
{
  unsigned int winPosX, winPosY;
  winPosX = 100;//mScreenWidth/2 - mWindowWidth/2;
  winPosY = 100;//mScreenHeight/2 - mWindowHeight/2;

  // Init glut and GL
  glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  // creating rendering window
  glutInitWindowPosition(100,100);
  glutInitWindowSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
  glutCreateWindow("Mesh Viewer");
  glutSetCursor(GLUT_CURSOR_CROSSHAIR);

  // initializing openGL
  glClearColor (0.53515625, 0.75390625f, 0.9609375f, 0.0);
  glEnable(GL_NORMALIZE);


  // Set default material
  GLfloat specular [] = { 0.5, 0.5, 0.5, 0.5 };
  GLfloat shininess [] = { 10.0 };
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  //glShadeModel(GL_FLAT);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Really nice perspective calculations

  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  glViewport(0,0,DEFAULT_WINDOW_WIDTH,DEFAULT_WINDOW_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (GLfloat)(DEFAULT_WINDOW_WIDTH)/(GLfloat)(DEFAULT_WINDOW_HEIGHT),NEAR_PLANE, FAR_PLANE);
  glMatrixMode(GL_MODELVIEW);

  glutPositionWindow(winPosX, winPosY);


}

//-----------------------------------------------------------------------------
void GUI::update()
{

  // Force redraw graphics
  glutPostRedisplay();
}

//-----------------------------------------------------------------------------
void GUI::displayFunc()
{
  // Time stuff
  Real timestamp = mClockArray[ANIMATION_CLOCK].read();
  Real dt = timestamp - mFrameTimestamp;
  mFrameTimestamp = timestamp;

  float drawFPSTime = timestamp - mTimeSinceLastFPS;
  static const float timelimit = 0.5;  // How often do we update the FPS count?
  if (drawFPSTime > timelimit){
    mCurrentFPS = mFramecounter/timelimit;
    mFramecounter = 0;
    mTimeSinceLastFPS = timestamp;
  }


  // initializing draw
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // light source
  GLfloat position [] = { 1.0, 1.0, 1.0, 1.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, position);
  glEnable(GL_LIGHT0);


  // Drawing XZ plane
  glDisable(GL_LIGHTING);
  if (mDrawXZPlane){
    drawXZplane(200,2.0,10);
  }
  glEnable(GL_LIGHTING);

  if (mShowMenu)
	  {
	  mMenu.draw();
	  }

  // Draw rotating cube
  //Real angle = 5*2*M_PI*mClockArray[ANIMATION_CLOCK].read();
  //drawCube(angle);


  // if fluid sim playback..
  if (mPlayback){
    float time = mClockArray[PLAYBACK_CLOCK].read()/3.0;

    for (unsigned int i = mPlaybackIndex; i+1 < mPlaybackMeshArray.size(); i++){
      if (mPlaybackMeshArray[i+1].timestamp >= time){
        mPlaybackIndex = i;
        break;
      }
    }
    if (mPlaybackMeshArray.size() != 0){
      mPlaybackMeshArray[mPlaybackIndex].mesh.draw();
    }

    if (mPlaybackMeshArray.size() == 1){
      mPlayback = false;
      mPlaybackIndex = 0;
    }

    // Stop
    if (mPlaybackIndex + 2 == mPlaybackMeshArray.size()){
      mPlayback = false;
      mPlaybackIndex = 0;
    }
  }
  else{
    // Draw geometry object(s)
    glColor3f(0,0,0.7);
    std::vector<Object>::iterator iter = mGeometryList.begin();
    std::vector<Object>::iterator iend = mGeometryList.end();
    while (iter != iend) {
      if (mDrawWireframe && (*iter).allowWireframe){
        glDisable(GL_LIGHTING);
        glDisable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }
      else{
        glEnable(GL_LIGHTING);
        glEnable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      }

      if ((*iter).draw){
        (*iter).geometry->draw();
      }
      iter++;
    }
  }


  // Draw fps
  glDisable(GL_LIGHTING);
  drawFPS(mCurrentFPS);
  glEnable(GL_LIGHTING);


  // Move observer
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // reset viewport and projection parameters
  glViewport(0,0,mWindowWidth,mWindowHeight);
  gluPerspective(45.0f, (GLfloat)(mWindowWidth)/(GLfloat)(mWindowHeight),NEAR_PLANE, FAR_PLANE);

  //Move observer
  mCam.advect(dt);
  gluLookAt(mCam.getPosition().x(), mCam.getPosition().y(), mCam.getPosition().z(),
            mCam.getLookAtPoint().x(), mCam.getLookAtPoint().y(), mCam.getLookAtPoint().z(),
            0,1,0);

  glMatrixMode(GL_MODELVIEW);

  mFramecounter++;

  glFlush();
  glutSwapBuffers();
}

//-----------------------------------------------------------------------------
void GUI::winReshapeFunc(GLint newWidth, GLint newHeight)
{
  mWindowWidth = newWidth;
  mWindowHeight = newHeight;

  // reset viewport and projection parameters
  glViewport(0,0,mWindowWidth,mWindowHeight);

  if (mWindowHeight != 0){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)(mWindowWidth)/(GLfloat)(mWindowHeight),NEAR_PLANE, FAR_PLANE);
    glMatrixMode(GL_MODELVIEW);
  }
}

//-----------------------------------------------------------------------------
void GUI::mouseFunc(GLint button, GLint action, GLint mouseX, GLint mouseY)
{
  if (mMousePos[X] == -1 && mMousePos[Y] == -1)
    {
      mMousePos[X] = mouseX;
      mMousePos[Y] = mouseY;

      mOldMousePos[X] = mouseX;
      mOldMousePos[Y] = mouseY;
    }
  else
    {
      mOldMousePos[X] = mMousePos[X];
      mOldMousePos[Y] = mMousePos[Y];

      mMousePos[X] = mouseX;
      mMousePos[Y] = mWindowHeight-mouseY;
    }

  glutPostRedisplay();
}

//-----------------------------------------------------------------------------
void GUI::mouseActiveMotionFunc(GLint mouseX, GLint mouseY)
{
  Real mx, my, mOldX, mOldY;
  if (mMousePos[X] == -1 && mMousePos[Y] == -1)
    {
      mMousePos[X] = mouseX;
      mMousePos[Y] = mouseY;

      mOldMousePos[X] = mouseX;
      mOldMousePos[Y] = mouseY;
    }
  else
    {
      mOldMousePos[X] = mMousePos[X];
      mOldMousePos[Y] = mMousePos[Y];

      mMousePos[X] = mouseX;
      mMousePos[Y] = mWindowHeight-mouseY;
    }

  //if (button == GLUT_LEFT_BUTTON)
  {
    getMouseScreenCoordinates(mOldMousePos[X], mOldMousePos[Y], mOldX,mOldY);
    getMouseScreenCoordinates(mMousePos[X], mMousePos[Y], mx,my);

    mCam.rotateXY((mOldX - mx)/2.0, (mOldY - my)/2.0);
  }

  glutPostRedisplay();
}

//-----------------------------------------------------------------------------
void GUI::mousePassiveMotionFunc(GLint mouseX, GLint mouseY)
{
  if (mMousePos[X] == -1 && mMousePos[Y] == -1)
    {
      mMousePos[X] = mouseX;
      mMousePos[Y] = mouseY;

      mOldMousePos[X] = mouseX;
      mOldMousePos[Y] = mouseY;
    }
  else
    {
      mOldMousePos[X] = mMousePos[X];
      mOldMousePos[Y] = mMousePos[Y];

      mMousePos[X] = mouseX;
      mMousePos[Y] = mWindowHeight-mouseY;
    }

  glutPostRedisplay();
}



//-----------------------------------------------------------------------------
void GUI::keyboardUpFunc(unsigned char keycode, GLint mouseX, GLint mouseY)
{
  mCam.stopAcc();
}

//-----------------------------------------------------------------------------
void GUI::keyboardFunc(unsigned char keycode, GLint mouseX, GLint mouseY)
{
  switch(keycode){
  case 'q' : case 'Q' :
    exit(0);
    break;
  case 'p' : case 'P' :
    mDrawXZPlane = !mDrawXZPlane;
    break;

  case 'W':
    {
      mCam.accUp();
      break;
    }
  case 'S':
    {
      mCam.accDown();
      break;
    }
  case 'w' :  // move forward
    {
      mCam.accForward();
      break;
    }
  case 's' :  // move backward
    {
      mCam.accBackward();
      break;
    }
  case 'a' : case 'A' :
    {
      mCam.accLeft();
      break;
    }
  case 'd' : case 'D' :
    {
      mCam.accRight();
      break;
    }
  case 'o' : case 'O' :  // center on origin
    {
      mCam.lookAtOrigo();
      break;
    }
  case ' ' : // full stop
    {
      mCam.stop();
    }
    break;
  case 'x' : case 'X' :  // return to original position
    {
      mCam.reset();
    }
    break;
  case '.' :
    {
      mCam.dolly(.1);
    }
    break;
  case ',' :
    {
      mCam.dolly(-.1);
    }
    break;
  case 'm' : case 'M':
    {
      // Wireframe rendering
      mDrawWireframe = true;
    }
    break;
  case 'n' : case 'N':
    {
      // Solid face rendering
      mDrawWireframe = false;
    }
    break;
  case 'b' : case 'B':
    {
    }
    break;

  case 'h' : case 'H':
	  {
	  mShowMenu = ! mShowMenu;
	  }
	  break;

  case '1' :
    {
      vector<Vector3<float> > c;
      c.push_back(Vector3<float>(1, 1, -.8));
      c.push_back(Vector3<float>(1, .8, -.7));
      c.push_back(Vector3<float>(1, .7, -.5));
      c.push_back(Vector3<float>(1, 1, -.5));
      c.push_back(Vector3<float>(1, .1, 0));
      c.push_back(Vector3<float>(1, .6, .3));
      c.push_back(Vector3<float>(1, .5, .4));
      c.push_back(Vector3<float>(1, 1, .5));
      LineStrip *ls = new LineStrip(c);
      addGeometry("Linestrip", ls);
    }
    break;

  case '2' :
    {
      vector<Vector3<float> > c;
      c.push_back(Vector3<float>(1, 1, -.8));
      c.push_back(Vector3<float>(1, .8, -.7));
      c.push_back(Vector3<float>(1, .7, -.5));
      c.push_back(Vector3<float>(1, 1, -.5));
      c.push_back(Vector3<float>(1, .1, 0));
      c.push_back(Vector3<float>(1, .6, .3));
      c.push_back(Vector3<float>(1, .5, .4));
      c.push_back(Vector3<float>(1, 1, .5));
      UniformCubicSpline *s = new UniformCubicSpline(c);
      addGeometry("Spline", s);
    }
    break;
  case '3' :
    {
      vector<Vector3<float> > c;
      c.push_back(Vector3<float>(1, 1, -.8));
      c.push_back(Vector3<float>(1, .8, -.7));
      c.push_back(Vector3<float>(1, .7, -.5));
      c.push_back(Vector3<float>(1, 1, -.5));
      c.push_back(Vector3<float>(1, .1, 0));
      c.push_back(Vector3<float>(1, .6, .3));
      c.push_back(Vector3<float>(1, .5, .4));
      c.push_back(Vector3<float>(1, 1, .5));
      UniformCubicSplineSubdivisionCurve * s2  = new UniformCubicSplineSubdivisionCurve(c);
      s2->subdivide(5);
      addGeometry("SSpline", s2);
    }
    break;
  case '4' :
    {
      std::cerr << "Loading subdivision mesh...\n";

      // Open input file
      std::string filename("../Objs/cow.obj");

      // Create new mesh
      LoopSubdivisionMesh * mesh = new LoopSubdivisionMesh();

      // Load mesh and add to geometry list
      std::ifstream infile;
      ObjIO objIO;
      infile.open(filename.c_str());
      objIO.load(mesh, infile);

      mesh->scale(0.2);
      mesh->validate();
      mesh->calculateVertexNormals();

      addGeometry("Subdivision", mesh);
    }
    break;
  case '5' :
    {
      std::cerr << "Subdiving mesh...";
      LoopSubdivisionMesh * mesh = getGeometry<LoopSubdivisionMesh>("Subdivision");
      mesh->subdivide();
      std::cerr << " done" << std::endl;

      mesh->validate();
      mesh->calculateVertexNormals();
    }
    break;
  case '6' :
    {
      std::cerr << "Loading adaptive subdivision mesh...\n";

      // Open input file
      std::string filename("Objs/cube.obj");

      // Create new mesh
      AdaptiveLoopSubdivisionMesh * mesh = new AdaptiveLoopSubdivisionMesh();

      // Load mesh and add to geometry list
      std::ifstream infile;
      ObjIO objIO;
      infile.open(filename.c_str());
      objIO.load(mesh, infile);

      mesh->scale(0.2);
      mesh->validate();
      mesh->calculateFaceNormals();

      addGeometry("AdaptiveSubdivision", mesh);
    }
    break;
  case '7' :
    {
      std::cerr << "Subdiving adaptive mesh...";
      AdaptiveLoopSubdivisionMesh * mesh = getGeometry<AdaptiveLoopSubdivisionMesh>("AdaptiveSubdivision");
      mesh->subdivide(10.0);
      std::cerr << " done" << std::endl;

      mesh->calculateFaceNormals();
    }
    break;
  case '8' :
    {
    }
    break;
  case '9' :
    {
    }
    break;
  case '-' :
    {
    }
    break;
  case '_' :
    {
    }
    break;

  case 'k' : case 'K' :
    {
    }
    break;
  case 'l' : case 'L' :
    {
    }
  }
  // Updating graphics
  glutPostRedisplay();
}

//-----------------------------------------------------------------------------
void GUI::specialFunc(GLint keycode, GLint mouseX, GLint mouseY)
{
}

//-----------------------------------------------------------------------------
void GUI::getMouseScreenCoordinates(int mouseX, int mouseY, Real &x, Real &y)
{
  // screen width = 4.0, screen height = 3.0, lower left corner = (0,0)
  x = 4.0*((Real)mouseX/(mWindowWidth));
  y = 3.0*((Real)mouseY/(mWindowHeight));
}

//-----------------------------------------------------------------------------
void GUI::drawXZplane(int nrOfGridCells, Real width, int subGridLines)
{
  Real spacing = width/(Real)nrOfGridCells;
  int counter;

  glBegin(GL_LINES);
  glColor3f(0.7f,0.7f,0.7f);
  // x sweep
  counter = 0;
  for (Real x = -width/2.0; x < width/2.0; x += spacing){
    if (counter >= subGridLines){
      glColor3f(0.3f,0.3f,0.3f);
      counter = 0;
    }
    else{
      glColor3f(0.7f,0.7f,0.7f);
    }
    glVertex3f(x,0.0f,-width/2.0);
    glVertex3f(x,0.0f,width/2.0);

    counter++;
  }
  // z sweep
  counter = 0;
  for (Real z = -width/2.0; z < width/2.0; z += spacing){
    if (counter >= subGridLines){
      glColor3f(0.3f,0.3f,0.3f);
      counter = 0;
    }
    else{
      glColor3f(0.7f,0.7f,0.7f);
    }
    glVertex3f(-width/2.0, 0.0f, z);
    glVertex3f(width/2.0, 0.0f, z);

    counter++;
  }

  // draw coordinate system
  //X-Axis
  glColor3f(1.0f,0.0f,0.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(width/2.0, 0.0f, 0.0f);

  //Y-axis
  glColor3f(0.0f,1.0f,0.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, width/2.0, 0.0f);

  //Z-axis
  glColor3f(0.0f,0.0f,1.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, width/2.0);

  glEnd();

  // Write axis info
  static const float distance = 0.1;
  glColor3f(1.0f,0.0f,0.0f);
  glRasterPos3f(distance,0,0);
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,'X');

  glColor3f(0.0f,1.0f,0.0f);
  glRasterPos3f(0,distance,0);
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,'Y');

  glColor3f(0.0f,0.0f,1.0f);
  glRasterPos3f(0,0,distance);
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,'Z');

}

//-----------------------------------------------------------------------------
void GUI::drawCube(Real angle)
{
  static const GLfloat vertices[][3] = {{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
                                        {1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
                                        {1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}};

  static const GLfloat normals[][3] = {{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
                                       {1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
                                       {1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}};

  static const GLfloat colors[][3] = {{0.0,0.0,0.0},{1.0,0.0,0.0},
                                      {1.0,1.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0},
                                      {1.0,0.0,1.0}, {1.0,1.0,1.0}, {0.0,1.0,1.0}};

  static const int polyList[][4] = { {0,3,2,1}, {2,3,7,6}, {0,4,7,3}, {1,2,6,5}, {4,5,6,7}, {0,1,5,4} };

  glPushMatrix();

  glScalef(0.1f,0.1f,0.1f);
  glTranslatef(0.0f, 5.0f,0.0f);
  glRotatef(angle,0.0f,1.0f,0.0f);
  glRotatef(angle,1.0f,1.0f,1.0f);

  // Draw the cube..
  for (int i = 0; i < 6; i++){
    const int a = polyList[i][0];
    const int b = polyList[i][1];
    const int c = polyList[i][2];
    const int d = polyList[i][3];
    glBegin(GL_POLYGON);
    glColor3fv(colors[a]);
    glNormal3fv(normals[a]);
    glVertex3fv(vertices[a]);
    glColor3fv(colors[b]);
    glNormal3fv(normals[b]);
    glVertex3fv(vertices[b]);
    glColor3fv(colors[c]);
    glNormal3fv(normals[c]);
    glVertex3fv(vertices[c]);
    glColor3fv(colors[d]);
    glNormal3fv(normals[d]);
    glVertex3fv(vertices[d]);
    glEnd();
  }

  glPopMatrix();
}

//-----------------------------------------------------------------------------
void GUI::drawFPS(float fps)
{
  const float pixelsPerLine = 48.0;
  float pixelsizeH = 1.0/mWindowHeight;
  //float pixelsizeW = 1.0/mWindowWidth;

  // Save matrices
  GLfloat modelview[16];
  GLfloat projection[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
  glGetFloatv(GL_PROJECTION_MATRIX, projection);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  //std::stringify FPS
  char buffer[10];
  sprintf(buffer, "%.0f", fps);
  std::string fpsStr(buffer);
  std::string result = std::string("FPS: ") + fpsStr;

  if (fps < 20){
    glColor3f(0.7,0,0.0);
  }
  else if (fps < 50){
    glColor3f(0.7,0.7,0.0);
  }
  else{
    glColor3f(0,0.5,0.0);
  }

  glRasterPos2f(-1,1.0-1*pixelsPerLine*pixelsizeH);
  for (unsigned int i = 0; i < result.length(); i++){
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,(result.c_str())[i]);
  }

  // Restore matrices
  glLoadMatrixf(projection);
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(modelview);
}


void GUI::drawText(const Vector3<float> & pos, const char * str)
{
  glRasterPos3f(pos[0], pos[1], pos[2]);
  for (unsigned int i = 0; str[i] != '\n'; i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
}


void GUI::addGeometry(const std::string & name, Geometry * geometry, int order)
{
  mGeometryList.push_back(Object(name, geometry, order));
  std::sort(mGeometryList.begin(), mGeometryList.end());
}


template <class T> T * GUI::getGeometry(const std::string & name)
{
  std::vector<Object>::iterator iter = mGeometryList.begin();
  std::vector<Object>::iterator iend = mGeometryList.end();
  while (iter != iend) {
    if ((*iter).name == name)
      return dynamic_cast<T *>((*iter).geometry);
    iter++;
  }
  std::cerr << "Warning: cannot find '" << name << "' in geometry list!" << std::endl;
  return NULL;
}
