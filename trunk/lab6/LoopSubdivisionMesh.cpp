/*************************************************************************************************
 *
 * Modeling and animation (TNM079) 2007
 * Code base for lab assignments. Copyright:
 *   Gunnar Johansson (gunnar.johansson@itn.liu.se)
 *   Ken Museth (ken.museth@itn.liu.se)
 *   Michael Bang Nielsen (bang@daimi.au.dk)
 *   Ola Nilsson (ola.nilsson@itn.liu.se)
 *   Andreas Sˆderstrˆm (andreas.soderstrom@itn.liu.se)
 *
 *************************************************************************************************/

#include "LoopSubdivisionMesh.h"
#include <cassert>

/*! Subdivides the mesh uniformly one step
*/
bool LoopSubdivisionMesh::subdivide()
{
  HalfEdgeMesh subDivMesh;
  subDivMesh.setTransform(getTransform());

  std::vector<HalfEdgeMesh::Face>::const_iterator it = mFaces.begin();
  std::vector<HalfEdgeMesh::Face>::const_iterator iend = mFaces.end();

  // loop over each face and create 4 new ones
  while(it != iend){

    // get the inner halfedges
    unsigned int e0, e1, e2;
    e0 = (*it).edge;
    e1 = mEdges[(*it).edge].next;
    e2 = mEdges[(*it).edge].prev;

    // Compute positions of the vertices
    Vector3<float> pn0 = computeVertex(e0);
    Vector3<float> pn1 = computeVertex(e1);
    Vector3<float> pn2 = computeVertex(e2);

    // Compute positions of the edge vertices
    Vector3<float> pn3 = computeEdgeVertex(e0);
    Vector3<float> pn4 = computeEdgeVertex(e1);
    Vector3<float> pn5 = computeEdgeVertex(e2);

    // add the four new triangles to new mesh
    subDivMesh.addTriangle(pn0, pn3, pn5);
    subDivMesh.addTriangle(pn3, pn4, pn5);
    subDivMesh.addTriangle(pn3, pn1, pn4);
    subDivMesh.addTriangle(pn5, pn4, pn2);

    ++it;
  }
  // Assigns the new mesh
  *this = LoopSubdivisionMesh(subDivMesh, ++mNumSubDivs);
  // Change return value....
  return mNumSubDivs;
}


/*! Computes a new vertex, replacing a vertex in the old mesh
*/
Vector3<float> LoopSubdivisionMesh::computeVertex(unsigned int edgeIndex)
{
  // Get the current vertex
  Vector3<float> v = mVerts.at( mEdges.at(edgeIndex).vert ).vec;


  return v;
}


/*! Computes a new vertex, placed along an edge in the old mesh
*/
Vector3<float> LoopSubdivisionMesh::computeEdgeVertex(unsigned int edgeIndex)
{

  // Place the edge vertex halfway along the edge
  HalfEdge & e0 = mEdges.at(edgeIndex);
  HalfEdge & e1 = mEdges.at(e0.pair);
  Vector3<float> & v0 = mVerts.at(e0.vert).vec;
  Vector3<float> & v1 = mVerts.at(e1.vert).vec;
  return (v0 + v1) * 0.5;
}



//! Loops over the edges incident to the vertex and collects all pairs' verts
//! Almost identical to findNeighbourTris
bool LoopSubdivisionMesh::findNeighbourVerts(const unsigned int vertexIndex, std::vector<unsigned int>& foundVerts) const
{
  foundVerts.clear();

  // Add your code here
  return !(foundVerts.size() == 0);
}


//! Return weights for interior verts
float LoopSubdivisionMesh::beta(unsigned int valence){
  if(valence == 6){
    return 1.f / 16.f;
  } else if(valence == 3){
    return 3.f / 16.f;
  } else{
    return  3.f / (8.f * valence);
  }
}
