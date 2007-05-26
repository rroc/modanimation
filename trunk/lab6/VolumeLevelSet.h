#ifndef __VOLUME_LEVEL_SET_H__
#define __VOLUME_LEVEL_SET_H__

#include "LevelSet.h"
#include "Volume.h"
#include "NavierStokesVectorField.h"

#ifdef __APPLE__
#include "GLUT/glut.h"
#else
#include "GL/glut.h"
#endif
#include <vector>

class VolumeLevelSet : public LevelSet{
public:
		typedef std::vector<Vector3<int> > VolumeMask;
		typedef std::vector<Vector3<int> > InsideMask;

  	VolumeLevelSet(float dx);
  	VolumeLevelSet(float dx, const Implicit & impl);
    ~VolumeLevelSet();

    virtual void setBoundingBox(const Bbox & b);
    virtual void setNarrowBandWidth(float width);

    virtual void draw();

    Volume<Vector3<float> >* getVelocityField();
    float getInsideConstant() {return mInsideConstant*mDx;}

    void buildMasks();
    const VolumeMask& getVolumeMask() const;
    const InsideMask& getInsideMask() const;

    LevelSetGrid& getLevelSetGrid();

    bool isInside(int i, int j, int k) const;

    void buildAdvectionField();
    NavierStokesVectorField* getAdvectionField();

    template <class MeshType> void triangulate(float sampleDensity);

protected:
      void clearDisplayList();
		void commonConstructorOps();
		void drawVolumeGrid();

		Volume<Vector3<float> >* mVelocityField;
		Volume<Vector3<float> >* mAdvectionField;
		Volume<Vector3<float> >* mOldAdvectionField; // Used for velocity extension
		Volume<bool>* mInsideBoolMask;

    GLuint mDisplayList;
    bool mDisplayListExists;
		VolumeMask mVolumeMask;
		InsideMask mInsideMask;

  	static const float mInsideConstant;
};



template <class MeshType>
void VolumeLevelSet::triangulate(float delta)
{
  if (mMesh != NULL) {
    delete mMesh;
    mMesh = NULL;
  }
  delta = 1.0;

  mMesh = new MeshType();

  // Get axis aligned bounding box (in world space)
  Bbox b = getBoundingBox();

  // Prepare progress bar
  unsigned int totalSamples = mVolumeMask.size();
  unsigned int currentSample = 0;
  unsigned int reportFreq = totalSamples / 30;

  // Loop over narrow band
  std::cerr << "Triangulating (VolLS) [";

  for (unsigned int p = 0; p < mVolumeMask.size(); p++) {
    const Vector3<int>& pos = mVolumeMask[p];
    int i = pos.x();
    int j = pos.y();
    int k = pos.z();

    float voxelValues[8] = {
      mGrid.getValue(i,j,k),
      mGrid.getValue(i+1, j, k),
      mGrid.getValue(i+1, j+1, k),
      mGrid.getValue(i, j+1, k),
      mGrid.getValue(i, j, k+1),
      mGrid.getValue(i+1, j, k+1),
      mGrid.getValue(i+1, j+1, k+1),
      mGrid.getValue(i, j+1, k+1)
    };
    std::vector<Vector3<float> > tris = ::triangulate(voxelValues, i, j, k, delta);
    for(unsigned int n = 0; n < tris.size(); n+=3){
      mMesh->addTriangle(tris.at(n)*mDx + b.pMin, tris.at(n+2)*mDx + b.pMin, tris.at(n+1)*mDx + b.pMin); // triangulation uses clockwise orientation
    }

    currentSample++;
    if (currentSample % reportFreq == 0)
      std::cerr << "=";
  }
  std::cerr << "] done" << std::endl;
}

#endif
