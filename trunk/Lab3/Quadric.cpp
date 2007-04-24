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

Quadric::Quadric(const Matrix4x4<float> & q){
  this->mQuadric = q;
}

Quadric::~Quadric(){}

/*!
 * Evaluation of world coordinates are done through either transformation
 * of the world-coordinates by mWorld2Obj, or transformation of the quadric
 * coefficient matrix by getTransform() once.
 */
float Quadric::getValue(float x, float y, float z) const
{
  return 0;
}

/*!
 * Use the quadric matrix to evaluate the gradient.
 */
Vector3<float> Quadric::getGradient(float x, float y, float z, float d) const
{
  return Vector3<float>(0,0,0);
}

float Quadric::getCurvature(float x, float y, float z) const
{
  return Implicit::getCurvature(x, y, z);
}
