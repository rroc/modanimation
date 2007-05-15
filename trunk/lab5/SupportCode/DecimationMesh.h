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
#ifndef _DECIMATION_MESH
#define _DECIMATION_MESH

#include "DecimationInterface.h"
#include "HalfEdgeMesh.h"
#include "Heap.h"

class DecimationMesh : public DecimationInterface, public HalfEdgeMesh
{
public :

  DecimationMesh() : mNumCollapsedVerts(0), mNumCollapsedEdges(0), mNumCollapsedFaces(0) { }
  virtual ~DecimationMesh() { }

  /*! The EdgeCollapse is a Heapable type */
  struct EdgeCollapse : public Heap::Heapable
  {
    unsigned int halfEdge;
    Vector3<float> position;
  };

  virtual void initialize();

  virtual bool decimate();

  virtual bool decimate(unsigned int targetFaces);

  virtual void draw();

protected :
  virtual void updateVertexProperties(unsigned int ind);

  virtual void updateFaceProperties(unsigned int ind);

  virtual void computeCollapse(EdgeCollapse * collapse) = 0;

  bool isValidCollapse(EdgeCollapse * collapse);

  inline bool isVertexCollapsed(unsigned int ind) { return mCollapsedVerts[ind]; }
  inline bool isEdgeCollapsed(unsigned int ind) { return mCollapsedEdges[ind]; }
  inline bool isFaceCollapsed(unsigned int ind) { return mCollapsedFaces[ind]; }

  inline void collapseVertex(unsigned int ind) {
    mCollapsedVerts[ind] = true;
    mNumCollapsedVerts++;
  }
  inline void collapseEdge(unsigned int ind) {
    mHalfEdge2EdgeCollapse[ind] = NULL;
    mCollapsedEdges[ind] = true;
    mNumCollapsedEdges++;
  }
  inline void collapseFace(unsigned int ind) {
    mCollapsedFaces[ind] = true;
    mNumCollapsedFaces++;
  }

  //! State array of 'active' verts
  std::vector<bool> mCollapsedVerts;
  //! State array of 'active' edges
  std::vector<bool> mCollapsedEdges;
  //! State array of 'active' faces
  std::vector<bool> mCollapsedFaces;

  //! Number of collapsed verts
  unsigned int mNumCollapsedVerts;
  //! Number of collapsed edges
  unsigned int mNumCollapsedEdges;
  //! Number of collapsed faces
  unsigned int mNumCollapsedFaces;

  //! Utility mapping between half edges and collapses
  std::vector<EdgeCollapse *> mHalfEdge2EdgeCollapse;

  //! The heap that stores the edge collapses
  Heap mHeap;
};

#endif
