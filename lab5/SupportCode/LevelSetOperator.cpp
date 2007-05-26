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

/*! Computes the squares of the partial derivatives in x, y, z using the
 * Godunov method
 *
 * \f[
 * \left( \dfrac{\partial\phi}{\partial x} \right)^2 \approx \begin{cases}
 * \text{max}\left[\text{max}(\phi_x^-,0)^2,       \text{min}(\phi_x^+,0)^2\right] & F > 0 \\
 * \text{max}\left[\text{min}(\phi_x^-,0)^2,       \text{max}(\phi_x^+,0)^2\right] & F < 0 \\
 * \end{cases}
 * \f]
 *
 * \param[in] i grid x coordinate
 * \param[in] j grid y coordinate
 * \param[in] k grid z coordinate
 * \param[in] a speed function
 * \param[out] ddx2 (dphi/dx)^2
 * \param[out] ddy2 (dphi/dy)^2
 * \param[out] ddz2 (dphi/dy)^2
 */
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

Vector3<float> LevelSetOperator::gradient( const Vector3<float>& v, const float& i, const float& j, const float& k, bool useWENO )
	{
	// calculate upwind differentials
	float ddx, ddy, ddz;
	if(!useWENO)
		{
		// flow in the negative direction: use upwind difXp
		ddx = ( v.x() < 0.0f ) ? mLS->diffXp(i, j, k) : mLS->diffXm(i, j, k);
		ddy = ( v.y() < 0.0f ) ? mLS->diffYp(i, j, k) : mLS->diffYm(i, j, k);
		ddz = ( v.z() < 0.0f ) ? mLS->diffZp(i, j, k) : mLS->diffZm(i, j, k);
		}
	else
		{
		// flow in the negative direction: use upwind difXp
		ddx = ( v.x() < 0.0f ) ? mLS->diffXpWENO(i, j, k) : mLS->diffXmWENO(i, j, k);
		ddy = ( v.y() < 0.0f ) ? mLS->diffYpWENO(i, j, k) : mLS->diffYmWENO(i, j, k);
		ddz = ( v.z() < 0.0f ) ? mLS->diffZpWENO(i, j, k) : mLS->diffZmWENO(i, j, k);
		}

	// compute time differential as dot product
	return Vector3<float>(ddx, ddy, ddz);
	}

float LevelSetOperator::forwardEuler( const float& i, const float& j, const float& k, const float& velocity, const float& dt )
	{
	// update the new value using first-order forward Euler
	float phiCurrent	= getGrid().getValue(i, j, k); //mLS->getValue(x, y, z);
	return ( phiCurrent + velocity * dt );
	}
