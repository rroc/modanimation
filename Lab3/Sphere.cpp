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
#include "Sphere.h"

Sphere::Sphere(float r)
{
  this->radius2 = r*r;
  this->mBox = Bbox(Vector3<float>(-r, -r, -r), Vector3<float>(r, r, r));
}

Sphere::~Sphere() { }

float Sphere::getValue(float x, float y, float z) const
{
  Vector4<float> vprim, v = Vector4<float>(x, y, z, 1.f);
  vprim = mWorld2Obj*v;
  return (vprim*vprim - 1.f - radius2);
}

