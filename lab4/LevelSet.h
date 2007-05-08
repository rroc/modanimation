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
#ifndef __levelset_h__
#define __levelset_h__

#include "Implicit.h"
#include "LevelSetGrid.h"
#include <iostream>

class LevelSet : public Implicit
{
  friend class LevelSetOperator;

protected :

  LevelSetGrid mGrid;
  //! Grid spacing
  float mDx;

public :
  LevelSet(float dx);
  LevelSet(float dx, const Implicit & impl);
  LevelSet(float dx, const Implicit & impl, const Bbox & box);

  virtual ~LevelSet() { }

  virtual void draw();

  //! evaluate the implicit at world coordinates x y z
  virtual float getValue(float x, float y, float z) const;

  //! calculate the gradient of the implicit at world coordinates x y z
  virtual Vector3<float> getGradient(float x, float y, float z, float delta = 1e-3) const;

  //! Sets the bounding box in current frame coordinates
  virtual void setBoundingBox(const Bbox & b);

  virtual void setNarrowBandWidth(float width);

  inline const float getDx() const { return mDx; }

  //! First order negative differential in x
  float diffXm(int i, int j, int k) const;
  //! First order positive differential in x
  float diffXp(int i, int j, int k) const;
  //! Second order central differential in x
  float diffXpm(int i, int j, int k) const;
  //! Second order second central differential in x
  float diff2Xpm(int i, int j, int k) const;

  //! First order negative differential in y
  float diffYm(int i, int j, int k) const;
  //! First order positive differential in y
  float diffYp(int i, int j, int k) const;
  //! Second order central differential in y
  float diffYpm(int i, int j, int k) const;
  //! Second order second central differential in y
  float diff2Ypm(int i, int j, int k) const;

  //! First order negative differential in z
  float diffZm(int i, int j, int k) const;
  //! First order positive differential in z
  float diffZp(int i, int j, int k) const;
  //! Second order central differential in z
  float diffZpm(int i, int j, int k) const;
  //! Second order second central differential in z
  float diff2Zpm(int i, int j, int k) const;

  //! First order mixed differential in x,y
  float diff2XYpm(int i, int j, int k) const;
  //! First order mixed differential in y,z
  float diff2YZpm(int i, int j, int k) const;
  //! First order mixed differential in z,x
  float diff2ZXpm(int i, int j, int k) const;

  friend std::ostream& operator << (std::ostream &os, const LevelSet &LS)
  {
    os << "Level set bounding box: " << LS.mBox.pMin << " -> " << LS.mBox.pMax << std::endl;
    os << "Level set dx: " << LS.mDx << std::endl;
    os << LS.mGrid << std::endl;
    return os;
  }

};


#endif
