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
#include "HalfEdgeMesh.h"
#include "ColorMap.h"
#ifdef __APPLE__
#include "GLUT/glut.h"
#else
#include "GL/glut.h"
#endif
#include <limits>
#include <queue>


//const unsigned int HalfEdgeMesh::BORDER = std::numeric_limits<unsigned int>::max();
//const unsigned int HalfEdgeMesh::UNINITIALIZED = std::numeric_limits<unsigned int>::max()-1;

const unsigned int HalfEdgeMesh::BORDER = UINT_MAX;
const unsigned int HalfEdgeMesh::UNINITIALIZED = UINT_MAX-1;

HalfEdgeMesh::HalfEdgeMesh()
: mVertSize(0)
, mFaceSize(0)
, mEdgeSize(0)
	{
	}

HalfEdgeMesh::~HalfEdgeMesh()
	{
	}



//-----------------------------------------------------------------------------
bool HalfEdgeMesh::addTriangle(const Vector3<float> &v1, const Vector3<float> &v2, const Vector3<float> & v3){

	// Add the vertices of the face/triangle
	unsigned int ind1, ind2, ind3;
	addVertex(v1, ind1);
	addVertex(v2, ind2);
	addVertex(v3, ind3);

	// Add all half-edge pairs
	unsigned int edgeind1, edgeind2, edgeind3;
	unsigned int edgeind1pair, edgeind2pair, edgeind3pair;

	addHalfEdgePair( ind1, ind2, edgeind1, edgeind1pair );
	addHalfEdgePair( ind2, ind3, edgeind2, edgeind2pair );
	addHalfEdgePair( ind3, ind1, edgeind3, edgeind3pair );

	// Connect inner ring
	mEdges[edgeind1].next = edgeind2;
	mEdges[edgeind1 ].prev = edgeind3;

	mEdges[ edgeind2 ].next = edgeind3;
	mEdges[ edgeind2 ].prev = edgeind1;

	mEdges[edgeind3 ].next = edgeind1;
	mEdges[ edgeind3 ].prev = edgeind2;


	// Finally, create the face
	Face f;
	f.edge = edgeind1;
	mFaces.push_back( f );

	// All half-edges share the same left face (previously added)
	int index = mFaceSize++;

	mEdges[edgeind1 ].face = index;
	mEdges[ edgeind2 ].face = index;
	mEdges[ edgeind3 ].face = index;

	return true;
	}



//-----------------------------------------------------------------------------
bool HalfEdgeMesh::addVertex(const Vector3<float> & v, unsigned int &indx)
	{
	//	THashTable::iterator it = hash.find( x );
	//	std::map<Vector3<float>,unsigned int>::iterator it = mUniqueVerts.find(v);
	VertexHashTable::iterator it = mUniqueVerts.find( v );

	if (it != mUniqueVerts.end()){
		indx = (*it).second; // (*it).second == it.second, which to me was not as clear...
		return false;
		}

	mUniqueVerts[v] = indx = mVertSize; // op. [ ] constructs a new entry in map
	mVertSize++;

	Vertex vert;
	vert.vec = v;
	mVerts.push_back(vert);

	return true;
	}


bool HalfEdgeMesh::addHalfEdgePair(unsigned int v1, unsigned int v2, unsigned int &indx1, unsigned int &indx2)
	{
	//// Search for the HalfEdge pair among existing edges
	//std::vector<HalfEdge>::reverse_iterator iter = mEdges.rbegin();
	//std::vector<HalfEdge>::reverse_iterator iend = mEdges.rend();
	//while (iter != iend) 
	//	{
	//	if ((*iter).vert == v1 && mEdges[(*iter).pair].vert == v2) 
	//		{
	//		indx1 = iter.base() - mEdges.begin() - 1;
	//		indx2 = (*iter).pair;
	//		return false;
	//		}
	//	iter++;
	//	}

	std::pair<unsigned int, unsigned int> key = std::make_pair( std::min(v1,v2), std::max(v1,v2) );
	EdgeHashTable::iterator it = mUniqueEdges.find( key );

	//does not exist yet
	if ( mUniqueEdges.end() == it)
		{
		// If not found, add both half-edges
		indx1 = mEdgeSize++;
		indx2 = mEdgeSize++;

		// Create edges and set pair index
		HalfEdge edge1, edge2;
		edge1.pair = indx2;
		edge2.pair = indx1;

		// Connect the edges to the verts
		edge1.vert = v1;
		edge2.vert = v2;

		// Connect the verts to the edges
		mVerts[v1].edge = indx1;
		mVerts[v2].edge = indx2;

		// Store the edges
		mEdges.push_back(edge1);
		mEdges.push_back(edge2);

		mUniqueEdges[key] = mVerts[ std::min(v1,v2)].edge;

		return true;
		}
	//edge already created
	else
		{
		indx1 = (v1<v2)? it->second : mEdges[ it->second].pair;
		indx2 = (v1<v2)? mEdges[ it->second ].pair : it->second;
		return false;
		}
	}


void HalfEdgeMesh::mergeBoundaryEdge(unsigned int indx)
	{

	}


