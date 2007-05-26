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
#ifndef _LOOP_SUBDIVISION_MESH_
#define _LOOP_SUBDIVISION_MESH_

#include "HalfEdgeMesh.h"

/*! \brief Subdivision mesh that implements the Loop scheme
*/
class LoopSubdivisionMesh : public HalfEdgeMesh
{
public :

  LoopSubdivisionMesh(const HalfEdgeMesh & m, unsigned int s) : HalfEdgeMesh(m), mNumSubDivs(s) { }
  LoopSubdivisionMesh() : mNumSubDivs(0) { }

  virtual ~LoopSubdivisionMesh() { }

  //! Subdivides the mesh uniformly one step
  virtual bool subdivide();

  //! Loops over the edges incident to the vertex and collects all pairs' verts
  //! Almost identical to findNeighbourTris
  bool findNeighbourVerts(const unsigned int vertexIndex, std::vector<unsigned int>& foundVerts) const;

  //! Return weights for interior verts
  float beta(unsigned int valence);

  //! Uses the HalfEdge::draw
  virtual void draw() { HalfEdgeMesh::draw(); }

protected :
  //! The number of accumulated subdivisions
  unsigned int mNumSubDivs;

  //! Computes a new vertex, replacing a vertex in the old mesh
  Vector3<float> computeVertex(unsigned int edgeIndex);

  //! Computes a new vertex, placed along an edge in the old mesh
  Vector3<float> computeEdgeVertex(unsigned int edgeIndex);
};

#endif
