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
#ifndef __implicit_gradient_field_h__
#define __implicit_gradient_field_h__

#include "Function3D.h"
#include "Vector3.h"
#include "Implicit.h"

class ImplicitGradientField : public Function3D<Vector3<float> >
{
protected :
  const Implicit * mImplicit;

public:
  ImplicitGradientField(const Implicit * implicit) : mImplicit(implicit) { };
  virtual ~ImplicitGradientField() {}

  //! Evaluate the function at x,y,z
  virtual Vector3<float> getValue(float x, float y, float z) const
  {
    return mImplicit->getGradient(x,y,z)*0.1;
  }

  //! Return a bound on the maximum value of the function
  virtual Vector3<float> getMaxValue() const
  {
    return Vector3<float>(1);
  }
};

#endif
