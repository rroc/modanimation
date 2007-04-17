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
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include "GUI.h"

using namespace std;

static const int defaultScreenWidth = 800;
static const int defaultScreenHeight = 600;

GUI mainGUI;

//-----------------------------------------------------------------------------
void displayFunc()
{
  mainGUI.displayFunc();
}

//-----------------------------------------------------------------------------
void winReshapeFunc(int newWidth, int newHeight)
{
  mainGUI.winReshapeFunc(newWidth, newHeight);
}

//-----------------------------------------------------------------------------
void mouseFunc(int button, int action, int mouseX, int mouseY)
{
  mainGUI.mouseFunc(button, action, mouseX, mouseY);
}

//-----------------------------------------------------------------------------
void mouseActiveMotionFunc(int mouseX, int mouseY)
{
  mainGUI.mouseActiveMotionFunc(mouseX, mouseY);
}

//-----------------------------------------------------------------------------
void mousePassiveMotionFunc(int mouseX, int mouseY)
{
  mainGUI.mousePassiveMotionFunc(mouseX, mouseY);
}

//-----------------------------------------------------------------------------
void keyboardUpFunc(unsigned char keycode, int mouseX, int mouseY)
{
  mainGUI.keyboardUpFunc(keycode, mouseX, mouseY);
}

//-----------------------------------------------------------------------------
void keyboardFunc(unsigned char keycode, int mouseX, int mouseY)
{
  mainGUI.keyboardFunc(keycode, mouseX, mouseY);
}

//-----------------------------------------------------------------------------
void specialFunc(int keycode, int mouseX, int mouseY)
{
  mainGUI.specialFunc(keycode, mouseX, mouseY);
}

//-----------------------------------------------------------------------------
void idleFunc()
{
  mainGUI.update();
}

//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  glutInit (&argc, argv);
  mainGUI.init();

  // glut callback functions
  glutDisplayFunc(displayFunc);
  glutReshapeFunc(winReshapeFunc);
  glutMouseFunc(mouseFunc);
  glutMotionFunc(mouseActiveMotionFunc);
  glutPassiveMotionFunc(mousePassiveMotionFunc);
  //glutKeyboardUpFunc(keyboardUpFunc);
  glutKeyboardFunc(keyboardFunc);
  glutSpecialFunc(specialFunc);
  glutIdleFunc(idleFunc);
  
  // Start main loop      
  glutMainLoop();

  return 0;
}
