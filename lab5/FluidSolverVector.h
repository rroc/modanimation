#ifndef __FLUID_SOLVER_VECTOR_H__
#define __FLUID_SOLVER_VECTOR_H__

#include <vector>
#include <Volume.h>
#include <VolumeLevelSet.h>
#include "FluidSolverSparseMatrix.h"

class FluidSolverVector{
	public:
    FluidSolverVector();
    ~FluidSolverVector();

		void buildVector(Volume<float>* field, VolumeLevelSet* volLS);
		const float get(int i, int j, int k) const { return mField->getValue(i,j,k);}
		void set(int i, int j, int k, const float val) {mField->setValue(i,j,k,val);}

		const VolumeLevelSet* getLS() const {return mVolLS;}

	private:
		Volume<float>* mField;
		VolumeLevelSet* mVolLS;
};

float norm(const FluidSolverVector& v);
float dot(const FluidSolverVector& a, const FluidSolverVector& b);
void a_equals_b_minus_C_times_d(FluidSolverVector& a, const FluidSolverVector& b, const FluidSolverSparseMatrix& C, const FluidSolverVector& d);
void a_equals_b_plus_c_times_d( FluidSolverVector& a, const FluidSolverVector& b, const float c, const FluidSolverVector& d);
void a_equals_B_times_c(FluidSolverVector& a, const FluidSolverSparseMatrix& B, const FluidSolverVector& c);
void a_equals_a_plus_b_times_c(FluidSolverVector& a, const float b, const FluidSolverVector& c);
void a_equals_b(FluidSolverVector& a, const FluidSolverVector& b);

#endif
