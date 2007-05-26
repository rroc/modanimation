#include "FluidSolverVector.h"

FluidSolverVector::FluidSolverVector()
{
}

FluidSolverVector::~FluidSolverVector()
{
}

void FluidSolverVector::buildVector(Volume<float>* field, VolumeLevelSet* volLS)
{
  mField = field;
  mVolLS = volLS;
}


float norm(const FluidSolverVector& v)
{
	float normValue = 0;

	const VolumeLevelSet::InsideMask& insideMask = (v.getLS())->getInsideMask();
	for (unsigned int p = 0; p < insideMask.size(); p++){
    const Vector3<unsigned int>& pos = insideMask[p];
    unsigned int i = pos.x();
    unsigned int j = pos.y();
    unsigned int k = pos.z();


		const float value = v.get(i,j,k);
		//printf("%i %i %i\n", i,j,k);
		//printf("%e\n", value);
		normValue += value*value;
	}
	//system("pause");

	//printf("Norm: %e\n", normValue);
	return sqrt(normValue);
}

float dot(const FluidSolverVector& a, const FluidSolverVector& b)
{
	float dot = 0;

	const VolumeLevelSet::InsideMask& insideMask = a.getLS()->getInsideMask();
	for (unsigned int p = 0; p < insideMask.size(); p++){
    const Vector3<unsigned int>& pos = insideMask[p];
    unsigned int i = pos.x();
    unsigned int j = pos.y();
    unsigned int k = pos.z();

    dot += a.get(i,j,k)*b.get(i,j,k);
	}
	//printf("Dot: %e\n", dot);

  return dot;
}

void a_equals_b_minus_C_times_d(FluidSolverVector& a, const FluidSolverVector& b, const FluidSolverSparseMatrix& C, const FluidSolverVector& d)
{
	FluidSolverSparseMatrix::Iterator iter, iend;
	iter = C.begin();
	iend = C.end();

	while (iter != iend){
    const FluidSolverSparseMatrix::Row& row = *iter;
    const Vector3<unsigned int> pos = row.cPos;
    unsigned int i = pos.x();
    unsigned int j = pos.y();
    unsigned int k = pos.z();

		const float val = b.get(i,j,k) -
			    (
					 row.xp1*d.get(i+1,j,k) +
			     row.xm1*d.get(i-1,j,k) +

					 row.yp1*d.get(i,j+1,k) +
			     row.ym1*d.get(i,j-1,k) +

					 row.zp1*d.get(i,j,k+1) +
			     row.zm1*d.get(i,j,k-1) +

					 row.c*d.get(i,j,k)
					);

		a.set(i,j,k, val);

		assert(a.getLS()->isInside(i,j,k));

		iter++;
	}
}

void a_equals_B_times_c( FluidSolverVector& a, const FluidSolverSparseMatrix& B, const FluidSolverVector& c)
{
	FluidSolverSparseMatrix::Iterator iter, iend;
	iter = B.begin();
	iend = B.end();

	while (iter != iend){
    const FluidSolverSparseMatrix::Row& row = *iter;
    Vector3<unsigned int> pos = row.cPos;
    unsigned int i = pos.x();
    unsigned int j = pos.y();
    unsigned int k = pos.z();

		const float val = (
					 row.xp1*c.get(i+1,j,k) +
			     row.xm1*c.get(i-1,j,k) +

					 row.yp1*c.get(i,j+1,k) +
			     row.ym1*c.get(i,j-1,k) +

					 row.zp1*c.get(i,j,k+1) +
			     row.zm1*c.get(i,j,k-1) +

					 row.c*c.get(i,j,k)
					);

		a.set(i,j,k, val);

    //assert(a.getLS()->isInside(i,j,k));

		iter++;
	}
}

void a_equals_b_plus_c_times_d(FluidSolverVector& a, const FluidSolverVector& b, const float c, const FluidSolverVector& d)
{
	const VolumeLevelSet::InsideMask& insideMask = a.getLS()->getInsideMask();
	for (unsigned int p = 0; p < insideMask.size(); p++){
    const Vector3<unsigned int>& pos = insideMask[p];
    unsigned int i = pos.x();
    unsigned int j = pos.y();
    unsigned int k = pos.z();

    const float val = b.get(i,j,k) + c*d.get(i,j,k);
    a.set(i,j,k, val);
	}
}

void a_equals_a_plus_b_times_c(FluidSolverVector& a, const float b, const FluidSolverVector& c)
{
	const VolumeLevelSet::InsideMask& insideMask = a.getLS()->getInsideMask();
	for (unsigned int p = 0; p < insideMask.size(); p++){
    const Vector3<unsigned int>& pos = insideMask[p];
    unsigned int i = pos.x();
    unsigned int j = pos.y();
    unsigned int k = pos.z();

		const float val = a.get(i,j,k) + b*c.get(i,j,k);
    a.set(i,j,k, val);
	}
}

void a_equals_b(FluidSolverVector& a, const FluidSolverVector& b)
{
	const VolumeLevelSet::InsideMask& insideMask = a.getLS()->getInsideMask();
	for (unsigned int p = 0; p < insideMask.size(); p++){
    const Vector3<unsigned int>& pos = insideMask[p];
    unsigned int i = pos.x();
    unsigned int j = pos.y();
    unsigned int k = pos.z();

		const float val = b.get(i,j,k);
    a.set(i,j,k, val);
	}
}
