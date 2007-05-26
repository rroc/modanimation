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
#ifndef __cyclide_h__
#define __cyclide_h__

#include "Implicit.h"

/*!  \brief Cyclide object (kind of torus) */
class Cyclide : public Implicit{
public:
  Cyclide();
  virtual ~Cyclide();
  virtual float getValue(float x, float y, float z) const;
protected:
  float A, B, C, D;
};

#endif