void HalfEdgeMesh::validate()
	{
	std::vector<HalfEdge>::iterator iterEdge = mEdges.begin();
	std::vector<HalfEdge>::iterator iterEdgeEnd = mEdges.end();
	while (iterEdge != iterEdgeEnd) {
		if ((*iterEdge).face == UNINITIALIZED ||
			(*iterEdge).next == UNINITIALIZED ||
			(*iterEdge).pair == UNINITIALIZED ||
			(*iterEdge).prev == UNINITIALIZED ||
			(*iterEdge).vert == UNINITIALIZED)
			std::cerr << "HalfEdge " << iterEdge - mEdges.begin() << " not properly initialized" << std::endl;

		iterEdge++;
		}
	std::cerr << "Done with edge check (checked " << mEdgeSize << " edges)" << std::endl;

	std::vector<Face>::iterator iterFace = mFaces.begin();
	std::vector<Face>::iterator iterFaceEnd = mFaces.end();
	while (iterFace != iterFaceEnd) {
		if ((*iterFace).edge == UNINITIALIZED)
			std::cerr << "Face " << iterFace - mFaces.begin() << " not properly initialized" << std::endl;

		iterFace++;
		}
	std::cerr << "Done with face check (checked " << mFaceSize << " faces)" << std::endl;

	std::vector<Vertex>::iterator iterVertex = mVerts.begin();
	std::vector<Vertex>::iterator iterVertexEnd = mVerts.end();
	while (iterVertex != iterVertexEnd) {
		if ((*iterVertex).edge == UNINITIALIZED)
			std::cerr << "Vertex " << iterVertex - mVerts.begin() << " not properly initialized" << std::endl;

		iterVertex++;
		}
	std::cerr << "Done with vertex check (checked " << mVertSize << " vertices)" << std::endl;

	std::cerr << "Looping through triangle neighborhood of each vertex... ";
	iterVertex = mVerts.begin();
	iterVertexEnd = mVerts.end();
	std::vector<unsigned int> foundTriangles;
	int emptyCount = 0;
	while (iterVertex != iterVertexEnd) {
		if (!findNeighbourTriangles(iterVertex - mVerts.begin(), foundTriangles))
			emptyCount++;

		iterVertex++;
		}
	std::cerr << std::endl << "Done: " << emptyCount << " isolated vertices found" << std::endl;


	}


//-----------------------------------------------------------------------------
bool HalfEdgeMesh::findNeighbourTriangles(const unsigned int vertexIndex, std::vector<unsigned int>& foundTriangles) const
	{
	foundTriangles.clear();

	unsigned int edge = mVerts.at( vertexIndex ).edge;

	//go to the previous so we can always proceed in the same way in the loop
	int pair = mEdges[edge].prev;
	edge = pair;

	int next(0);

	//find the neighbours
	do
		{
		foundTriangles.push_back( mEdges.at( pair ).face );

		next = mEdges[pair].next;
		pair = mEdges[next].pair;

		} while( pair != edge );


		return !(foundTriangles.size() == 0);			
	}

float HalfEdgeMesh::area()
	{
	float totalArea(0);
	unsigned int numTriangles = mFaceSize;
	for (unsigned int i = 0; i < numTriangles; i++)
		{
		totalArea += calculateFaceNormal( i ).length() / 2.0f;
		}		
	return totalArea;
	}

float HalfEdgeMesh::volume()
	{
	float totalVolume(0);

	unsigned int numTriangles = mFaceSize;
	for (unsigned int i = 0; i < numTriangles; i++)
		{
		Vector3<float> normal = calculateFaceNormal( i );
		float length = normal.length();
		normal.normalize();
		int edgeIndex1 = mFaces.at(i).edge;
		int edgeIndex2 = mEdges.at( edgeIndex1 ).next;
		int edgeIndex3 = mEdges.at( edgeIndex2 ).next;

		Vertex v1 = mVerts.at( mEdges.at( edgeIndex1 ).vert );
		Vertex v2 = mVerts.at( mEdges.at( edgeIndex2 ).vert );
		Vertex v3 = mVerts.at( mEdges.at( edgeIndex3 ).vert );

		float area = length / 2.0f;
		totalVolume += (( v1.vec + v2.vec + v3.vec ) / 3.0f)*normal * area;
		}

	return totalVolume/3.0f;
	}
int HalfEdgeMesh::genus() const
	{
	printf("Genus calculation not implemented for half-edge mesh!\n");
	return 0;
	}

int HalfEdgeMesh::shells() const 
	{
	std::queue<Vertex> vertexQueue;


	return 0;
	}

float HalfEdgeMesh::curvature(const unsigned int vertexIndex, const Vector3<float>& n)
	{
	printf("Curvature calculation not implemented for half-edge mesh!\n");
	return 0;
	}

