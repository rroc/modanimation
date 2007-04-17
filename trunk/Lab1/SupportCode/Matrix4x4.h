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
#ifndef _matrix4x4_h
#define _matrix4x4_h

template <typename Real> class Vector3;
template <typename Real> class Vector4;

template <typename Real>
class Matrix4x4
{
public:
  Matrix4x4();
  Matrix4x4(const Real m[4][4]);
	
  void toGLMatrix(float glMatrix[16]) const;
  Matrix4x4 inverse() const;
  // Input: i (row), j (column)
  Real &operator()(unsigned int i, unsigned int j);
  Real *operator[](unsigned int i) { return m[i]; }
  Vector4<Real> operator*(const Vector4<Real>& vec4) const;
  Matrix4x4 operator*(const Matrix4x4& m2) const;

  // Static methods	
  static Matrix4x4 scale(Real scale);
  static Matrix4x4 scale(Real sx, Real sy, Real sz);
  // Angles in radians
  static Matrix4x4 rotationXYZ(Real rx, Real ry, Real rz);
  static Matrix4x4 translation(Real tx, Real ty, Real tz);
  static Matrix4x4 identity();

protected:
  Real m[4][4];

  static Matrix4x4 identityMatrix;
};

#include "Matrix4x4_Impl.h"

#endif
