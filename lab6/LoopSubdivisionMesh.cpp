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
	while(it != iend)
		{

		// get the inner halfedges
		unsigned int e0, e1, e2;
		e0 = (*it).edge;
		e1 = mEdges[(*it).edge].next;
		e2 = mEdges[(*it).edge].prev;

		// Compute positions of the old vertices
		Vector3<float> pn0 = computeVertex(e0);
		Vector3<float> pn1 = computeVertex(e1);
		Vector3<float> pn2 = computeVertex(e2);

		// Compute positions of the new vertices on the edge
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
	std::vector<unsigned int> foundVerts;
	unsigned int vertIndex = mEdges[edgeIndex].vert;
	findNeighbourVerts( vertIndex, foundVerts );

	int k = foundVerts.size();
	float b = beta( k );

	Vector3<float> newVertex;
	for(int i=0; i<k; i++)
		{
		newVertex += (mVerts[ foundVerts[i] ].vec * b);
		}
	
	// Get the current vertex
	Vector3<float> v = mVerts.at( vertIndex ).vec;
	newVertex += ( v * (1.0f-k*b) );

	return newVertex;
	}


/*! Computes a new vertex, placed along an edge in the old mesh
*/
Vector3<float> LoopSubdivisionMesh::computeEdgeVertex(unsigned int edgeIndex)
	{
	//connecting edge
	HalfEdge& e0 = mEdges[edgeIndex];
	HalfEdge& e1 = mEdges[e0.pair];

	//Opposite vertices' edges
	HalfEdge& e2 = mEdges[e0.prev];
	HalfEdge& e3 = mEdges[e1.prev];

	//Scale the effects of the vertices
	static const float threeByEight = 3.0f/8.0f;
	static const float oneByEight   = 1.0f/8.0f;

	Vector3<float> v_connecting0 = mVerts.at(e0.vert).vec;
	Vector3<float> v_connecting1 = mVerts.at(e1.vert).vec;

	Vector3<float> v_opposite0 = mVerts.at(e2.vert).vec;
	Vector3<float> v_opposite1 = mVerts.at(e3.vert).vec;

	//NOTE: better to do multiplication here, to prevent _weird_ rounding errors :)
	return ( (v_connecting0 +v_connecting1)*threeByEight + (v_opposite0 + v_opposite1)*oneByEight );
	}



//! Loops over the edges incident to the vertex and collects all pairs' verts
//! Almost identical to findNeighbourTris
bool LoopSubdivisionMesh::findNeighbourVerts(const unsigned int vertexIndex, std::vector<unsigned int>& foundVerts) const
	{
	foundVerts.clear();

	unsigned int edge = mVerts.at( vertexIndex ).edge;

	//go to the previous so we can always proceed in the same way in the loop

	unsigned int prev;
	unsigned int pair = edge;
	
	do
		{
		prev = mEdges[pair].prev;
		foundVerts.push_back( mEdges[prev].vert );
		pair = mEdges[prev].pair;
		} 
		while ( pair != edge );	

	return !(foundVerts.size() == 0);
	}


//! Return weights for interior verts
float LoopSubdivisionMesh::beta(unsigned int valence){
	if(valence == 6)
		{
		return 1.f / 16.f;
		} 
	else if(valence == 3)
		{
		return 3.f / 16.f;
		} 
	else
		{
		return  3.f / (8.f * valence);
		}
	}

