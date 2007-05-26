#ifndef __EULER_INTEGRATOR_H__
#define __EULER_INTEGRATOR_H__

#include "Vector3.h"
#include "Volume.h"

class EulerIntegrator{
  public:
    EulerIntegrator();
    ~EulerIntegrator();


  Vector3<float> integrate(const Vector3<float>& v0, const Vector3<float>& f, const float dt) const;

  private:

};

#endif
