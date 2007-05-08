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
#include "LevelSetOperator.h"

void LevelSetOperator::godunov(unsigned int i, unsigned int j, unsigned int k, float a,
                               float & ddx2, float & ddy2, float & ddz2)
{
  float ddxm = mLS->diffXm(i,j,k);
  float ddxp = mLS->diffXp(i,j,k);
  float ddym = mLS->diffYm(i,j,k);
  float ddyp = mLS->diffYp(i,j,k);
  float ddzm = mLS->diffZm(i,j,k);
  float ddzp = mLS->diffZp(i,j,k);

  if (a > 0) {
    ddx2 = std::max( std::pow(std::max(ddxm,0.0f),2), std::pow(std::min(ddxp,0.0f),2) );
    ddy2 = std::max( std::pow(std::max(ddym,0.0f),2), std::pow(std::min(ddyp,0.0f),2) );
    ddz2 = std::max( std::pow(std::max(ddzm,0.0f),2), std::pow(std::min(ddzp,0.0f),2) );
  }
  else {
    ddx2 = std::max( std::pow(std::min(ddxm,0.0f),2), std::pow(std::max(ddxp,0.0f),2) );
    ddy2 = std::max( std::pow(std::min(ddym,0.0f),2), std::pow(std::max(ddyp,0.0f),2) );
    ddz2 = std::max( std::pow(std::min(ddzm,0.0f),2), std::pow(std::max(ddzp,0.0f),2) );
  }
}
