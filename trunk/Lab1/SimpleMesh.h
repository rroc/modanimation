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
#ifndef __simplemesh_h__
#define __simplemesh_h__

#ifdef __APPLE__
#include "GLUT/glut.h"
#else
#include "GL/glut.h"
#endif
#include "Mesh.h"

#include <set>
#include <map>
#include <cassert>
#include <limits>
#include <algorithm>
#include <cmath>
#include <utility>

#include "Vector3.h"

struct MyEdge{
	MyEdge(unsigned int a, unsigned int b) : a(min(a, b)), b(max(a,b)) {}
	bool operator < (const MyEdge &eb) const {
		if (a<eb.a)
			return true;
		else if(a==eb.a)
			if(b<eb.b)
				return true;
		return false;
	}
	unsigned int a, b;
};


class SimpleMesh : public Mesh {

protected:

	struct Face {
		unsigned int v1, v2, v3;
	};

	std::vector<Vector3<float> > mVerts;
	std::vector<Face> mFaces;
	std::vector<Vector3<float> > mNormals;

	std::map<Vector3<float>, unsigned int> mUniqueVerts;

	//! Adds a vertex to the mesh
	virtual bool addVertex(const Vector3<float>& v, unsigned int &indx);

	// Given a vertex, find all triangles that includes this vertex
	virtual bool findNeighbourTriangles(const unsigned int vertexIndex, std::vector<unsigned int>& foundTriangles) const;

public: 
	SimpleMesh();
	~SimpleMesh();

	//! Adds a triangle to the mesh. \sa addTriangle
	virtual bool addTriangle(const Vector3<float> &v1, const Vector3<float> &v2, const Vector3<float> & v3);

	//! Access to internal vertex data
	const std::vector<Vector3<float> >& getVerts() const { return mVerts; }
	const std::vector<Vector3<float> >& getNormals() const { return mNormals; }
	const std::vector<Face>& getTris() const { return mFaces; }

	// Methods for calculating normals
	virtual void calculateFaceNormals();
	virtual void calculateVertexNormals();  

	// Draw call
	virtual void draw();


	// Curvature calculation test
	float mMinCurv;
	float mMaxCurv;
	std::vector<float> curvatureArray;
	virtual float curvature(const unsigned int vertexIndex, const Vector3<float>& n);
	void calcCurv();
};


#endif
