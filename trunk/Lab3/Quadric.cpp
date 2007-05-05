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
#include "Quadric.h"

Quadric::Quadric(const Matrix4x4<float> & q)
	{
	const float side(1.0f);
	this->mQuadric = q;
	this->mBox = Bbox(Vector3<float>(-side, -side, -side), Vector3<float>(side,side,side));
	}

Quadric::~Quadric(){}

/*!
* Evaluation of world coordinates are done through either transformation
* of the world-coordinates by mWorld2Obj, or transformation of the quadric
* coefficient matrix by getTransform() once.
*/
float Quadric::getValue(float x, float y, float z) const
	{
	Vector4<float> vprim(x, y, z, 1.f);
	return vprim * (mQuadricPrime * vprim);
	}

/*!
* Use the quadric matrix to evaluate the gradient.
*/
Vector3<float> Quadric::getGradient(float x, float y, float z, float delta) const
	{
	Vector4<float> p(x, y, z, 1.f);	
	p = mQuadric * ( mWorld2Obj * p);
	return 2.0f *( Vector3<float> ( p[0],p[1],p[2] ) );
	}

float Quadric::getCurvature(float x, float y, float z) const
	{
	return Implicit::getCurvature(x, y, z);
	}

void Quadric::setTransform( const Matrix4x4<float> & transform )
	{
	Implicit::setTransform(transform);
	Matrix4x4<float> tr = mWorld2Obj.transpose();
	mQuadricPrime = tr*mQuadric*mWorld2Obj;
	}
