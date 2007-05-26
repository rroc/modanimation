#include "Cube.h"

Cube::Cube()
{
	Vector3<float> lowP(-1.1);
	Vector3<float> highP(1.1);
  setBoundingBox(Bbox(lowP, highP));

	Matrix4x4<float> matrix;
	matrix[0][0] = 0.0;
	matrix[1][1] = 0.0;
	matrix[2][2] = 0.0;
	matrix[3][3] = 0.0;

	matrix[0][3] = 0.0;
	matrix[1][3] = 0.5;
	matrix[2][3] = 0.0;

  // Plane 1
	Quadric* q = new Quadric(matrix);
	q->translate(0.0, 0.5, 0);
	q->setBoundingBox(Bbox(lowP, highP));
  mPlanes.push_back(q);

  // Plane 2
	matrix[0][3] = 0.0;
	matrix[1][3] = -0.5;
	matrix[2][3] = 0.0;
	q = new Quadric(matrix);
	q->translate(0.0, -0.5, 0);
	q->setBoundingBox(Bbox(lowP, highP));
  mPlanes.push_back(q);

  // Plane 3
	matrix[0][3] = 0.5;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
	q = new Quadric(matrix);
	q->translate(0.5, 0.0, 0);
	q->setBoundingBox(Bbox(lowP, highP));
  mPlanes.push_back(q);

  // Plane 4
	matrix[0][3] = -0.5;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
	q = new Quadric(matrix);
	q->translate(-0.5, 0, 0);
	q->setBoundingBox(Bbox(lowP, highP));
  mPlanes.push_back(q);


  // Plane 5
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.5;
	q = new Quadric(matrix);
	q->translate(0.0, 0.0, 0.5);
	q->setBoundingBox(Bbox(lowP, highP));
  mPlanes.push_back(q);

  // Plane 6
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = -0.5;
	q = new Quadric(matrix);
	q->translate(0.0, 0, -0.5);
	q->setBoundingBox(Bbox(lowP, highP));
  mPlanes.push_back(q);

}

Cube::~Cube()
{
  for (unsigned int i = 0; i < mPlanes.size(); i++){
    delete mPlanes[i];
  }
}


float Cube::getValue(float x, float y, float z) const
{
  // union..
  Vector4<float> vprim, v = Vector4<float>(x, y, z, 1.f);
  vprim = mWorld2Obj*v;


  float value = mPlanes[0]->getValue(vprim[0], vprim[1], vprim[2]);
  for (unsigned int i = 1; i < mPlanes.size(); i++){
    value = std::max(value, mPlanes[i]->getValue(vprim[0], vprim[1], vprim[2]));
  }


  return value;
}
