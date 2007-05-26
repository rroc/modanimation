#ifndef __SEMI_LAGRANGIAN_INTEGRATOR_H__
#define __SEMI_LAGRANGIAN_INTEGRATOR_H__

#include "Vector3.h"
#include "Volume.h"

class SemiLagrangianIntegrator{
	public:
    SemiLagrangianIntegrator();
    ~SemiLagrangianIntegrator();

    Vector3<float> integrate(const Vector3<int>& pos, const Volume<Vector3<float> >& vectorField, const float dt, const unsigned int numSteps = 3) const;

	private:
};

#endif
