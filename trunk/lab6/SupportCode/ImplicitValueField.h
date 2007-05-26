/*************************************************************************************************
 *
 * Modeling and animation (TNM079) 2007
 * Code base for lab assignments. Copyright:
 *   Gunnar Johansson (gunnar.johansson@itn.liu.se)
 *   Ken Museth (ken.museth@itn.liu.se)
 *   Michael Bang Nielsen (bang@daimi.au.dk)
 *   Ola Nilsson (ola.nilsson@itn.liu.se)
 *   Andreas S�derstr�m (andreas.soderstrom@itn.liu.se)
 *
 *************************************************************************************************/
#ifndef __implicit_value_field_h__
#define __implicit_value_field_h__

#include "Function3D.h"
#include "Vector3.h"
#include "Implicit.h"

class ImplicitValueField : public Function3D<float>
{
protected :
  const Implicit * mImplicit;

public:
  ImplicitValueField(const Implicit * implicit) : mImplicit(implicit) { };
  virtual ~ImplicitValueField() {}

  //! Evaluate the function at x,y,z
  virtual float getValue(float x, float y, float z) const
  {
    return mImplicit->getValue(x,y,z);
  }

  //! Return a bound on the maximum value of the function
  virtual float getMaxValue() const
  {
    return 1;
  }
};

#endif
