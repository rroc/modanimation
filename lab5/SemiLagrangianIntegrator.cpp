#include "SemiLagrangianIntegrator.h"
#include "TrilinearInterpolator.h"

SemiLagrangianIntegrator::SemiLagrangianIntegrator()
{
}

SemiLagrangianIntegrator::~SemiLagrangianIntegrator()
{
}

// Semi lagrangian integrator for vector fields
Vector3<float> SemiLagrangianIntegrator::integrate(const Vector3<int>& x, const Volume<Vector3<float> >& y, const float dt, const unsigned int numSteps) const
{
  //printf("Simi-lagrangian integration not yet implemented!\n");

	float localDT = dt/numSteps;

	Vector3<float> result;

	// Get initial poistion
	Vector3<float> p((float)x[0], (float)x[1], (float)x[2]);

	// Get velocity at initial position
	Vector3<float> v = y.getValue(x[0], x[1], x[2]);

	// Perform backwards particle trace trough the (static) velocity field
	for (unsigned int i = 0; i < numSteps; i++){

		// Move a small step backwards trough the field

		// Find new velocity at the new position (this is done trough interpolation)
	}

	return v;
}
