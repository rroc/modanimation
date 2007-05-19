#include "SemiLagrangianIntegrator.h"
#include "TrilinearInterpolator.h"

SemiLagrangianIntegrator::SemiLagrangianIntegrator()
	{
	}

SemiLagrangianIntegrator::~SemiLagrangianIntegrator()
	{
	}

// Semi lagrangian integrator for vector fields
Vector3<float> SemiLagrangianIntegrator::integrate(const Vector3<int>& pos, const Volume<Vector3<float> >& vectorField, const float dt, const unsigned int numSteps) const
	{
	//printf("Semi-lagrangian integration not yet implemented!\n");

	float localDT = dt/numSteps;
	Vector3<float> result;

	// Get initial position
	Vector3<float> p((float)pos[0], (float)pos[1], (float)pos[2]);

	// Get velocity at initial position
	Vector3<float> v = vectorField.getValue(pos[0], pos[1], pos[2]);

	// Perform backwards particle trace trough the (static) velocity field
	for (unsigned int i = 0; i < numSteps; i++)
		{
		// Move a small step backwards through the field
		p -= v*localDT;

		// Find new velocity at the new position (this is done through interpolation)
		v = vectorField.getValue( p.x(), p.y(), p.z() );
		}

	return v;
	}
