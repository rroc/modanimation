/*************************************************************************************************
*
* Modeling and animation (TNM079) 2007
* Code base for lab assignments. Copyright:
*   Gunnar Johansson (gunnar.johansson@itn.liu.se)
*   Ken Museth (ken.museth@itn.liu.se)
*   Michael Bang Nielsen (bang@daimi.au.dk)
*   Ola Nilsson (ola.nilsson@itn.liu.se)
*   Andreas Söderström (andreas.soderstrom@itn.liu.se)
*
*************************************************************************************************/

#include "LevelSet.h"
#include "Util.h"


LevelSet::LevelSet(float dx) : mDx(dx)
	{
	}


LevelSet::LevelSet(float dx, const Implicit & impl) : mDx(dx)
	{
	// Get the bounding box (in world space) from the implicit
	// function to initialize the level set's bounding box.
	Bbox b = impl.getBoundingBox();
	setBoundingBox(b);

	// Loop over volume and sample the implicit function
	int i = 0, j = 0, k = 0;
	for(float x = mBox.pMin.x(); x < mBox.pMax.x()+0.5*mDx; x += mDx, i++) {
		for(float y = mBox.pMin.y(); y < mBox.pMax.y()+0.5*mDx; y += mDx, j++) {
			for(float z = mBox.pMin.z(); z < mBox.pMax.z()+0.5*mDx; z += mDx, k++ ) {
				mGrid.setValue(i,j,k, impl.getValue(x,y,z));
				}
			k = 0;
			}
		j = 0;
		}
	}


LevelSet::LevelSet(float dx, const Implicit & impl, const Bbox & box) : mDx(dx)
	{
	setBoundingBox(box);

	// Loop over volume and sample the implicit function
	int i = 0, j = 0, k = 0;
	for(float x = mBox.pMin.x(); x < mBox.pMax.x()+0.5*mDx; x += mDx, i++) {
		for(float y = mBox.pMin.y(); y < mBox.pMax.y()+0.5*mDx; y += mDx, j++) {
			for(float z = mBox.pMin.z(); z < mBox.pMax.z()+0.5*mDx; z += mDx, k++ ) {
				mGrid.setValue(i,j,k, impl.getValue(x,y,z));
				}
			k = 0;
			}
		j = 0;
		}
	}


float LevelSet::getValue(float x, float y, float z) const
	{
	transformWorld2Obj(x,y,z);

	int i = (int)((x - mBox.pMin.x()) / mDx);
	int j = (int)((y - mBox.pMin.y()) / mDx);
	int k = (int)((z - mBox.pMin.z()) / mDx);

	float bx = (x - mBox.pMin.x()) / mDx - i;
	float by = (y - mBox.pMin.y()) / mDx - j;
	float bz = (z - mBox.pMin.z()) / mDx - k;

	if (i == mGrid.getDimX()-1) {
		i--;
		bx = 1-bx;
		}
	if (j == mGrid.getDimY()-1) {
		j--;
		by = 1-by;
		}
	if (k == mGrid.getDimZ()-1) {
		k--;
		bz = 1-bz;
		}

	float val =
		mGrid.getValue(i,   j,   k  ) * (1-bx) * (1-by) * (1-bz) +
		mGrid.getValue(i+1, j,   k  ) *  bx    * (1-by) * (1-bz) +
		mGrid.getValue(i+1, j+1, k  ) *  bx    *  by    * (1-bz) +
		mGrid.getValue(i,   j+1, k  ) * (1-bx) *  by    * (1-bz) +
		mGrid.getValue(i,   j,   k+1) * (1-bx) * (1-by) *  bz    +
		mGrid.getValue(i+1, j,   k+1) *  bx    * (1-by) *  bz    +
		mGrid.getValue(i+1, j+1, k+1) *  bx    *  by    *  bz    +
		mGrid.getValue(i,   j+1, k+1) * (1-bx) *  by    *  bz;

	return val;
	}


