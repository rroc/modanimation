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
#ifndef __operatormeancurvatureflow_h__
#define __operatormeancurvatureflow_h__

#include "LevelSetOperator.h"

/*! \brief A level set operator that does mean curvature flow.
*
* This class implements level set propagation in the normal direction
* as defined by the mean curvature flow \f$\kappa\f$ in the following PDE
*
*  \f[
*  \dfrac{\partial \phi}{\partial t} + \alpha \kappa|\nabla \phi| = 0
*  \f]
*/
//! \lab4 Implement mean curvature flow
class OperatorMeanCurvatureFlow : public LevelSetOperator
	{
	protected:
		//! Scaling parameter, affects time step constraint
		float mAlpha;
	public :

		OperatorMeanCurvatureFlow(LevelSet * LS, float alpha=.9f)
			: LevelSetOperator(LS), mAlpha(alpha) { }

		virtual void propagate(float time)
			{
			std::cout << "applying mean curvature flow operator " << std::endl;

			// Create buffer used to store intermediate results
			std::vector<float> buffer;

			// calculate stable timestep dt
			float bound = 0.9f;
			float dX = mLS->getDx();
			float dt = bound * (dX * dX) / (6 * mAlpha );

			// Propagate level set with stable timestep dt
			// until requested time is reached
			for (float elapsed = 0; elapsed < time;) {

				if (dt > time-elapsed)
					dt = time-elapsed;
				elapsed += dt;

				LevelSetGrid::Iterator iter = getGrid().beginNarrowBand();
				LevelSetGrid::Iterator iend = getGrid().endNarrowBand();

				int counter = 0;
				while (iter != iend) 
					{

					//		  if ( (counter++ % 100) == 0)
					//		  {
					//			  std::cout << ".";
					//		  }

					unsigned int i = iter.getI();
					unsigned int j = iter.getJ();
					unsigned int k = iter.getK();

					/* *** calculate curvature *** */

					// second order derivatives
					float ddx2 = mLS->diff2Xpm(i, j, k);
					float ddy2 = mLS->diff2Ypm(i, j, k);
					float ddz2 = mLS->diff2Zpm(i, j, k);

					// first order derivatives
					float ddx = mLS->diffXpm(i, j, k);
					float ddy = mLS->diffYpm(i, j, k);
					float ddz = mLS->diffZpm(i, j, k);

					// mixed derivatives
					float dydz = mLS->diff2YZpm(i, j, k);
					float dxdz = mLS->diff2ZXpm(i, j, k);
					float dxdy = mLS->diff2XYpm(i, j, k);

					// squares of the first derivatives
					float ddxSqr = ddx * ddx;
					float ddySqr = ddy * ddy;
					float ddzSqr = ddz * ddz;

					float denominator = 2.0f * pow(ddxSqr + ddySqr + ddzSqr, 3.0f/2.0f);
					float curvatureK =	(ddxSqr * (ddy2 + ddz2) - 2.0f*ddy*ddz*dydz) / denominator + 
						(ddySqr * (ddx2 + ddz2) - 2.0f*ddx*ddz*dxdz) / denominator +
						(ddzSqr * (ddx2 + ddy2) - 2.0f*ddx*ddy*dxdy) / denominator;

					/* *** calculate gradient using central differentials *** */
					ddx = mLS->diffXpm(i, j, k);
					ddy = mLS->diffYpm(i, j, k);
					ddz = mLS->diffZpm(i, j, k);

					/* *** compute time differential as product *** */
					float gradientNorm = sqrt(ddx*ddx + ddy*ddy + ddz*ddz);
					float changeRate = mAlpha * curvatureK * gradientNorm;

					/* *** update phi using first order forward Euler time integration *** */
					float currentPhi	= getGrid().getValue(i, j, k);
					float nextPhi		=  currentPhi + changeRate * dt;

					// assign new value and store it in the buffer
					buffer.push_back( nextPhi );

					iter++;
					}

				// Copy new values from buffer to grid
				iter = getGrid().beginNarrowBand();
				std::vector<float>::iterator iterBuffer = buffer.begin();
				while (iter != iend) 
					{
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
