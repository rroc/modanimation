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
	mMenu.addMenuLine("(1)   Load Bunny");
	mMenu.addMenuLine("(2)   Load Sphere");
	mMenu.addMenuLine("(3)   Load Cow");
	mMenu.addMenuLine("(4)   Load Spheres Fractal");
	mMenu.addMenuLine("(5)   Decimate To ...");
	mMenu.addMenuLine("(6)   Blended Union (Implicit Spheres)");
	mMenu.addMenuLine("(7)   Blended Intersection (Implicit Spheres)");
	mMenu.addMenuLine("(8)   Blended Difference (Implicit Spheres)");
	mMenu.addMenuLine("(9)   Quadric");
	}

//-----------------------------------------------------------------------------
GUI::~GUI()
	{
	for (unsigned int i = 0; i < mGeometryList.size(); i++){
		delete mGeometryList[i];
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

	//glCullFace(GL_BACK);
	//glEnable(GL_CULL_FACE);

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



	// Draw rotating cube
	//Real angle = 5*2*M_PI*mClockArray[ANIMATION_CLOCK].read();
	//drawCube(angle);


	// Draw geometry object(s)
	glColor3f(0,0,0.7);
	for (unsigned int i = 0; i < mGeometryList.size(); i++){
		mGeometryList[i]->draw();
		}


	// Draw fps and menu
	glDisable(GL_LIGHTING);
	drawFPS(mCurrentFPS);

	if (mShowMenu)
	{
		mMenu.draw();
	}

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
	  glDisable(GL_LIGHTING);
	  glDisable(GL_CULL_FACE);
	  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	  }
	  break;
  case 'n' : case 'N':
	  {
	  // Solid face rendering
	  glEnable(GL_LIGHTING);
	  glEnable(GL_CULL_FACE);
	  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	  }
	  break;

  case 'h' : case 'H':
	  {
		  mShowMenu = !mShowMenu;
	  }
	break;
  case '1' :
	  {
	  std::cerr << "Loading mesh...\n";

	  // Open input file
	  std::string filename("../Objs/bunnySmall.obj");

	  // Create new mesh
	  SimpleMesh* mesh = new SimpleMesh;

	  // Load mesh and add to geometry list
	  std::ifstream infile;
	  ObjIO objIO;
	  infile.open(filename.c_str());
	  objIO.load(mesh, infile);
	  mGeometryList.push_back(mesh);

	  // Calculate vertex normals
	  mesh->calculateVertexNormals();

	  // Calculate curvature
	  mesh->calcCurv();

	  // Move the mesh to a nice location
	  mesh->translate(0.5, 0, -0.5);
	  mesh->rotate(0, -0.15*M_PI, 0);
	  mesh->scale(0.1);
	  }
	  break;
  case '2' :
	  {
	  std::cerr << "Loading half edge mesh...\n";

	  // Open input file
	  std::string filename("../Objs/sphere1.0.obj");

	  // Create new mesh
	  HalfEdgeMesh* mesh = new HalfEdgeMesh;

	  // Load mesh and add to geometry list
	  std::ifstream infile;
	  ObjIO objIO;
	  infile.open(filename.c_str());
	  objIO.load(mesh, infile);

	  mesh->validate();
	  mesh->calculateFaceNormals();
	  mesh->calculateVertexNormals();

	  mesh->setShadingFlag(Mesh::SMOOTH_SHADING);

	  mGeometryList.push_back(mesh);
	  mesh->scale(0.1);
	  }
	  break;
  case '3' :
	  {
	  std::cerr << "Loading simple decimation mesh...\n";

	  // Open input file
	  std::string filename("../Objs/cow.obj");

	  // Create new mesh
	  SimpleDecimationMesh* mesh = new SimpleDecimationMesh;

	  // Load mesh and add to geometry list
	  std::ifstream infile;
	  ObjIO objIO;
	  infile.open(filename.c_str());
	  objIO.load(mesh, infile);

	  mesh->validate();
	  mesh->calculateFaceNormals();
	  mesh->calculateVertexNormals();

	  mesh->initialize();
	  mesh->setShadingFlag(Mesh::FLAT_SHADING);

	  mGeometryList.push_back(mesh);
	  }
	  break;
  case '4' :
	  {
//	  SimpleDecimationMesh * mesh = static_cast<SimpleDecimationMesh *>(mGeometryList[mGeometryList.size()-1]);
//	  mesh->decimate();

	  SphereFractal* sf = new SphereFractal(2);
	  sf->scale(1.0);

	  // Triangulate the sphere using a sample distance of 0.02
	  // using a SimpleMesh and compute face and vertex normals
	  //sf->triangulate<SimpleMesh>(0.02);
	  //sf->getMesh<SimpleMesh>().calculateFaceNormals();
	  //sf->getMesh<SimpleMesh>().calculateVertexNormals();
	  sf->triangulate<QuadricDecimationMesh>(0.1);
	  sf->getMesh<QuadricDecimationMesh>().calculateFaceNormals();
	  sf->getMesh<QuadricDecimationMesh>().calculateVertexNormals();

	  // Add it to the geometry list
	  mGeometryList.push_back(sf);
	  }
	  break;

  case '5' :
	  {
	  unsigned int targetFaces;
	  std::cout << "Enter target number of faces: ";
	  std::cin >> targetFaces;

	  if( mGeometryList.size()>0 )
		  {
		  SphereFractal* sf = static_cast<SphereFractal*>(mGeometryList[mGeometryList.size()-1]);
		  QuadricDecimationMesh& mesh = sf->getMesh<QuadricDecimationMesh>();
		  mesh.initialize();
		  mesh.decimate(targetFaces);
		  //mesh.cleanup();
		  }
	  }
	  break;
  case '6' :
	  {
	  // Create an implicit sphere and transform it
	  //   Sphere * s1 = new Sphere(1.0);
	  //   //s1->translate(0, 0, 0.1);
	  //   s1->scale(1.0);

	  //// Triangulate the sphere using a sample distance of 0.02
	  //// using a SimpleMesh and compute face and vertex normals
	  //s1->triangulate<SimpleMesh>(0.1);
	  //s1->getMesh<SimpleMesh>().calculateFaceNormals();
	  //s1->getMesh<SimpleMesh>().calculateVertexNormals();

	  //// Add it to the geometry list
	  //mGeometryList.push_back(s1);


	  Sphere * s1 = new Sphere(1.0);
	  s1->translate(0, 0, 0.1);
	  s1->scale(0.2);

	  // Create an implicit sphere and transform it
	  Sphere * s2 = new Sphere(1.0);
	  s2->translate(0, 0, -0.1);
	  s2->scale(0.2);

	  //Union* blob = new Union( s1, s2 );
	  BlendedUnion* blob = new BlendedUnion( s1, s2, 2 );

	  // Triangulate the sphere using a sample distance of 0.02
	     // using a SimpleMesh and compute face and vertex normals
	     blob->triangulate<SimpleMesh>(0.02);
	     blob->getMesh<SimpleMesh>().calculateFaceNormals();
	     blob->getMesh<SimpleMesh>().calculateVertexNormals();

	     // Add it to the geometry list
	     mGeometryList.push_back(blob);
	  }
	  break;
  case '7' :
	  {
	  // Create an implicit sphere and transform it
	  Sphere * s1 = new Sphere(1.0);
	  s1->translate(0, 0, 0.1);
	  s1->scale(0.2);

	  // Create an implicit sphere and transform it
	  Sphere * s2 = new Sphere(1.0);
	  s2->translate(0, 0, -0.1);
	  s2->scale(0.2);

	  BlendedIntersection* blob = new BlendedIntersection( s1, s2, 10 );

	  // Triangulate the sphere using a sample distance of 0.02
	  // using a SimpleMesh and compute face and vertex normals
	  blob->triangulate<SimpleMesh>(0.02);
	  blob->getMesh<SimpleMesh>().calculateFaceNormals();
	  blob->getMesh<SimpleMesh>().calculateVertexNormals();

	  // Add it to the geometry list
	  mGeometryList.push_back(blob);
	  }
	  break;
  case '8' :
	  {
	  // Create an implicit sphere and transform it
	  Sphere * s1 = new Sphere(1.0);
	  s1->translate(0, 0, 0.1);
	  s1->scale(0.2);

	  // Create an implicit sphere and transform it
	  Sphere * s2 = new Sphere(1.0);
	  s2->translate(0, 0, -0.1);
	  s2->scale(0.2);

	  BlendedDifference* blob = new BlendedDifference( s1, s2, 2 );

	  // Triangulate the sphere using a sample distance of 0.02
	  // using a SimpleMesh and compute face and vertex normals
	  blob->triangulate<SimpleMesh>(0.01);
	  blob->getMesh<SimpleMesh>().calculateFaceNormals();
	  blob->getMesh<SimpleMesh>().calculateVertexNormals();

	  // Add it to the geometry list
	  mGeometryList.push_back(blob);
	  }
	  break;
  case '9' :
	  {
	  float q[4][4] = {
		  {1.0f/(0.25*0.25),0,0,0},
		  {0,1.0f/(0.25*0.25),0,0},
		  {0,0,1.0f/(0.125*0.125),0},
		  {0,0,0,-1}};
		  //float q[4][4] = {
		  //	{1.0f,0,0,0},
		  //	{0,1.0f,0,0},
		  //	{0,0,0,0},
		  //	{0,0,0,-1}};

		  Matrix4x4<float> cylinder( q );

		  Quadric* quadric=new Quadric( cylinder );

		  quadric->scale(1.0);

		  quadric->triangulate<SimpleMesh>(0.02);
		  quadric->getMesh<SimpleMesh>().calculateFaceNormals();
		  quadric->getMesh<SimpleMesh>().calculateVertexNormals();


		  // Add it to the geometry list
		  mGeometryList.push_back(quadric);
	  }
	  break;

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