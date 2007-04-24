#include "SphereFractal.h"
#include <fstream>

using namespace std;

SphereFractal::SphereFractal(int level)
{

  // Reading fractal from file..
  ifstream inFile;

  if (level < 2) {level = 2;}
  if (level > 5) {level = 5;}

  if (level == 2){
    inFile.open("SupportCode/balls2levels.txt");
  }
  else if (level == 3){
    inFile.open("SupportCode/balls3levels.txt");
  }
  else if (level == 4){
    inFile.open("SupportCode/balls4levels.txt");
  }
  else if (level == 5){
    inFile.open("SupportCode/balls5levels.txt");
  }


  while (inFile.good()){
    float tx;
    inFile >> tx;

    if (!inFile.good()) break;

    float ty;
    inFile >> ty;

    if (!inFile.good()) break;

    float tz;
    inFile >> tz;

    if (!inFile.good()) break;

    float r;
    inFile >> r;

    if (!inFile.good()) break;



    // Building transform (position and scale the unit sphere)
    Matrix4x4<float> transform;
    transform = transform*transform.translation(tx,ty,tz);
    transform = transform*transform.scale(r);

    // Add sphere to array of spheres
    Implicit* sphere = new Sphere(1.0);
    sphere->setTransform(transform);
    mSpheres.push_back(sphere);
  }


  // Building fractal
  mFractal = buildFractal();

  // Setting bounding box for sphere fractal object
  Bbox box = mFractal->getBoundingBox();

  float maxDist = abs(box.pMax[0] - box.pMin[0]);
  maxDist = max(maxDist, abs(box.pMax[1] - box.pMin[1]));
  maxDist = max(maxDist, abs(box.pMax[2] - box.pMin[2]));

  box.pMin -= maxDist*0.05;
  box.pMax += maxDist*0.05;
  setBoundingBox(box);

}

SphereFractal::~SphereFractal()
{
  for (unsigned int i = 0; i < mSpheres.size(); i++){
    delete mSpheres[i];
  }
}

Implicit*  SphereFractal::buildFractal()
{
  Union* u = new Union(mSpheres[0], mSpheres[1]);

  for (unsigned int i = 2; i < mSpheres.size(); i++){
     u = new Union(u, mSpheres[i]);
  }

  return u;
}

float SphereFractal::getValue(float x, float y, float z) const
{
  return mFractal->getValue(x,y,z);
}

