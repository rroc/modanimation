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
#ifndef __COLOR_MAP_H__
#define __COLOR_MAP_H__

#include "Vector3.h"
#include "Util.h"
#include <vector>
class ColorMap{
public:
  ColorMap();
  virtual ~ColorMap() {}

  virtual Vector3<float> map(float val, float low, float high) const;
  virtual Vector3<float> map(const Vector3<float> & val, float low, float high) const;

protected:
  std::vector<Vector3<float> > colors;
};

#endif
