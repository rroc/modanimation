/*************************************************************************************************
 *
 * Modeling and animation (TNM079) 2007
 * Code base for lab assignments. Copyright:
 *   Gunnar Johansson (gunnar.johansson@itn.liu.se)
 *   Ken Museth (ken.museth@itn.liu.se)
 *   Michael Bang Nielsen (bang@daimi.au.dk)
 *   Ola Nilsson (ola.nilsson@itn.liu.se)
 *   Andreas Sderstrm (andreas.soderstrom@itn.liu.se)
 *
 *************************************************************************************************/
#ifndef __volume_h__
#define __volume_h__

#include <vector>
#include <cassert>
#include "Vector3.h"
#include "Util.h"

/*!
 * A 3D volume of templated type T.
 * Stores values in an stl vector<T>. Uses row major storage, i.e. dimZ's is changing most
 * rapidly.
 */
template<class T>
class Volume{
protected:
  //! An stl vector to hold actual data
  std::vector<T> mData;
  //! The dimensions of the volume
  int mDimX, mDimY, mDimZ;
  //! premult = dimY*dimZ, avoids this multiplication for each getValue
  int mPremult;

public:
  //! Default constructor initializes to zero volume
  Volume() : mData(0), mDimX(0), mDimY(0), mDimZ(0), mPremult(0) {}
  //! Sized constructor initializes volume of size dimXxdimYxdimZ to default value for type T
  Volume(int dimX, int dimY, int dimZ)
    : mData(dimX*dimY*dimZ, T()), mDimX(dimX), mDimY(dimY), mDimZ(dimZ), mPremult(dimY*dimZ){}

  Volume(int dimX, int dimY, int dimZ, T defaultVal)
    : mData(dimX*dimY*dimZ, defaultVal), mDimX(dimX), mDimY(dimY), mDimZ(dimZ), mPremult(dimY*dimZ){}

  inline int getDimX() const { return mDimX; }
  inline int getDimY() const { return mDimY; }
  inline int getDimZ() const { return mDimZ; }
  //! Returns the value at i,j,k
  T getValue(int i, int j, int k) const {
    i = clamp(i, 0, mDimX-1);
    j = clamp(j, 0, mDimY-1);
    k = clamp(k, 0, mDimZ-1);
    return mData.at(i*mPremult + j*mDimZ + k); // .at() does bound checking, throws exception
    //return mData[i*premult + j*dimZ + k]; // op [] does no bound checking
  }

	//! Returns the value at x,y,z (uses trilinear interpolation
	T getValue(float x, float y, float z) const
	{
  	int i = (int)x;
  	int j = (int)y;
  	int k = (int)z;

  	float bx = x - i;
  	float by = y - j;
  	float bz = z - k;

  	T val =
    	getValue(i,   j,   k  ) * (1-bx) * (1-by) * (1-bz) +
    	getValue(i+1, j,   k  ) *  bx    * (1-by) * (1-bz) +
    	getValue(i+1, j+1, k  ) *  bx    *  by    * (1-bz) +
    	getValue(i,   j+1, k  ) * (1-bx) *  by    * (1-bz) +
    	getValue(i,   j,   k+1) * (1-bx) * (1-by) *  bz    +
    	getValue(i+1, j,   k+1) *  bx    * (1-by) *  bz    +
    	getValue(i+1, j+1, k+1) *  bx    *  by    *  bz    +
    	getValue(i,   j+1, k+1) * (1-bx) *  by    *  bz;

  	return val;
	}



  //! Sets the value at i,j,k to val
  void setValue(int i, int j, int k, const T& val){
    assert(i < mDimX && i >= 0 && j < mDimY && j >= 0 && k < mDimZ && k >= 0);
    mData.at(i*mPremult + j*mDimZ + k) = val; // .at() does bound checking, throws exception
    // mData[i*premult + j*dimZ + k] = val;
  }
  //! Load a volume from binary stream is
  void load(std::istream & is){

  }
  //! Save volume to binary stream os
  void save(std::ostream & os){

  }
};




#endif
