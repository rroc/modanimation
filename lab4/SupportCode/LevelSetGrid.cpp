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

#include "LevelSetGrid.h"
#include "Util.h"


LevelSetGrid::Iterator::Iterator(const BitMask3D * mask, int i, int j, int k)
{
  // Initialize counters
  this->mask = mask;
  this->iMax = mask->getDimX();
  this->jMax = mask->getDimY();
  this->kMax = mask->getDimZ();
  this->i = clamp(i, (int)0, iMax);
  this->j = clamp(j, (int)0, jMax);
  this->k = clamp(k, (int)0, kMax);

  // Check if we're within range
  if(i == iMax && j == jMax && k == kMax){
    endState = true;
  } else {
    endState = false;
  }

  // Go to the first valid entry
  if (!endState && !mask->getValue(i,j,k)) (*this)++;
}


float LevelSetGrid::getValue(int i, int j, int k) const
{
  // skip mask check, just access
  return mPhi.getValue(i,j,k);
}


void LevelSetGrid::setValue(int i, int j, int k, float f)
{
  // Implicitly sets the mask of (i,j,k) to true
  mMask.setValue(i,j,k,true);
  mPhi.setValue(i,j,k, f);
}


void LevelSetGrid::dilate()
{
  BitMask3D newMask(mMask);
  Iterator it = beginNarrowBand();
  Iterator iend = endNarrowBand();
  while(it!=iend){
    int i = it.getI();
    int j = it.getJ();
    int k = it.getK();
    newMask.setValue(i, j, k, true);
    if (k < getDimZ()-1)  newMask.setValue(i, j, k+1, true);
    if (k > 0)            newMask.setValue(i, j, k-1, true);
    if (j < getDimY()-1)  newMask.setValue(i, j+1, k, true);
    if (j > 0)            newMask.setValue(i, j-1, k, true);
    if (i < getDimX()-1)  newMask.setValue(i+1, j, k, true);
    if (i > 0)            newMask.setValue(i-1, j, k, true);
    it++;
  }
  mMask = newMask;
}


void LevelSetGrid::rebuild()
{
  Iterator it = beginNarrowBand();
  Iterator iend = endNarrowBand();
  while(it!=iend){
    int i = it.getI();
    int j = it.getJ();
    int k = it.getK();

	//NARROW BANDING
	float phi = mPhi.getValue(i,j,k);
	float phiAbs = fabs(phi);

	//float gamma = mOutsideConstant * 1.5f;

	//|phi| <= beta
	//use the value...

	////|phi| > gamma
	//if( phiAbs > gamma ) 
	//	{
	//	mPhi.setValue(i, j, k, 0.0f );
	//	mMask.setValue(i, j, k, false);
	//	}

	//// beta < |phi| <= gamma
	//else if( phiAbs > mOutsideConstant && phiAbs <= gamma ) 
	//	{
	//	float value = 
	//		( (2.0f*phiAbs + gamma + - 3.0f*mOutsideConstant) * ((phiAbs-gamma)*(phiAbs-gamma)) ) 
	//		/ pow((gamma - mOutsideConstant), 3.0f );
	//	mPhi.setValue(i, j, k, value );
	//	mMask.setValue(i, j, k, true );
	//	}

	////OUTSIDE GAMMA
	//if(mPhi.getValue(i,j,k) > mOutsideConstant ) {
	//	mPhi.setValue(i, j, k, 0.0f);
	//	mMask.setValue(i, j, k, false);
	//	}
	//else if(mPhi.getValue(i,j,k) < mInsideConstant ) {
	//	mPhi.setValue(i, j, k, 0.0f);
	//	mMask.setValue(i, j, k, false);
	//	}

	////OUTSIDE BETA
	//else if(mPhi.getValue(i,j,k) > mOutsideConstant * (2.0f/3.0f) ) {
	//	float value = ( (2.0f*phiAbs + mOutsideConstant + - 3.0f*mOutsideConstant * (2.0f/3.0f) ) * ((phiAbs-mOutsideConstant)*(phiAbs-mOutsideConstant)) ) 
	//				 / pow((mOutsideConstant - mOutsideConstant * (2.0f/3.0f)), 3.0f );
	//	mPhi.setValue(i, j, k, value);
	//	mMask.setValue(i, j, k, false);
	//	}
	//else if(mPhi.getValue(i,j,k) < mInsideConstant * (2.0f/3.0f)  ) {
	//	float value = ( (2.0f*phiAbs + mInsideConstant + - 3.0f*mInsideConstant * (2.0f/3.0f) ) * ((phiAbs-mInsideConstant)*(phiAbs-mInsideConstant)) ) 
	//		/ pow((mInsideConstant - mInsideConstant * (2.0f/3.0f)), 3.0f );
	//	mPhi.setValue(i, j, k, value);
	//	mMask.setValue(i, j, k, false);
	//	}


    //    std::cerr << mPhi.getValue(i,j,k) << " -> " ;
    if(mPhi.getValue(i,j,k) > mOutsideConstant) {
      mPhi.setValue(i, j, k, mOutsideConstant);
      mMask.setValue(i, j, k, false);
    }
    else if(mPhi.getValue(i,j,k) < mInsideConstant) {
      mPhi.setValue(i, j, k, mInsideConstant);
      mMask.setValue(i, j, k, false);
    }

    //    std::cerr << mPhi.getValue(i,j,k) << ", " ;
    it++;
  }
}


Vector3<int> LevelSetGrid::getDimensions()
{
  return Vector3<int>(getDimX(), getDimY(), getDimZ());
}

