/*************************************************************************************************
 *
 * Modeling and animation (TNM079) 2007
 * Code base for lab assignments. Copyright:
 *   Gunnar Johansson (gunnar.johansson@itn.liu.se)
 *   Ken Museth (ken.museth@itn.liu.se)
 *   Michael Bang Nielsen (bang@daimi.au.dk)
 *   Ola Nilsson (ola.nilsson@itn.liu.se)
 *   Andreas S�derstr�m (andreas.soderstrom@itn.liu.se)
 *
 *************************************************************************************************/
#include "SimpleDecimationMesh.h"



void SimpleDecimationMesh::computeCollapse(EdgeCollapse * collapse)
{
  // This edge collapse places the new vertex halfway along the edge.
  // The cost is computed as the vertex-to-vertex distance between the new vertex
  // and the old vertices at the edge's endpoints.

  // First, get the vertices at the endpoints
  const Vector3<float>& v0 = mVerts[ mEdges[collapse->halfEdge].vert ].vec;
  const Vector3<float>& v1 = mVerts[ mEdges[mEdges[collapse->halfEdge].pair].vert ].vec;

  // Then, compute the new position and the cost
  collapse->position = (v0 + v1)*0.5;
  collapse->cost = (collapse->position - v0).length();
}

