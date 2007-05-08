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
#ifndef __levelset_operator_h__
#define __levelset_operator_h__

#include "LevelSet.h"

class LevelSetOperator
{
protected :
  LevelSet * mLS;

  //! Exposes access to the level set grid internally
  LevelSetGrid & getGrid() { return mLS->mGrid; }
  const LevelSetGrid & getGrid() const { return mLS->mGrid; }

  void godunov(unsigned int i, unsigned int j, unsigned int k, float a,
               float & ddx2, float & ddy2, float & ddz2);


public :

  LevelSetOperator(LevelSet * LS) : mLS(LS) { }
  virtual ~LevelSetOperator() {}
  virtual void propagate(float time) = 0;
};


#endif

