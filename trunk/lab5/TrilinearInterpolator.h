#ifndef __TRILINEAR_INTERPOLATOR_H__
#define __TRILINEAR_INTERPOLATOR_H__

#include "Interpolator.h"

class TrilinearInterpolator : public Interpolator
{
	public:
    TrilinearInterpolator();
    ~TrilinearInterpolator();

  	virtual Vector3<float> interpolate(float x, float y, float z, const Volume<Vector3<float> >& grid);
  	virtual float interpolate(float x, float y, float z, const Volume<float >& grid);

	protected:
};

#endif
