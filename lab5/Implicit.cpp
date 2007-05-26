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
#include "Implicit.h"

#ifdef __APPLE__
#include "GLUT/glut.h"
#else
#include "GL/glut.h"
#endif

Implicit::Implicit() : mMesh(NULL) {}

Implicit::~Implicit()
	{
	if (mMesh != NULL) {
		delete mMesh;
		mMesh = NULL;
		}
	}

/*!
* Evaluation of world coordinates are done through transformation
* of the world-coordinates by mWorld2Obj and then evaluation.
*/
Vector3<float> Implicit::getGradient(float x, float y, float z, float delta) const
	{
	float deltaX0 = getValue(x+delta, y, z );
	float deltaX1 = getValue(x-delta, y, z );
	float deltaY0 = getValue(x, y+delta, z );
	float deltaY1 = getValue(x, y-delta, z );
	float deltaZ0 = getValue(x, y, z+delta );
	float deltaZ1 = getValue(x, y, z-delta );

	float dX = (deltaX0 - deltaX1 ) / (2.0f * delta) ;
	float dY = (deltaY0 - deltaY1 ) / (2.0f * delta) ;
	float dZ = (deltaZ0 - deltaZ1 ) / (2.0f * delta) ;

	return Vector3<float>(dX, dY, dZ);
	}


/*!
* Evaluation of world coordinates are done through transformation
* of the world-coordinates by mWorld2Obj and then evaluation.
*/
float Implicit::getCurvature(float x, float y, float z, float delta) const
	{
	double deltaX0 = getValue(x+delta, y, z );
	double deltaX1 = getValue(x-delta, y, z );
	double deltaY0 = getValue(x, y+delta, z );
	double deltaY1 = getValue(x, y-delta, z );
	double deltaZ0 = getValue(x, y, z+delta );
	double deltaZ1 = getValue(x, y, z-delta );
	double point   = getValue(x, y, z );
	double deltaInvSq = 1.0f/(delta*delta);

	float dX = ( deltaX0 - 2.0f*point + deltaX1 ) * deltaInvSq;
	float dY = ( deltaY0 - 2.0f*point + deltaY1 ) * deltaInvSq;
	float dZ = ( deltaZ0 - 2.0f*point + deltaZ1 ) * deltaInvSq;

	//printf("[%f](%f)\n", dX + dY + dZ, delta);
	return (dX + dY + dZ);
	}

/*!
* This method will loop over the vertices of the  mesh and draw
* a gradient vector at each vertex. The length is scaled to .1*normalized.
*
* NB. Visual studio has RTTI disabled by default. That needs to be turned on.
* Go to Project settings -> C/C++ -> Language and turn on "Enable Run Time Type Info"
*
*/
void Implicit::drawGradients(){
	if(typeid(*mMesh) == typeid(SimpleMesh)){

		SimpleMesh * ptr = static_cast<SimpleMesh*>(mMesh);
		const std::vector<Vector3<float> > verts = ptr->getVerts();
		const std::vector<Vector3<float> > normals = ptr->getNormals();

		glColor3f(1, 0, 0);
		glBegin(GL_LINES);

		//DRAW GRADIENTS
		for(unsigned int i=0; i < verts.size(); i++){
			const Vector3<float> v1 = verts.at(i);
			const Vector3<float> v2 = v1 + getGradient(v1.x(), v1.y(), v1.z()).normalize()*.1;
			glVertex3fv(v1.getArrayPtr());
			glVertex3fv(v2.getArrayPtr());
			}
		glEnd();

		//DRAW NORMALS
		if (verts.size() == normals.size()) {
			glColor3f(0, 0, 1);
			glBegin(GL_LINES);
			for(unsigned int i=0; i < normals.size(); i++){
				const Vector3<float> v1 = verts.at(i); 
				const Vector3<float> v2 = v1 + normals.at(i)*.1;
				glVertex3fv(v1.getArrayPtr());
				glVertex3fv(v2.getArrayPtr());
				}
			glEnd();
			}
		}
	else {
		std::cerr << "No drawGradients function implemented for mesh type: " << typeid(*mMesh).name() << std::endl;
		}
	}

