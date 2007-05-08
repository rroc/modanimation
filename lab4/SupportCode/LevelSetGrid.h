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
#ifndef __levelset_grid_h__
#define __levelset_grid_h__

#include "Volume.h"
#include "Vector3.h"
#include "BitMask3D.h"
#include <iostream>
#include <limits>

class LevelSetGrid
{
protected:
  Volume<float> mPhi;
  BitMask3D mMask;
  float mInsideConstant, mOutsideConstant;


public:

  LevelSetGrid(int dimX=0, int dimY=0, int dimZ=0,
               float insideConstant=-std::numeric_limits<float>::max(),
               float outsideConstant=std::numeric_limits<float>::max())
    : mPhi(dimX, dimY, dimZ, outsideConstant), mMask(dimX, dimY, dimZ),
      mInsideConstant(insideConstant), mOutsideConstant(outsideConstant) { }

  ~LevelSetGrid() { }



  class Iterator
  {
    friend class LevelSetGrid;

  protected:
    const BitMask3D * mask;
    int i,j,k;
    int iMax, jMax, kMax;
    bool endState;

    Iterator(const BitMask3D * mask, int i=0, int j=0, int k=0);


  public :

    inline Iterator & operator ++ (int) {
      endState = false;
      if (endState != true){
        do{
          ++k;
          if(k >= kMax) {
            k = 0;
            ++j;
            if(j >= jMax){
              j = 0;
              ++i;
              if(i >= iMax){
		            k = kMax;
		            j = jMax;
		            i = iMax;
                endState = true;
              }
            }
          }
        } while (!endState && (mask->getValue(i,j,k) != true));
      }
      return *this;
    }

    bool operator !=(const Iterator & b) const {
      return (this->i != b.i || this->j != b.j || this->k != b.k);
    }

    int getI() const { return i; }
    int getJ() const { return j; }
    int getK() const { return k; }
  };


  Iterator beginNarrowBand() { return Iterator(&mMask); }
  const Iterator beginNarrowBand() const { return Iterator(&mMask); }

  Iterator endNarrowBand() { return Iterator(&mMask, mMask.getDimX(), mMask.getDimY(), mMask.getDimZ()); }
  const Iterator endNarrowBand() const { return Iterator(&mMask, mMask.getDimX(), mMask.getDimY(), mMask.getDimZ()); }

  inline int getDimX() const { return mPhi.getDimX(); }
  inline int getDimY() const { return mPhi.getDimY(); }
  inline int getDimZ() const { return mPhi.getDimZ(); }

  //! Return grid dimensions as measured in number of grid cells
	Vector3<int> getDimensions();

  float getValue(int i, int j, int k) const;
  void setValue(int i, int j, int k, float f);

  void setInsideConstant(float insideConstant) { mInsideConstant = insideConstant; }
  inline const float getInsideConstant() const { return mInsideConstant; }

  void setOutsideConstant(float outsideConstant) { mOutsideConstant = outsideConstant; }
  inline const float getOutsideConstant() const { return mOutsideConstant; }

  //! Dilates the narrow band with 6 connectivity
  void dilate();

  //! Rebuild the narrow band by culling too large values from mask
  void rebuild();



  friend std::ostream& operator << (std::ostream &os, const LevelSetGrid &grid)
  {
    os << "Grid dimensions: " << "(" << grid.getDimX() << "x" << grid.getDimY() << "x" << grid.getDimZ() << ")" << std::endl;
    Iterator iter = grid.beginNarrowBand();
    Iterator iend = grid.endNarrowBand();
    while (iter != iend) {
      int i = iter.getI(), j = iter.getJ(), k = iter.getK();
      os << "(" << i << "," << j << "," << k << "): " << grid.getValue(i,j,k) << std::endl;
      iter++;
    }
    return os;
  }
};


#endif
