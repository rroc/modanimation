
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
#ifndef __implicit_h__
#define __implicit_h__

#include "Geometry.h"
#include "Bbox.h"
#include "Mesh.h"
#include "SimpleMesh.h"
#include "MarchingCubes.h"

/*!  \brief Implicit base class */
class Implicit : public Geometry{
public:
  Implicit();
  virtual ~Implicit();

  // function evaluation operators
  //! evaluate the implicit at world coordinates x y z
  virtual float getValue(float x, float y, float z) const = 0;
  //! calculate the gradient of the implicit at world coordinates x y z
  virtual Vector3<float> getGradient(float x, float y, float z, float delta = 1e-3) const;  // delta is the dx used for calculating the normal.
  //! calculate the curvature of the implicit at world coordinates x y z
  virtual float getCurvature(float x, float y, float z, float delta = 1e-3) const;  // delta is the dx used for calculating the normal.

  //! Creates a drawable mesh by running marching cubes over the bounding box
  template <class MeshType> void triangulate(float sampleDensity);
  //! Returns the mesh for outside manipultaion. Decimation etc.
  template <class MeshType> MeshType & getMesh() { return static_cast<MeshType &>(*mMesh); }

  //! Draws the triangulated mesh
  virtual void draw();
  //<! Draws gradient vectors for all vertices in mesh. Only works with SimpleMesh meshes
  virtual void drawGradients();
  //<! Draws curvature spheres for all vertices in mesh. Only works with SimpleMesh meshes
  virtual void drawCurvature();

  //! Returns axis aligned bounding box in world space
  Bbox getBoundingBox() const;
  //! Sets the bounding box in current frame coordinates
  virtual void setBoundingBox(const Bbox & b);

  //! Set transformation
  virtual void setTransform(const Matrix4x4<float> & transform);

protected:
  void transformWorld2Obj(float & x, float & y, float & z) const;

  Mesh * mMesh;
  Bbox mBox;
  Matrix4x4<float> mWorld2Obj;
};

/*!
 * Marching cubes mesh extraction. Runs over the bounding box.
 * Must be declared in h-file since templating is used.
 */
template <class MeshType>
void Implicit::triangulate(float delta)
{
  if (mMesh != NULL) {
    delete mMesh;
    mMesh = NULL;
  }

  mMesh = new MeshType();

  // Get axis aligned bounding box (in world space)
  Bbox b = getBoundingBox();
  Vector3<float> & pmin = b.pMin;
  Vector3<float> & pmax = b.pMax;

  // Prepare progress bar
  unsigned int samplesX = static_cast<unsigned int>( (pmax.x() - pmin.x()) / delta );
  unsigned int samplesY = static_cast<unsigned int>( (pmax.y() - pmin.y()) / delta );
  unsigned int samplesZ = static_cast<unsigned int>( (pmax.z() - pmin.z()) / delta );
  unsigned int totalSamples = samplesX*samplesY*samplesZ;
  unsigned int currentSample = 0;
  unsigned int reportFreq = totalSamples / 30;

  unsigned int numTriangles(0);
  // Loop over bounding box
  std::cerr << "Triangulating [";
  for(float i = pmin.x(); i < pmax.x()-0.5*delta; i += delta ) {
    for(float j = pmin.y(); j < pmax.y()-0.5*delta; j += delta ) {
      for(float k = pmin.z(); k < pmax.z()-0.5*delta; k += delta ) {
	      float voxelValues[8] = {
	        getValue(i,j,k),
	        getValue(i+delta, j, k),
	        getValue(i+delta, j+delta, k),
	        getValue(i, j+delta, k),
	        getValue(i, j, k+delta),
	        getValue(i+delta, j, k+delta),
	        getValue(i+delta, j+delta, k+delta),
	        getValue(i, j+delta, k+delta)
	      };
	      std::vector<Vector3<float> > tris = ::triangulate(voxelValues, i, j, k, delta);
	      for(unsigned int n = 0; n < tris.size(); n+=3){
	        mMesh->addTriangle(tris.at(n), tris.at(n+2), tris.at(n+1)); // triangulation uses clockwise orientation
			numTriangles++;
	      }

        currentSample++;
        if (currentSample % reportFreq == 0)
          std::cerr << "=";
      }
    }
  }
  std::cerr << "]" << std::endl<< "done: " << numTriangles << std::endl;
}



#endif
