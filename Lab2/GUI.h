/*************************************************************************************************
 *
 * Modeling and animation (TNM079) 2007
 * Code base for lab assignments. Copyright:
 *   Gunnar Johansson (gunnar.johansson@itn.liu.se)
 *   Ken Museth (ken.museth@itn.liu.se)
 *   Michael Bang Nielsen (bang@daimi.au.dk)
 *   Ola Nilsson (ola.nilsson@itn.liu.se)
 *   Andreas Söderström (andreas.soderstrom@itn.liu.se)
 *
 *************************************************************************************************/
#ifndef __GUI_H__
#define __GUI_H__

#include <vector>

#ifdef __APPLE__
#include "GLUT/glut.h"
#else
#include "GL/glut.h"
#endif

#include "Vector3.h"

#include "Geometry.h"
#include "SimpleMesh.h"
#include "HalfEdgeMesh.h"
#include "SimpleDecimationMesh.h"
#include "QuadricDecimationMesh.h"
#include "ObjIO.h"

#include "Stopwatch.h"
#include "Util.h"
#include "Camera.h"

class GUI
{
  typedef float Real;
  typedef short Index;

public:
  GUI();
  ~GUI();
  void init();
  void update();

  void displayFunc();
  void winReshapeFunc(GLint newWidth, GLint newHeight);
  void mouseFunc(GLint button, GLint action, GLint mouseX, GLint mouseY);
  void mouseActiveMotionFunc(GLint mouseX, GLint mouseY);
  void mousePassiveMotionFunc(GLint mouseX, GLint mouseY);

  void keyboardUpFunc(unsigned char keycode, GLint mouseX, GLint mouseY);
  void keyboardFunc(unsigned char keycode, GLint mouseX, GLint mouseY);
  void specialFunc(GLint keycode, GLint mouseX, GLint mouseY);

  void idleFunc();

  static void drawText(const Vector3<float> & pos, const char * str);

private:
  void getMouseScreenCoordinates(int mouseX, int mouseY, Real &x, Real &y);
  void drawXZplane(int nrOfGridCells, Real width, int subGridLines);
  void drawCube(Real angle);
  void drawFPS(float fps);

  // GUI parameters
  unsigned int mWindowHeight;
  unsigned int mWindowWidth;
  int mMousePos[2];
  int mOldMousePos[2];

  // Observer position and motion
  Camera mCam;

  // used to restore window after full-screen mode
  int mOldWindowHeight;
  int mOldWindowWidth;

  // Clocks and time
  Real mFrameTimestamp;  // timestamp for last draw.. (seconds)
  Stopwatch mClockArray[256];  // 256 clocks..

  #define GLOBAL_CLOCK 0   		// starts running at startup..
  #define ANIMATION_CLOCK 1		// Used for camera movement

  #define STOPWATCH 255


  // Misc variables
  bool mDrawXZPlane;
  unsigned int mFramecounter;
  float mTimeSinceLastFPS;
  float mCurrentFPS;


  // Geometry list
  std::vector<Geometry*> mGeometryList;

};


#endif
