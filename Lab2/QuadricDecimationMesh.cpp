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
#include "QuadricDecimationMesh.h"

void QuadricDecimationMesh::initialize()
{
	// Allocate memory for the quadric array
	unsigned int numVerts = mVerts.size();
	mQuadrics.reserve(numVerts);
	std::streamsize width = std::cerr.precision(); // store stream precision
	for (unsigned int i = 0; i < numVerts; i++) {

		// Compute quadric for vertex i here
		mQuadrics.push_back(createQuadricForVert(i));

		// Calculate initial error, should be numerically close to 0
		Vector3<float> v0 = mVerts[i].vec;
		Vector4<float> v(v0.x(),v0.y(),v0.z(),1);
		Matrix4x4<float> m = mQuadrics.back();

		float error = v*(m*v);
		std::cerr << std::scientific << std::setprecision(2) << error << " ";
	}
	std::cerr << std::setprecision(width) << std::fixed; // reset stream precision

	// Run the initialize for the parent class to initialize the edge collapses
	DecimationMesh::initialize();
}

/*!
* \param[in,out] collapse The edge collapse object to (re-)compute, DecimationMesh::EdgeCollapse
*/
void QuadricDecimationMesh::computeCollapse(EdgeCollapse * collapse)
{
	// Compute collapse->position and collapse->key here
	// based on the quadrics at the edge endpoints



	// This edge collapse places the new vertex halfway along the edge.
	// The cost is computed as the vertex-to-vertex distance between the new vertex
	// and the old vertices at the edge's endpoints.

	unsigned int v1Index = mEdges[collapse->halfEdge].vert;
	unsigned int v2Index = mEdges[mEdges[collapse->halfEdge].pair].vert;

	// First, get the vertices at the endpoints
	const Vector3<float>& v1 = mVerts[ v1Index ].vec;
	const Vector3<float>& v2 = mVerts[ v2Index ].vec;

	Matrix4x4<float> Q1( mQuadrics.at(v1) );
	Matrix4x4<float> Q2 = mQuadrics.at(v2);

	Q1 += Q2;

	// "clean" last row
	Q1(3, 0) = Q1(3, 1) = Q1(3, 2) = 0.0f;
	Q1(3, 3) = 1.0f;

	Matrix4x4<float> invQ = Q1.inverse();
	Vector4<float> v(0.0f, 0.0f, 0.0f, 1.0f);

	/************************************************************************/
	/* OPTIMIZE USING JUST THE LAST COLUMN ?????!!!                         */
	/************************************************************************/
	v = invQ * v;

	// Then, compute the new position and the cost

	collapse->position = Vector3( v );
	//collapse->cost = (collapse->position - v0).length();




	std::cerr << "computeCollapse in QuadricDecimationMesh not implemented.\n";
}

/*! After each edge collapse the vertex properties need need to be updated */
void QuadricDecimationMesh::updateVertexProperties(unsigned int ind)
{
	DecimationMesh::updateVertexProperties(ind);
	mQuadrics[ind] = createQuadricForVert(ind);
}

/*!
* \param[in] indx vertex index, points into HalfEdgeMesh::mVerts
*/
Matrix4x4<float> QuadricDecimationMesh::createQuadricForVert(unsigned int indx) const{
	float q[4][4] = {{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0},
	{0,0,0,0}};
	Matrix4x4<float> Q(q);

	// get the neighboring triangles
	std::vector<unsigned int> foundTriangles;
	this->findNeighbourTriangles(indx, foundTriangles);

	std::vector<unsigned int>::iterator faces = foundTriangles.begin();
	std::vector<unsigned int>::iterator facesEnd = foundTriangles.end();
	for (; faces != facesEnd; faces++)
	{
		// get the first half-edge of the current face
		unsigned int curFaceIndex = *faces;
		Face f = mFaces[curFaceIndex];
		unsigned int edge = f.edge;
		
		// get the first vertex: any one is good
		unsigned int vertexIndex = mEdges[edge].vert;
		Vector3<float> v1 = mVerts[vertexIndex].vec;

		Vector3<float> normal = f.normal;

		// The plane cartesian equation
		// d = -(ax + by + cz), where N=(a, b, c) and V=(x,y,z)
		float a = normal.x();
		float b = normal.y();
		float c = normal.z();
		float d = -(v1 * normal);

		// just add the upper diagonal matrix: we don't need to sum the other
		// ones for now
		Q(0, 0) += a*a;
		Q(0, 1) += a*b;
		Q(0, 2) += a*c;
		Q(0, 3) += a*d;
		Q(1, 1) += b*b;
		Q(1, 2) += b*c;
		Q(1, 3) += b*d;
		Q(2, 2) += c*c;
		Q(2, 3) += c*d;
		Q(3, 3) += d*d;

	}

	// the remaining items can now be assigned
	Q(1, 0) = Q(0, 1);
	Q(2, 0) = Q(0, 2);
	Q(3, 0) = Q(0, 3);
	Q(2, 1) = Q(1, 2);
	Q(3, 1) = Q(1, 3);
	Q(3, 2) = Q(2, 3);

	// The quadric for a vertex is the sum of all the quadrics for the adjacent faces
	// Tip: Matrix4x4 has an operator +=
	return Q;
}

/*!
* \param[in] indx face index, points into HalfEdgeMesh::mFaces
*/
Matrix4x4<float> QuadricDecimationMesh::createQuadricForFace(unsigned int indx) const{

	// Calculate the quadric for a face here using the formula from Garland and Heckbert
	return Matrix4x4<float>();
}

void QuadricDecimationMesh::draw()
{
	HalfEdgeMesh::draw();
	drawQuadrics();
}


void QuadricDecimationMesh::drawQuadrics()
{
	if (mQuadratic == NULL){
		mQuadratic = gluNewQuadric();// Create A Pointer To The Quadric Object
		gluQuadricNormals(mQuadratic, GLU_SMOOTH);// Create Smooth Normals
		gluQuadricTexture(mQuadratic, GL_TRUE);// Create Texture Coords
	}
	else{
		// Draw the quadrics here by applying the appropriate transform to a sphere.
		// The quadrics are stored in the mQuadrics array.
		// The sphere can be drawn by calling 'gluSphere(mQuadratic,radius, numSlices , numStacks)';
		// Example: gluSphere(mQuadratic,1.0, 10 , 10)
	}

}