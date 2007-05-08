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
#ifndef __function_3d_h__
#define __function_3d_h__

/*! \brief Base class for functions in R3
 * A Function3D can be evaluated anywhere in space.
 */
template<class T>
class Function3D{
public:
  //! Evaluate the function at x,y,z
  virtual T getValue(float x, float y, float z) const = 0;
  //! Return a bound on the maximum value of the function
  virtual T getMaxValue() const = 0;
  virtual ~Function3D() {}
};

#include "Vector3.h"
/*! Constant vector field that always returns a constant vector */
class VFConstant : public Function3D<Vector3<float> >{
protected:
  Vector3<float> mV;
public:
  VFConstant(const Vector3<float> v) : mV(v) {}
  Vector3<float> getValue(float x, float y, float z) const {
    return mV;
  }
  //! Return a bound on the maximum value of the function
  Vector3<float> getMaxValue() const {
    return getValue(0.f,0.f,0.f);
  }
};
#endif
