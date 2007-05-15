#ifndef __NAVIER_STOKES_VECTOR_FIELD_H__
#define __NAVIER_STOKES_VECTOR_FIELD_H__

#include "Function3D.h"
#include "Volume.h"
#include "Matrix4x4.h"
#include "Vector4.h"
#include "Bbox.h"

class NavierStokesVectorField : public Function3D<Vector3<float> > {
	public:
    NavierStokesVectorField(const Volume<Vector3<float> >& field, const Matrix4x4<float> worldToObjTransform, float dx, Bbox bbox) :
			mField(field), mWorldToObjTransform(worldToObjTransform), mDx(dx), mBbox(bbox)
		{
      mNumVoxels = Vector3<int>(field.getDimX(), field.getDimY(), field.getDimZ());

      // Find maximum velocity in x,y,z directions
      mMaxVel = Vector3<float>(mDx*0.001);
      /*Vector3<float> max(std::numeric_limits<float>::min());

      for (int i = 0; i < mNumVoxels.x(); i++){
        for (int i = 0; i < mNumVoxels.x(); i++){
          for (int i = 0; i < mNumVoxels.x(); i++){
						max.x() = std::max(max.x(), fabs(mField.getValue(i,j,k)));
						max.y() = std::max(max.y(), fabs(mField.getValue(i,j,k)));
						max.z() = std::max(max.z(), fabs(mField.getValue(i,j,k)));
					}
				}
			}*/
		}

		~NavierStokesVectorField() {}

  	Vector3<float> virtual getMaxValue() const
		{
			return mMaxVel;
		}

    Vector3<float> virtual getValue(int i, int j, int k) const
    {
      return mField.getValue(i,j,k);
    }

//  	Vector3<float> virtual getValue(float xf, float yf, float zf) const
Vector3<float> virtual getValue(float x, float y, float z) const
	{
		// Transform to grid coordinates..
  	/*Vector4<float> vec(xf,yf,zf,1.0f);
  	vec = mWorldToObjTransform*vec;

		vec[0] -= mBbox.pMin.x();
		vec[1] -= mBbox.pMin.y();
		vec[2] -= mBbox.pMin.z();

		int i = (int)round(vec[0]/mDx);
		int j = (int)round(vec[1]/mDx);
		int k = (int)round(vec[2]/mDx);

    float xmult = (mBbox.pMax.x() - mBbox.pMin.x()) / mDx;
    float ymult = (mBbox.pMax.y() - mBbox.pMin.y()) / mDx;
    float zmult = (mBbox.pMax.z() - mBbox.pMin.z()) / mDx;

		int i = (int)round((xf - mBbox.pMin.x())/xmult);
		int j = (int)round((yf - mBbox.pMin.y())/ymult);
		int k = (int)round((zf - mBbox.pMin.z())/zmult);
*/

  // Transform to object space
  Vector4<float> vprim, v = Vector4<float>(x, y, z, 1.f);
  vprim = mWorldToObjTransform*v;
  x = vprim[0]; y = vprim[1]; z = vprim[2];
  
  // Compute the grid indices
  int i = (int)round((x - mBbox.pMin.x())/mDx);
  int j = (int)round((y - mBbox.pMin.y())/mDx);
  int k = (int)round((z - mBbox.pMin.z())/mDx);

		return mField.getValue(i,j,k);
  }


	private:
		Vector3<float> mMaxVel;
		const Volume<Vector3<float> >& mField;
		const Matrix4x4<float> mWorldToObjTransform;
		const float mDx;
		Vector3<int> mNumVoxels;
		const Bbox mBbox;
};

#endif
