#ifndef __FLUID_SIM_SETUP_H__
#define __FLUID_SIM_SETUP_H__

#include "Sphere.h"
#include "LevelSet.h"
#include "VolumeLevelSet.h"
#include "CSG.h"
#include "Bbox.h"
#include "Cube.h"

class FluidSimSetup{
	public:
    FluidSimSetup(float dx = 0.02, float bandWidth = 14);
    ~FluidSimSetup();

    LevelSet* getSimpleSolid();
    VolumeLevelSet* getSimpleFluid();

    LevelSet* getFluidBoxSolid();
    VolumeLevelSet* getFluidBoxFluid();

    LevelSet* getComplexSolid();
    VolumeLevelSet* getComplexFluid();

//    Implicit* getComplexSolid();
//    Implicit* getComplexFluid();


	private:
		float mDx;
		float mBandWidth;
};

#endif
