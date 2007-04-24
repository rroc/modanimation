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
#include "Cyclide.h"

Cyclide::Cyclide(){
  this->A = 1.5;
  this->B = 1.17;
  this->C = sqrtf(A*A-B*B);
  this->D = 1.23;
  this->mBox = Bbox(Vector3<float>(-1), Vector3<float>(1));
}

Cyclide::~Cyclide(){}

float Cyclide::getValue(float x, float y, float z) const{
  Vector4<float> vprim, v = Vector4<float>(x, y, z, 1.f);
  vprim = mWorld2Obj*v;
  const float f = (
		   (vprim[0]*vprim[0] +
		    vprim[1]*vprim[1] +
		    vprim[2]*vprim[2] +
		    B*B -
		    D*D)
		   * (vprim[0]*vprim[0] +
		      vprim[1]*vprim[1] +
		      vprim[2]*vprim[2] +
		      B*B -
		      D*D)
		   - 4.f*(
			  (A*vprim[0] - C*D)*(A*vprim[0] - C*D)
			  + B*B * vprim[1]*vprim[1]
			  )
		   );
  return f;
}
