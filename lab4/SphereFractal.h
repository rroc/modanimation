#ifndef __SPHERE_FRACTAL_H__
#define __SPHERE_FRACTAL_H__

#include "Implicit.h"
#include <vector>
#include "Matrix4x4.h"
#include "Sphere.h"
#include "CSG.h"
#include "Bbox.h"
#include "Geometry.h"

/*! \brief Fractal of spheres class*/
class SphereFractal : public Implicit {
  public:

    //! Constructor. Level denotes the level of the fractal. Level can be between 2 and 5
	  SphereFractal(int level = 2);

    ~SphereFractal();

    virtual float getValue(float x, float y, float z) const;

    //! Builds the fractal. Returns a pointer to an implicit geometry object.
    Implicit* buildFractal();

  private:
    std::vector<Implicit*> mSpheres;
    Implicit* mFractal;

	float p;

};

#endif
