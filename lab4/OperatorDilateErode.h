/*************************************************************************************************
 *
 * Modeling and animation (TNM079) 2007
 * Code base for lab assignments. Copyright:
 *   Gunnar Johansson (gunnar.johansson@itn.liu.se)
 *   Ken Museth (ken.museth@itn.liu.se)
 *   Michael Bang Nielsen (bang@daimi.au.dk)
 *   Ola Nilsson (ola.nilsson@itn.liu.se)
 *   Andreas Sderstrm (andreas.soderstrom@itn.liu.se)
 *
 *************************************************************************************************/
#ifndef __operatordilateerode_h__
#define __operatordilateerode_h__

#include "LevelSetOperator.h"

/*! \brief A level set operator that does erosion or dilation.
 *
 * This class implements level set propagation in the normal direction
 * as defined by
 *  \f$
 *  \dfrac{\partial \phi}{\partial t}+ F(\mathbf{x})|\nabla \phi| = 0
 *  \f$
 * where the sign of F dictates erosion (c<0), or dilation (c>0).
 */
//! \lab4 Implement erosion and dilation
class OperatorDilateErode : public LevelSetOperator
{
protected :
  float mA;

public :

  OperatorDilateErode(LevelSet * LS, float a) : LevelSetOperator(LS), mA(a) { }

  virtual void propagate(float time)
  {
    // Create buffer used to store intermediate results
    std::vector<float> buffer;

    // Determine timestep for stability
    float dt = ( mLS->getDx() / abs( mA ) );

	printf("time = %f\n", time);
	printf("dt = %f\n", dt);

    // Propagate level set with stable timestep dt
    // until requested time is reached
    for (float elapsed = 0; elapsed < time;) {

		printf(".");
      if (dt > time - elapsed)
        dt = time - elapsed;
      elapsed += dt;

      // Iterate over grid and compute the grid values for the next timestep
      LevelSetGrid::Iterator iter = getGrid().beginNarrowBand();
      LevelSetGrid::Iterator iend = getGrid().endNarrowBand();
      while (iter != iend) {
        unsigned int i = iter.getI();
        unsigned int j = iter.getJ();
        unsigned int k = iter.getK();

        float ddx2, ddy2, ddz2;

        // Compute the new value and store it in the buffer
		godunov(i, j, k, mA, ddx2, ddy2, ddz2 );

		float gradientNorm = sqrt( ddx2 + ddy2 + ddz2 );
		float dPhiDt = -mA * gradientNorm;

		float x,y,z;
		this->mLS->grid2World(i,j,k, x,y,z);
		float phiCurrent	= mLS->getValue(x, y, z);
		float phiNext		= phiCurrent + dPhiDt * dt;

        buffer.push_back( phiNext );

        iter++;
      }
	  printf("\n");

      // Copy new values from buffer to grid
      iter = getGrid().beginNarrowBand();
      std::vector<float>::iterator iterBuffer = buffer.begin();
      while (iter != iend) {
        unsigned int i = iter.getI();
        unsigned int j = iter.getJ();
        unsigned int k = iter.getK();

        getGrid().setValue(i,j,k, (*iterBuffer));
        iter++;
        iterBuffer++;
      }
      buffer.clear();
    }
  }

};

#endif
