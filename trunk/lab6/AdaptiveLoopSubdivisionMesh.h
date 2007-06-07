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
#ifndef __ADAPTIVE_LOOP_SUBDIVISION_MESH_H__
#define __ADAPTIVE_LOOP_SUBDIVISION_MESH_H__

#include "LoopSubdivisionMesh.h"
#include "Util.h"
#include <limits>

/*! \brief Adaptive Subdivision mesh that is based on the Loop scheme
*/
class AdaptiveLoopSubdivisionMesh : public LoopSubdivisionMesh
{
public:
	AdaptiveLoopSubdivisionMesh(const HalfEdgeMesh & m, unsigned int s);
	AdaptiveLoopSubdivisionMesh();
	~AdaptiveLoopSubdivisionMesh();

	//! Subdivides the mesh uniformly one step
	virtual bool subdivide();

	//! Subdivides the mesh adaptivly one step. The angle is given in degrees, not radians
	virtual bool subdivide(float flatAngle);

private:
	//! Method that ensures that boundaries between subdivided and not subdivided triangles match
	void treatFlatFaces(HalfEdgeMesh& subDivMesh);

	//! Classify triangles as flat or not flat
	void findFlatTriangles(float flatAngle);

	//! Calculate the number of flat neighbours to each triangle. Flatness 3 is assigned to all flat triangles.
	void calculateDegreeOfFlatness();

	//! Finds the face index of the three faces neighbour to "face".
	void findTriangleNeighbours(const Face& face, unsigned int& neighbourIndex1, unsigned int& neighbourIndex2, unsigned int& neighbourIndex3);

	//! Finds which edge belonging to face 1 that is shared with face 2
	//! Returns the array index of that edge
	unsigned int findSharedEdge(const Face& face1, const Face& face2);

	//! Returns true if the vertex with index "vertexIndex" is shared by a triangle that has been subdivided
	bool isSharedBySubdividedTriangle(unsigned int vertexIndex);

	std::vector<bool> mFaceIsFlat;
	std::vector<unsigned int> mFlatness;
};

#endif
