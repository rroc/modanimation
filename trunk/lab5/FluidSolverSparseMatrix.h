#ifndef __FLUIDSOLVER_SPARSE_MATRIX__
#define __FLUIDSOLVER_SPARSE_MATRIX__

#include <vector>
#include "Vector3.h"

class FluidSolverSparseMatrix{
	public:

		struct Row{
			float xp1;
			float xm1;

			float yp1;
			float ym1;

			float zp1;
			float zm1;

			float c; // center element

			Vector3<unsigned int> cPos; // Coordinates for center element;
		};

		typedef std::vector<Row>::const_iterator Iterator;

  	FluidSolverSparseMatrix();
  	~FluidSolverSparseMatrix();

  	void clear() {mMatrix.clear();}
  	void addRow(Row row) {mMatrix.push_back(row);}

    Iterator begin() const {return mMatrix.begin();}
    Iterator end() const {return mMatrix.end();}

    bool rowIsSingular(const Row& row) const;

	private:
		std::vector<Row> mMatrix;

};

#endif