/*!
* This method will loop over the vertices of the  mesh and draw
* a sphere with radius proportional to the curvature at te point.
*
* NB. Visual studio has RTTI disabled by default. That needs to be turned on.
* Go to Project settings -> C/C++ -> Language and turn on "Enable Run Time Type Info"
*
*/
void Implicit::drawCurvature(){
	if(typeid(*mMesh) == typeid(SimpleMesh)){

		SimpleMesh * ptr = static_cast<SimpleMesh*>(mMesh);
		const std::vector<Vector3<float> > verts = ptr->getVerts();

		for(unsigned int i=0; i < verts.size(); i++){
			const Vector3<float> v1 = verts.at(i);
			glPushMatrix();
			glTranslatef(v1.x(), v1.y(), v1.z());
			//glutSolidSphere(sqrtf(fabsf(getCurvature(v1.x(), v1.y(), v1.z(), 0.0001)))*.002, 5, 5);
			glutSolidSphere(sqrtf(fabsf(getCurvature(v1.x(), v1.y(), v1.z() )))*.0005, 10, 10);
			glPopMatrix();
			}
		}
	else {
		std::cerr << "No drawCurvature function implemented for mesh type: " << typeid(*mMesh).name() << std::endl;
		}
	}


void Implicit::draw(){
	if (mMesh == NULL)
		std::cerr << "Error: implicit geometry not triangulated, add call to triangulate()" << std::endl;
	else
		mMesh->draw();

	//  Turn these on to visualize gradients and curvature
	//drawGradients();
	//drawCurvature();

	// Draw bounding box for debugging
	Bbox b = getBoundingBox();

	Vector3<float> & v0 = b.pMin;
	Vector3<float> & v1 = b.pMax;

	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex3f(v0.x(), v0.y(), v0.z());
	glVertex3f(v1.x(), v0.y(), v0.z());
	glVertex3f(v1.x(), v1.y(), v0.z());
	glVertex3f(v0.x(), v1.y(), v0.z());
	glVertex3f(v0.x(), v0.y(), v0.z());
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(v0.x(), v0.y(), v1.z());
	glVertex3f(v1.x(), v0.y(), v1.z());
	glVertex3f(v1.x(), v1.y(), v1.z());
	glVertex3f(v0.x(), v1.y(), v1.z());
	glVertex3f(v0.x(), v0.y(), v1.z());
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(v0.x(), v0.y(), v0.z());
	glVertex3f(v0.x(), v0.y(), v1.z());

	glVertex3f(v1.x(), v0.y(), v0.z());
	glVertex3f(v1.x(), v0.y(), v1.z());

	glVertex3f(v0.x(), v1.y(), v0.z());
	glVertex3f(v0.x(), v1.y(), v1.z());

	glVertex3f(v1.x(), v1.y(), v0.z());
	glVertex3f(v1.x(), v1.y(), v1.z());
	glEnd();
	}


Bbox Implicit::getBoundingBox() const
	{
	// transform returns a copy
	return mBox.transform(getTransform());
	}


void Implicit::setBoundingBox(const Bbox &b)
	{
	mBox = b.transform(mWorld2Obj);
	}


void Implicit::setTransform(const Matrix4x4<float> & transform) {
	Geometry::setTransform(transform);
	mWorld2Obj = getTransform().inverse();
	}


void Implicit::transformWorld2Obj(float & x, float & y, float & z) const
	{
	Vector4<float> vprim, v = Vector4<float>(x, y, z, 1.f);
	vprim = mWorld2Obj*v;
	x = vprim[0]; y = vprim[1]; z = vprim[2];
	}
