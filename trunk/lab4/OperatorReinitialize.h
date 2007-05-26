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
#ifndef __operatorreinitialize_h__
#define __operatorreinitialize_h__

#include "LevelSetOperator.h"

/*! \brief A level set operator that re-initializes a level set towards a signed distance function.
 *
 * This class implements level set re-initialization by solving the PDE
 *
 *  \f{eqnarray*}
 *  \dfrac{\partial \phi}{\partial t} = S(\phi) (1 - |\nabla \phi|)\\
 *  S = \dfrac{1}{\sqrt{\phi^2+|\nabla \phi|^2\Delta x^2}}
 *  \f}
 *
 */
class OperatorReinitialize : public LevelSetOperator
{
public :

  OperatorReinitialize(LevelSet * LS) : LevelSetOperator(LS) { }

  virtual void propagate(float time)
  {
    // Create buffer used to store intermediate results
    std::vector<float> buffer;

    // Determine timestep for stability
    float dx = mLS->getDx();
    float dt = 0.5 * dx;


    // Propagate level set with stable timestep dt
    // until requested time is reached
    for (float elapsed = 0; elapsed < time;) {

      getGrid().dilate();

      if (dt > time-elapsed)
        dt = time-elapsed;
      elapsed += dt;

      // Iterate over grid and compute the grid values for the next timestep
      LevelSetGrid::Iterator iter = getGrid().beginNarrowBand();
      LevelSetGrid::Iterator iend = getGrid().endNarrowBand();
      while (iter != iend) {
        unsigned int i = iter.getI();
        unsigned int j = iter.getJ();
        unsigned int k = iter.getK();

        // Compute the sign function (from central differencing?)
        float ddxc = mLS->diffXpm(i,j,k);
        float ddyc = mLS->diffYpm(i,j,k);
        float ddzc = mLS->diffZpm(i,j,k);
        float normgrad2 = ddxc*ddxc + ddyc*ddyc + ddzc*ddzc;
        float val = getGrid().getValue(i,j,k);
        float sign = val / std::sqrt(val*val + normgrad2*dx*dx);

        float ddx2, ddy2, ddz2;
        godunov(i,j,k, sign, ddx2, ddy2, ddz2);

        // Compute the new value and store it in the buffer
        float ddt = sign * (1 - std::sqrt(ddx2 + ddy2 + ddz2));
        val = getGrid().getValue(i,j,k) + ddt*dt;
        buffer.push_back(val);

        iter++;
      }

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

      // Read maximum norm of gradient
      float maxGrad = -std::numeric_limits<float>::max();
      iter = getGrid().beginNarrowBand();
      while (iter != iend) {
        unsigned int i = iter.getI();
        unsigned int j = iter.getJ();
        unsigned int k = iter.getK();


        float ddxc = mLS->diffXpm(i,j,k);
        float ddyc = mLS->diffYpm(i,j,k);
        float ddzc = mLS->diffZpm(i,j,k);
        float normgrad2 = ddxc*ddxc + ddyc*ddyc + ddzc*ddzc;
        if (maxGrad < normgrad2) maxGrad = normgrad2;

        iter++;
      }
      maxGrad = std::sqrt(maxGrad);
      std::cerr << "Maximum gradient: " << maxGrad << std::endl;

      getGrid().rebuild();
    }

  }

  virtual void propagateTo( float delta )
	  {
	  // Create buffer used to store intermediate results
	  std::vector<float> buffer;

	  // Determine timestep for stability
	  float dx = mLS->getDx();
	  float dt = 0.5 * dx;

	  // Read maximum norm of gradient
	  float maxGrad = std::numeric_limits<float>::max();

	while( maxGrad>delta )
	{

	  // Propagate level set with stable timestep dt
	  // until requested time is reached
	  for (float elapsed = 0, time = 0.02; elapsed < time;) 
		  {

		  getGrid().dilate();

		  if (dt > time-elapsed)
			  dt = time-elapsed;
		  elapsed += dt;

		  // Iterate over grid and compute the grid values for the next timestep
		  LevelSetGrid::Iterator iter = getGrid().beginNarrowBand();
		  LevelSetGrid::Iterator iend = getGrid().endNarrowBand();
		  while (iter != iend) {
			  unsigned int i = iter.getI();
			  unsigned int j = iter.getJ();
			  unsigned int k = iter.getK();

			  // Compute the sign function (from central differencing?)
			  float ddxc = mLS->diffXpm(i,j,k);
			  float ddyc = mLS->diffYpm(i,j,k);
			  float ddzc = mLS->diffZpm(i,j,k);
			  float normgrad2 = ddxc*ddxc + ddyc*ddyc + ddzc*ddzc;
			  float val = getGrid().getValue(i,j,k);
			  float sign = val / std::sqrt(val*val + normgrad2*dx*dx);

			  float ddx2, ddy2, ddz2;
			  godunov(i,j,k, sign, ddx2, ddy2, ddz2);

			  // Compute the new value and store it in the buffer
			  float ddt = sign * (1 - std::sqrt(ddx2 + ddy2 + ddz2));
			  val = getGrid().getValue(i,j,k) + ddt*dt;
			  buffer.push_back(val);

			  iter++;
			  }

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

		  // Read maximum norm of gradient
		  maxGrad = -std::numeric_limits<float>::max();
		  iter = getGrid().beginNarrowBand();
		  while (iter != iend) {
			  unsigned int i = iter.getI();
			  unsigned int j = iter.getJ();
			  unsigned int k = iter.getK();


			  float ddxc = mLS->diffXpm(i,j,k);
			  float ddyc = mLS->diffYpm(i,j,k);
			  float ddzc = mLS->diffZpm(i,j,k);
			  float normgrad2 = ddxc*ddxc + ddyc*ddyc + ddzc*ddzc;
			  if (maxGrad < normgrad2) maxGrad = normgrad2;

			  iter++;
			  }
		  maxGrad = std::sqrt(maxGrad);
		  std::cerr << "Maximum gradient: " << maxGrad << std::endl;

		  getGrid().rebuild();
		  }
		}


	  }

};

#endif
