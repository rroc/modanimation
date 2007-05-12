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
#ifndef __operatoradvect_h__
#define __operatoradvect_h__

#include "LevelSetOperator.h"
#include "Function3D.h"


/*! \brief A level set operator that does external advection
*
* This class implements level set advectionr in an external vector field by the PDE
*
*  \f$
*  \dfrac{\partial \phi}{\partial t} + \mathbf{V}(\mathbf{x})\cdot \nabla \phi = 0
*  \f$
*/
//! \lab4 Implement advection in external vector field
class OperatorAdvect : public LevelSetOperator
	{
	protected :
		Function3D<Vector3<float> > * mVectorField;

	private :
		bool mUseWENO;
		bool mUseRungeKutta;

	public :

		OperatorAdvect(LevelSet * LS, Function3D<Vector3<float> > * vf) 
			: LevelSetOperator(LS)
			, mVectorField(vf) 
			, mUseWENO(false)
			, mUseRungeKutta(false)
			{ }

		OperatorAdvect(LevelSet * LS, Function3D<Vector3<float> > * vf, bool aUseWENO) 
			: LevelSetOperator(LS)
			, mVectorField(vf) 
			, mUseWENO(aUseWENO)
			, mUseRungeKutta(false)
			{ }

		OperatorAdvect(LevelSet * LS, Function3D<Vector3<float> > * vf, bool aUseWENO, bool aRungeKutta) 
			: LevelSetOperator(LS)
			, mVectorField(vf) 
			, mUseWENO(aUseWENO)
			, mUseRungeKutta(aRungeKutta)
			{ }

		virtual void propagate(float time)
			{
			// Create buffer used to store intermediate results
			std::vector<float> buffer;


			// Determine timestep for stability
			Vector3<float> v = mVectorField->getMaxValue();
			float delta = mLS->getDx();
			float x = delta / abs( v.x() ); 
			float y = delta / abs( v.y() );
			float z = delta / abs( v.z() );
			float alpha = 0.9f;
			float dt =  std::min( x, std::min( y, z ) );

			// Propagate level set with stable timestep dt
			// until requested time is reached
			for (float elapsed = 0; elapsed < time;) {

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

					// Get vector for grid point (i,j,k) from vector field
					// Remember to translate (i,j,k) into world coordinates (x,y,z)
					float x,y,z;
					this->mLS->grid2World(i,j,k, x,y,z);
					v = mVectorField->getValue( x,y,z );


					//getGrid().getValue(i, j, k);
					float velocity = - gradient(v, i,j,k, mUseWENO) * v;
					float phiNext = forwardEuler( i,j,k, velocity, dt );
					

					// assign new value and store it in the buffer
					buffer.push_back( phiNext );
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
				std::cerr << elapsed << std::endl;
				}
			}

	};

#endif