/*!
* Evaluates gradient at (x,y,z) through discrete finite difference scheme.
*/
Vector3<float> LevelSet::getGradient(float x, float y, float z, float delta) const
	{
	transformWorld2Obj(x,y,z);

	int i = (int)((x - mBox.pMin.x()) / mDx);
	int j = (int)((y - mBox.pMin.y()) / mDx);
	int k = (int)((z - mBox.pMin.z()) / mDx);

	//printf("[%f, %f, %f]\n", diffXpm(x,y,z), diffYpm(x,y,z), diffZpm(x,y,z) );
	return Vector3<float>( diffXpm(i,j,k), diffYpm(i,j,k), diffZpm(i,j,k) );
	//return Implicit::getGradient(x, y, z, delta);
	}

void LevelSet::setBoundingBox(const Bbox & b)
	{
	// Loop over existing grid to find the maximum and minimum values
	// stored. These are used to initialize the new grid with decent values.
	LevelSetGrid::Iterator iter = mGrid.beginNarrowBand();
	LevelSetGrid::Iterator iend = mGrid.endNarrowBand();
	float maxVal = -std::numeric_limits<float>::max();
	float minVal = std::numeric_limits<float>::max();
	while (iter != iend) {
		int i = iter.getI();
		int j = iter.getJ();
		int k = iter.getK();

		float val = mGrid.getValue(i,j,k);
		if (maxVal < val)  maxVal = val;
		if (minVal > val)  minVal = val;
		iter++;
		}

	// Create a new grid with requested size
	Vector3<float> extent = b.pMax - b.pMin;
	int dimX = (int)round(extent.x()/mDx) + 1;
	int dimY = (int)round(extent.y()/mDx) + 1;
	int dimZ = (int)round(extent.z()/mDx) + 1;
	LevelSetGrid grid(dimX, dimY, dimZ, minVal, maxVal);

	// Copy all old values to new grid
	iter = mGrid.beginNarrowBand();
	while (iter != iend) {
		int i = iter.getI();
		int j = iter.getJ();
		int k = iter.getK();

		// Get the (x,y,z) coordinates of grid point (i,j,k)
		float x = i*mDx + mBox.pMin.x();
		float y = j*mDx + mBox.pMin.y();
		float z = k*mDx + mBox.pMin.z();

		// Check that (x,y,z) is inside the new bounding box
		if (x < b.pMin.x() || x > b.pMax.x() ||
			y < b.pMin.y() || y > b.pMax.y() ||
			z < b.pMin.z() || z > b.pMax.z()) {
				iter++;
				continue;
			}

		// Compute the new grid point (l,m,n)
		int l = (int)round((x - b.pMin.x()) / mDx);
		int m = (int)round((y - b.pMin.y()) / mDx);
		int n = (int)round((z - b.pMin.z()) / mDx);

		grid.setValue(l,m,n, mGrid.getValue(i,j,k));
		iter++;
		}

	// Set inside and outside constants
	grid.setInsideConstant( mGrid.getInsideConstant() );
	grid.setOutsideConstant( mGrid.getOutsideConstant() );

	// Set the new bounding box
	Implicit::setBoundingBox(b);

	// Reassign the new grid
	mGrid = grid;

	std::cerr << "Level set created with grid size: " << mGrid.getDimensions() << std::endl;
	}


void LevelSet::setNarrowBandWidth(float width)
	{
	mGrid.setInsideConstant(-width*0.5*mDx);
	mGrid.setOutsideConstant(width*0.5*mDx);
	mGrid.rebuild();
	}

//! \lab4
float LevelSet::diffXm(int i, int j, int k)  const
	{
	float current =	 mGrid.getValue(i,j,k);
	float previous = mGrid.getValue(i-1,j,k);

	return (current-previous) / mDx;
	}

//! \lab4
float LevelSet::diffXp(int i, int j, int k)  const
	{
	float current =	 mGrid.getValue(i,j,k);
	float next = mGrid.getValue(i+1,j,k);

	return (next - current) / mDx;
	}

//! \lab4
float LevelSet::diffXpm(int i, int j, int k)  const
	{
	float next		= mGrid.getValue(i+1,j,k);
	float previous	= mGrid.getValue(i-1,j,k);

	return (next-previous) / (2.0f*mDx);	}

