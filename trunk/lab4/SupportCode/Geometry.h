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
#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include "Matrix4x4.h"

/*!  \brief Geometry base class */
class Geometry{
public:
  Geometry() {}
  virtual ~Geometry() {};
  virtual void draw() = 0;

  //! Translate geometry
  void translate(float x, float y, float z) {
    setTransform(mTransform * Matrix4x4<float>::translation(x,y,z));
  }

  //! Scale geometry
  void scale(float s) {
    setTransform(mTransform * Matrix4x4<float>::scale(s));
  }

  //! Scale geometry
  void scale(float x, float y, float z) {
    setTransform(mTransform * Matrix4x4<float>::scale(x,y,z));
  }

  //! Rotate geometry
  void rotate(float rx, float ry, float rz) {
    setTransform(mTransform * Matrix4x4<float>::rotationXYZ(rx,ry,rz));
  }

  //! Set transformation
  virtual void setTransform(const Matrix4x4<float> & transform) {
    mTransform = transform;
  }

  //! Get transform
  Matrix4x4<float>& getTransform(){
    return mTransform;
  }
  const Matrix4x4<float>& getTransform() const {
    return mTransform;
  }

protected:
  Matrix4x4<float> mTransform;
};

#endif
