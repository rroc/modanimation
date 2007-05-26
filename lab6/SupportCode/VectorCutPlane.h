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
#ifndef __vector_cutplane_h__
#define __vector_cutplane_h__

#include "Geometry.h"
#include "Function3D.h"
#include "ColorMap.h"
#include "Vector3.h"
#include <vector>

class VectorCutPlane : public Geometry
{
protected :

  float mDx;
  const Function3D<Vector3<float> > * mFunction;
  const ColorMap * mMap;

  std::vector<Vector3<float> > mVectors;

public :

  VectorCutPlane(float dx, const Function3D<Vector3<float> > * function, const ColorMap * map);

  virtual void draw();

  void update();

  virtual void setTransform(const Matrix4x4<float> & transform);

};

#endif
