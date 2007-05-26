/*************************************************************************************************
*
* Modeling and animation (TNM079) 2007
* Code base for lab assignments. Copyright:
*   Gunnar Johansson (gunnar.johansson@itn.liu.se)
*   Ken Museth (ken.museth@itn.liu.se)
*   Michael Bang Nielsen (bang@daimi.au.dk)
*   Ola Nilsson (ola.nilsson@itn.liu.se)
*   Andreas Sderstrm (andreas.soderstrom@itn.liu.se)
*
*************************************************************************************************/
#ifndef __HALF_EDGE_H__
#define __HALF_EDGE_H__

#include "Mesh.h"
#include <map>
#include "Hashtable.h"

class HalfEdgeMesh : public Mesh {
public:
	HalfEdgeMesh();
	~HalfEdgeMesh();

	//! Adds a triangle to the mesh. \sa addTriangle
	virtual bool addTriangle(const Vector3<float> &v1, const Vector3<float> &v2, const Vector3<float> & v3);


	virtual float area();

	virtual float volume();

	virtual int genus() const;
	virtual int shells() const;

	virtual float curvature(const unsigned int vertexIndex, const Vector3<float>& n);

	virtual void calculateFaceNormals();

	virtual void calculateVertexNormals();

	Vector3<float> calculateFaceNormal( unsigned int aTriangle );

	virtual void draw();

	void validate();

protected:

	//! References a border, only for face pointers
	const static unsigned int BORDER;
	//! Denotes a reference to a non-existing object
	const static unsigned int UNINITIALIZED;

	struct HalfEdge {
		HalfEdge() : vert(UNINITIALIZED), face(UNINITIALIZED), next(UNINITIALIZED), prev(UNINITIALIZED), pair(UNINITIALIZED) { }
		unsigned int vert;  // index into mVerts
		unsigned int face;   // index into mFaces
		unsigned int next;  // index into mEdges
		unsigned int prev;  // index into mEdges
		unsigned int pair;  // index into mEdges
		};

	struct Vertex {
		Vertex() : edge(UNINITIALIZED) { }
		Vector3<float> vec;  // the vertex coordinates
		unsigned int edge; // index into mEdges
		Vector3<float> normal;
		};

	struct Face {
		Face() : edge(UNINITIALIZED) { }
		unsigned int edge; // index into mEdges
		Vector3<float> normal; // the face normal
		};

	int mEdgeSize;
	std::vector<HalfEdge> mEdges;
	int mVertSize;
	std::vector<Vertex> mVerts;
	int mFaceSize;
	std::vector<Face> mFaces;


	//std::vector<Vector3<float> > mNormals;
	std::vector<float> mCurvature;

	//  std::map<Vector3<float>, unsigned int> mUniqueVerts;
	VertexHashTable mUniqueVerts;
	EdgeHashTable mUniqueEdges;

	//! Adds a vertex to the mesh
	virtual bool addVertex(const Vector3<float>& v, unsigned int &indx);

	bool addHalfEdgePair(unsigned int v1, unsigned int v2, unsigned int &indx1, unsigned int &indx2);

	void mergeBoundaryEdge(unsigned int indx);


	// Given a vertex, find all triangles that includes this vertex
	virtual bool findNeighbourTriangles(const unsigned int vertexIndex, std::vector<unsigned int>& foundTriangles) const;

	};

#endif