void HalfEdgeMesh::calculateFaceNormals()
	{
	unsigned int numTriangles = mFaceSize;
	for (unsigned int i = 0; i < numTriangles; i++){

		HalfEdge* edge = &mEdges[mFaces[i].edge];

		Vector3<float>& p0 = mVerts[edge->vert].vec;
		edge = &mEdges[edge->next];

		Vector3<float>& p1 = mVerts[edge->vert].vec;
		edge = &mEdges[edge->next];

		Vector3<float>& p2 = mVerts[edge->vert].vec;

		// Calculate face normal
		Vector3<float> v1 = p1-p0;
		Vector3<float> v2 = p2-p0;
		Vector3<float> n = cross(v1,v2);
		n.normalize();
		mFaces[i].normal = n; // assign normal to face
		}	
	}

void HalfEdgeMesh::calculateVertexNormals()
	{
	std::vector<unsigned int> foundTriangles;

	for (int i=0, endI=mVertSize; i<endI; i++)
		{
		findNeighbourTriangles(i, foundTriangles );

		Vector3<float> normal;
		int endJ=foundTriangles.size();
		for (int j=0; j<endJ; j++)
			{
			normal += calculateFaceNormal( foundTriangles[j] ).normalize();
			}
		normal = normal/endJ;

		//mNormals.push_back( normal );
		mVerts[i].normal = normal;
		}
	}


Vector3<float> HalfEdgeMesh::calculateFaceNormal( unsigned int aTriangle )
	{
	HalfEdge* edge = &mEdges[mFaces[aTriangle].edge];

	Vector3<float>& p0 = mVerts[edge->vert].vec;
	edge = &mEdges[edge->next];

	Vector3<float>& p1 = mVerts[edge->vert].vec;
	edge = &mEdges[edge->next];

	Vector3<float>& p2 = mVerts[edge->vert].vec;

	// Calculate face normal
	Vector3<float> v1 = p1-p0;
	Vector3<float> v2 = p2-p0;
	Vector3<float> n = cross(v1,v2);
	return n;
	}

//-----------------------------------------------------------------------------
void HalfEdgeMesh::draw() 
	{
	ColorMap colorMap;

	glMatrixMode(GL_MODELVIEW);

	// Apply transform	
	glPushMatrix(); // Push modelview matrix onto stack

	// Convert transform-matrix to format matching GL matrix format
	GLfloat glMatrix[16];
	mTransform.toGLMatrix(glMatrix); 

	// Load transform into modelview matrix 	
	glLoadMatrixf(glMatrix);

	// Draw geometry	
	glBegin(GL_TRIANGLES);
	const int numTriangles = mFaceSize;
	for (int i = 0; i < numTriangles; i++){

		HalfEdge* edge = &mEdges[mFaces[i].edge];

		Vector3<float>& p0 = mVerts[edge->vert].vec;
		edge = &mEdges[edge->next];

		Vector3<float>& p1 = mVerts[edge->vert].vec;
		edge = &mEdges[edge->next];

		Vector3<float>& p2 = mVerts[edge->vert].vec;

		if (getShadingFlag() == FLAT_SHADING){
			Vector3<float>& n = mFaces[i].normal;
			Vector3<float> color = colorMap.map(n, -1, 1);

			glColor3f(color[0],color[1], color[2]); 			
			glNormal3f(n[0], n[1], n[2]);
			glVertex3f(p0[0], p0[1], p0[2]);
			glVertex3f(p1[0], p1[1], p1[2]);
			glVertex3f(p2[0], p2[1], p2[2]);
			}
		else if (getShadingFlag() == SMOOTH_SHADING){

			HalfEdge* edge = &mEdges[mFaces[i].edge];

			unsigned int v1 = edge->vert;
			edge = &mEdges[edge->next];

			unsigned int v2 = edge->vert;
			edge = &mEdges[edge->next];

			unsigned int v3 = edge->vert;

			// Fetching normals - the normal index is the same as the vertex index
			Vector3<float>& n0 = mVerts[v1].normal;
			Vector3<float>& n1 = mVerts[v2].normal;
			Vector3<float>& n2 = mVerts[v3].normal;

			// Color mapping, maps the normal components to R,G,B. From [-1, 1] to [0,1] respectively
			Vector3<float> color0 = colorMap.map(n0, -1, 1);
			Vector3<float> color1 = colorMap.map(n1, -1, 1);
			Vector3<float> color2 = colorMap.map(n2, -1, 1);

			glColor3f(color0[0],color0[1], color0[2]); 			
			glNormal3f(n0[0], n0[1], n0[2]);
			glVertex3f(p0[0], p0[1], p0[2]);

			glColor3f(color1[0],color1[1], color1[2]); 				 		
			glNormal3f(n1[0], n1[1], n1[2]);
			glVertex3f(p1[0], p1[1], p1[2]);

			glColor3f(color2[0],color2[1], color2[2]); 				 		
			glNormal3f(n2[0], n2[1], n2[2]);
			glVertex3f(p2[0], p2[1], p2[2]);					
			}
		else{
			// No normals
			glVertex3f(p0[0], p0[1], p0[2]);
			glVertex3f(p1[0], p1[1], p1[2]);
			glVertex3f(p2[0], p2[1], p2[2]);			
			}		

		}  
	glEnd();

	// Restore modelview matrix
	glPopMatrix();
	}
