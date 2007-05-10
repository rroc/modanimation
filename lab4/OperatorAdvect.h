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
public :

  OperatorAdvect(LevelSet * LS, Function3D<Vector3<float> > * vf) : LevelSetOperator(LS), mVectorField(vf) { }

  virtual void propagate(float time)
  {
    // Create buffer used to store intermediate results
    std::vector<float> buffer;


    // Determine timestep for stability
	Vector3 v = mVectorField->getMaxValue();
	float delta = mLS->getDx();
	float x = delta / abs( v.x() ); 
	float y = delta / abs( v.y() );
	float z = delta / abs( v.z() );
    float dt =  min( x, min( y, z ) );

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
		
        // calculate upwind differentials
		float ddx, ddy, ddz;

        // compute time differential as dot product

        // assign new value and store it in the buffer
        buffer.push_back(0);

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
