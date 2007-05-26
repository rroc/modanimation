#include "EulerIntegrator.h"

EulerIntegrator::EulerIntegrator()
	{
	}

EulerIntegrator::~EulerIntegrator()
	{
	}

Vector3<float> EulerIntegrator::integrate(const Vector3<float>& v0, const Vector3<float>& f, const float dt) const
	{
	//printf("Euler integration not yet implemented!\n");
	// Add your code here...
	Vector3<float> resultingVelocity = v0 + f*dt;

	return resultingVelocity;
	}
