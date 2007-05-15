#include "FluidSimSetup.h"
#include "Quadric.h"
#include "OperatorReinitialize.h"

FluidSimSetup::FluidSimSetup(float dx, float bandWidth) : mDx(dx), mBandWidth(bandWidth)
{
}

FluidSimSetup::~FluidSimSetup()
{
}

LevelSet* FluidSimSetup::getSimpleSolid()
{
	Vector3<float> lowP(-1,0,-1);
	Vector3<float> highP(1,1,1);

	const float dx = mDx;

  /*Sphere* s = new Sphere(0.20);
	s->translate(0.0, 0.3, 0);
	s->setBoundingBox(Bbox(lowP, highP));*/
	Matrix4x4<float> matrix;
	matrix[0][0] = 0.0;
	matrix[1][1] = 0.0;
	matrix[2][2] = 0.0;
	matrix[3][3] = 0.0;

	matrix[0][3] = 0.0;
	matrix[1][3] = 0.5;
	matrix[2][3] = 0.0;


	Quadric* q = new Quadric(matrix);
	q->translate(0.0, 0.4, 0);
	q->setBoundingBox(Bbox(lowP, highP));

	LevelSet* ls = new LevelSet(dx, *q);
  ls->setNarrowBandWidth(mBandWidth);
  OperatorReinitialize opReInit(ls);
  opReInit.propagate(4*ls->getDx());

	ls->triangulate<SimpleMesh>(ls->getDx());
	ls->getMesh<SimpleMesh>().calculateFaceNormals();

  return ls;

}

VolumeLevelSet* FluidSimSetup::getSimpleFluid()
{
	Vector3<float> lowP(-1,0,-1);
	Vector3<float> highP(1,1,1);

	const float dx = mDx;

  Sphere* s = new Sphere(0.2, true);
	s->translate(0.0, 0.70, 0);
	s->setBoundingBox(Bbox(lowP, highP));

	VolumeLevelSet* ls = new VolumeLevelSet(dx, *s);
  ls->setNarrowBandWidth(mBandWidth);

	ls->triangulate<SimpleMesh>(ls->getDx());
	ls->getMesh<SimpleMesh>().calculateFaceNormals();

  return ls;
}

LevelSet* FluidSimSetup::getFluidBoxSolid()
{
	Vector3<float> lowP(-1,0,-1);
	Vector3<float> highP(1,1,1);

	const float dx = mDx;

  Cube* cube1 = new Cube();
  cube1->translate(0.0, 0.5, 0);
  cube1->scale(0.4,0.8,0.8);

  Cube* cube2 = new Cube();
  cube2->translate(0.0, 0.5, 0);
  cube2->scale(0.45,0.85,0.85);


  Difference* box = new Difference(cube2,cube1);

  box->setBoundingBox(Bbox(lowP, highP));
	LevelSet* ls = new LevelSet(dx, *box);
  ls->setNarrowBandWidth(mBandWidth);

  OperatorReinitialize opReInit(ls);
  opReInit.propagate(4*ls->getDx());

	ls->triangulate<SimpleMesh>(ls->getDx());
	ls->getMesh<SimpleMesh>().calculateFaceNormals();

	return ls;
}

VolumeLevelSet* FluidSimSetup::getFluidBoxFluid()
{
	Vector3<float> lowP(-1,0,-1);
	Vector3<float> highP(1,1,1);

	const float dx = mDx;

  Cube* cube = new Cube();
  cube->translate(0.0, 0.5, 0);
  cube->scale(0.4,0.6,0.2);

  Cube* cube2 = new Cube();
  cube2->translate(0.0, 0.15, 0);
  cube2->scale(0.4,0.1,0.8);

  Union* fluid = new Union(cube,cube2);
  fluid->setBoundingBox(Bbox(lowP, highP));

	VolumeLevelSet* ls = new VolumeLevelSet(dx, *fluid);
  ls->setNarrowBandWidth(mBandWidth);

  OperatorReinitialize opReInit(ls);
  opReInit.propagate(4*ls->getDx());

	ls->triangulate<SimpleMesh>(ls->getDx());
	ls->getMesh<SimpleMesh>().calculateFaceNormals();

	return ls;
}



VolumeLevelSet* FluidSimSetup::getComplexFluid()
{
	Vector3<float> lowP(-1,0,-1);
	Vector3<float> highP(1,1,1);

	// Create fluid
	const float dx = mDx;

  Sphere* s = new Sphere(0.15, true);
  //s->translate(0.0, 0.75, 0);
  s->translate(-0.2, 0.75, 0);
  s->setBoundingBox(Bbox(lowP, highP));

  VolumeLevelSet* ls = new VolumeLevelSet(dx, *s);
  ls->setNarrowBandWidth(mBandWidth);

	ls->triangulate<SimpleMesh>(ls->getDx());
	ls->getMesh<SimpleMesh>().calculateFaceNormals();

  return ls;
  //return s;
}

LevelSet* FluidSimSetup::getComplexSolid()
{
	Vector3<float> lowP(-1,0,-1);
	Vector3<float> highP(1,1,1);

  // Create solid
	const float dx = mDx;

  const float offset = 0.1;
  Sphere* s1 = new Sphere(1.0, true);
  s1->translate(0, 0.5+offset, 0);
  s1->scale(1,0.2,1);

  Sphere* s2 = new Sphere(1.0, true);
  s2->translate(0, 0.6+offset, 0);
  s2->scale(1,0.2,1);


  Sphere* s3 = new Sphere(0.15, true);
  s3->translate(-0.2, 0.4+offset, -0.2);

  Sphere* s4 = new Sphere(0.15, true);
  s4->translate(-0.2, 0.4+offset, 0.2);

  Sphere* s5 = new Sphere(0.15, true);
  s5->translate(0.2, 0.4+offset, 0.2);

  Sphere* s6 = new Sphere(0.15, true);
  s6->translate(0.2, 0.4+offset, -0.2);


  Sphere* s7 = new Sphere(0.05, true);
  s7->translate(0.0, 0.4+offset, 0.0);
  s7->scale(1,6,1);


  Sphere* s8 = new Sphere(0.7, true);
  s8->translate(0, 0.1+offset, 0);
  s8->scale(1,0.2,1);

  Sphere* s9 = new Sphere(0.7, true);
  s9->translate(0, 0.2+offset, 0);
  s9->scale(1,0.2,1);


	Difference* diff1 = new Difference(s1,s2);

	Union* u1 = new Union(diff1, s3);
	Union* u2 = new Union(u1, s4);
	Union* u3 = new Union(u2, s5);
	Union* u4 = new Union(u3, s6);

	Difference* diff2 = new Difference(u4,s7);

	Difference* diff3 = new Difference(s8,s9);

  Union* u5 = new Union(diff2, diff3);
	u5->setBoundingBox(Bbox(lowP, highP));

  LevelSet* solidLs = new LevelSet(dx, *u5);
  solidLs->setNarrowBandWidth(mBandWidth);

	solidLs->triangulate<SimpleMesh>(dx);
	solidLs->getMesh<SimpleMesh>().calculateFaceNormals();

  return solidLs;

}
