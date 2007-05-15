#ifndef __INTERPOLATOR_H__
#define __INTERPOLATOR_H__

#include "Vector3.h"
#include "Volume.h"

/*! \brief Interpolator base class */
class Interpolator{
	public:
  	Interpolator() {}
  	virtual ~Interpolator() {}

  	virtual Vector3<float> interpolate(float x, float y, float z, const Volume<Vector3<float> >& grid) = 0;
  	virtual float interpolate(float x, float y, float z, const Volume<float >& grid) = 0;


	protected:

};

#endif