//! \lab4
float LevelSet::diff2Xpm(int i, int j, int k)  const
	{
	float current	= mGrid.getValue(i,j,k);
	float next		= mGrid.getValue(i+1,j,k);
	float previous	= mGrid.getValue(i-1,j,k);

	return (next-2.0f*current+previous)/(mDx*mDx);
	}

//////Y 

//! \lab4
float LevelSet::diffYm(int i, int j, int k)  const
	{
	float current =	 mGrid.getValue(i,j,k);
	float previous = mGrid.getValue(i,j-1,k);

	return (current-previous) / mDx;
	}

//! \lab4
float LevelSet::diffYp(int i, int j, int k)  const
	{
	float current =	 mGrid.getValue(i,j,k);
	float next = mGrid.getValue(i,j+1,k);

	return (next - current) / mDx;
	}

//! \lab4
float LevelSet::diffYpm(int i, int j, int k)  const
	{
	float next		= mGrid.getValue(i,j+1,k);
	float previous	= mGrid.getValue(i,j-1,k);

	return (next-previous) / (2.0f*mDx);	
	}

//! \lab4
float LevelSet::diff2Ypm(int i, int j, int k)  const
	{
	float current	= mGrid.getValue(i,j,k);
	float next		= mGrid.getValue(i,j+1,k);
	float previous	= mGrid.getValue(i,j-1,k);

	return (next-2.0f*current+previous)/(mDx*mDx);
	}

/////Z
//! \lab4
float LevelSet::diffZm(int i, int j, int k)  const
	{
	float current =	 mGrid.getValue(i,j,k);
	float previous = mGrid.getValue(i,j,k-1);

	return (current-previous) / mDx;
	}

//! \lab4
float LevelSet::diffZp(int i, int j, int k)  const
	{
	float current =	 mGrid.getValue(i,j,k);
	float next = mGrid.getValue(i,j,k+1);

	return (next - current) / mDx;
	}

//! \lab4
float LevelSet::diffZpm(int i, int j, int k)  const
	{
	float next		= mGrid.getValue(i,j,k+1);
	float previous	= mGrid.getValue(i,j,k-1);

	return (next-previous) / (2.0f*mDx);	
	}

//! \lab4
float LevelSet::diff2Zpm(int i, int j, int k)  const
	{
	float current	= mGrid.getValue(i,j,k);
	float next		= mGrid.getValue(i,j,k+1);
	float previous	= mGrid.getValue(i,j,k-1);

	return (next-2.0f*current+previous)/(mDx*mDx);
	}


//! \lab4
float LevelSet::diff2XYpm(int i, int j, int k)  const
	{
	float nextI_nextJ = mGrid.getValue(i+1,j+1,k);
	float nextI_prevJ = mGrid.getValue(i+1,j-1,k);
	float prevI_prevJ = mGrid.getValue(i-1,j-1,k);
	float prevI_nextJ = mGrid.getValue(i-1,j+1,k);

	return (nextI_nextJ - nextI_prevJ + prevI_prevJ - prevI_nextJ )/(4.0f*mDx*mDx);
	}

//! \lab4
float LevelSet::diff2YZpm(int i, int j, int k)  const
	{
	float nextJ_nextK = mGrid.getValue(i,j+1,k+1);
	float nextJ_prevK = mGrid.getValue(i,j+1,k-1);
	float prevJ_prevK = mGrid.getValue(i,j-1,k-1);
	float prevJ_nextK = mGrid.getValue(i,j-1,k+1);

	return (nextJ_nextK - nextJ_prevK + prevJ_prevK - prevJ_nextK )/(4.0f*mDx*mDx);
	}

//! \lab4
float LevelSet::diff2ZXpm(int i, int j, int k)  const
	{
	float nextK_nextI = mGrid.getValue(i+1,j,k+1);
	float nextK_prevI = mGrid.getValue(i-1,j,k+1);
	float prevK_prevI = mGrid.getValue(i-1,j,k-1);
	float prevK_nextI = mGrid.getValue(i+1,j,k-1);

	return (nextK_nextI - nextK_prevI + prevK_prevI - prevK_nextI )/(4.0f*mDx*mDx);
	}

void LevelSet::draw()
	{
	Implicit::draw();
	}
