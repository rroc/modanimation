#ifndef __NAVIER_STOKES_SOLVER_H__
#define __NAVIER_STOKES_SOLVER_H__

#include "Vector3.h"
#include "Volume.h"
#include "EulerIntegrator.h"
#include "VolumeLevelSet.h"
#include "SemiLagrangianIntegrator.h"
#include <vector>
#include "FluidSolverSparseMatrix.h"
#include "FluidSolverVector.h"
#include "ConjugateGradient.h"

class NavierStokesSolver{
  public:
    NavierStokesSolver();
    ~NavierStokesSolver();


	float getTimestep(float alpha = 0.7);

  void solve(std::vector<Geometry*>& geometryList, float dt);

  private:
		bool isSolid(int i, int j, int k, std::vector<LevelSet*>& solids);
    void calculateNewTimestep(std::vector<Geometry*>& geometryList);
    void processVoxel(int i, int j, int k, float& element, float& centerElement, VolumeLevelSet* ls);
    void claculateDivergence(VolumeLevelSet* ls);
    float calculateVolume(VolumeLevelSet* ls);
		void swapFields();

		// Solver steps
    void selfAdvect(VolumeLevelSet* ls, float dt);
		void addForces(VolumeLevelSet* ls, float dt);
		void enforceDirichletBoundaryCondition(std::vector<Geometry*>& geometryList, Volume<Vector3<float> >* field);
		void calculateRHS(VolumeLevelSet* ls, float dt);
		void buildMatrix(VolumeLevelSet* ls);
    void solvePoissonEquation(VolumeLevelSet* ls, float dt);

		void velocityFieldCorrection(VolumeLevelSet* ls, float dt);

    // Solver components
    EulerIntegrator mEulerIntegrator;
    SemiLagrangianIntegrator mSemiLagrangianIntegrator;


    // Misc member data
		float mLargestDT;
		float mTargetVolume;
		float mCurrentVolume;
    Volume<Vector3<float> >* mCurrentField;
    Volume<Vector3<float> >* mOldField;
    Volume<float>* mPressureField;
    Volume<float>* mRHSField;
    Volume<bool>* mSolidMask;
    FluidSolverSparseMatrix mMatrix;


    int mDimX;
    int mDimY;
    int mDimZ;
    float mCellSize;

    // Constants
    static const float mG;
};

#endif
