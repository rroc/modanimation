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
#ifndef __scalar_cutplane_h__
#define __scalar_cutplane_h__

#include "Geometry.h"
#include "Function3D.h"
#include "ColorMap.h"
#ifdef __APPLE__
#include "GLUT/glut.h"
#else
#include "GL/glut.h"
#endif


class ScalarCutPlane : public Geometry
{
protected :

  float mDx;
  const Function3D<float> * mFunction;
  const ColorMap * mMap;

  GLuint mTextureID;
  GLuint mWidth, mHeight;

public :

  ScalarCutPlane(float dx, const Function3D<float> * function, const ColorMap * map);

  virtual void draw();

  void update();

  virtual void setTransform(const Matrix4x4<float> & transform);

};

#endif
