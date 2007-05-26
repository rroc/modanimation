#include "FluidSolverSparseMatrix.h"

FluidSolverSparseMatrix::FluidSolverSparseMatrix()
{
}

FluidSolverSparseMatrix::~FluidSolverSparseMatrix()
{
}

bool FluidSolverSparseMatrix::rowIsSingular(const Row& row) const
{
	bool isSingular = false;

	if (row.c == 0.0){
		isSingular = true;
	}
	else if (row.xp1 == 0.0 && row.xm1 == 0.0 && row.yp1 == 0.0 &&
					 row.ym1 == 0.0 && row.zp1 == 0.0 && row.zm1 == 0.0)
	{
		isSingular = true;
	}

	/*if (row.c != 6.0){
		isSingular = true;
	}
	else if (row.xp1 == 0.0 || row.xm1 == 0.0 || row.yp1 == 0.0 ||
					 row.ym1 == 0.0 || row.zp1 == 0.0 || row.zm1 == 0.0)
	{
		isSingular = true;
	}*/



	return isSingular;
}
