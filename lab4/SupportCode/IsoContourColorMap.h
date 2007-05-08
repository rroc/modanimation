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
#ifndef __ISO_CONTOUR_COLOR_MAP_H__
#define __ISO_CONTOUR_COLOR_MAP_H__

#include "ColorMap.h"

class IsoContourColorMap : public ColorMap
{
protected :
  float mFrequency;
public:
  IsoContourColorMap(float frequency) : mFrequency(frequency) { }
  virtual ~IsoContourColorMap() { }

  virtual Vector3<float> map(float val, float low, float high) const
  {
    // Take absolute value
    if (val < 0) val = -val;

    // Compute fraction w.r.t. requested frequency
    float fraction = val*mFrequency - (unsigned int)(val*mFrequency);

    // Do color mapping
    return ColorMap::map(fraction, low, high);
  }
};

#endif
