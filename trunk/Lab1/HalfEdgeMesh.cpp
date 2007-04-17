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
#include <set>
#include <vector>

const unsigned int HalfEdgeMesh::BORDER = std::numeric_limits<unsigned int>::max();
const unsigned int HalfEdgeMesh::UNINITIALIZED = std::numeric_limits<unsigned int>::max()-1;

bool sortEdges( std::pair<unsigned int, bool> a, std::pair<unsigned int, bool> b)
	{
	return a.second>b.second;
	}

HalfEdgeMesh::HalfEdgeMesh()
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

	bool newEdge1 = addHalfEdgePair( ind1, ind2, edgeind1, edgeind1pair );
	bool newEdge2 = addHalfEdgePair( ind2, ind3, edgeind2, edgeind2pair );
	bool newEdge3 = addHalfEdgePair( ind3, ind1, edgeind3, edgeind3pair );

	std::vector<std::pair<unsigned int, bool> > edges;
	edges.push_back( std::make_pair( edgeind1, newEdge1) );
	edges.push_back( std::make_pair( edgeind2, newEdge2) );
	edges.push_back( std::make_pair( edgeind3, newEdge3) );

	// Connect inner ring
 	mEdges.at( edgeind1 ).next = edgeind2;
	mEdges.at( edgeind1 ).prev = edgeind3;

	mEdges.at( edgeind2 ).next = edgeind3;
	mEdges.at( edgeind2 ).prev = edgeind1;

	mEdges.at( edgeind3 ).next = edgeind1;
	mEdges.at( edgeind3 ).prev = edgeind2;

	// Finally, create the face
	Face f;
	f.edge = edgeind1;
	mFaces.push_back( f );

	// All half-edges share the same left face (previously added)
	int index = mFaces.size()-1;

	mEdges.at( edgeind1 ).face = index;
	mEdges.at( edgeind2 ).face = index;
	mEdges.at( edgeind3 ).face = index;

	sort( edges.begin(), edges.end(), sortEdges );

	for(int i=0,endI=edges.size(); i<endI; i++)
		{
		//BORDER EDGE
		if ( edges.at(i).second )
			{
			//pass the inner edge
			insertBoundaryEdge( edges.at(i).first );
			}
		//CONNECT TO ANOTHER EDGE
		else
			{
			//pass the pair
			mergeBoundaryEdge( edges.at(i).first );
			}
		}

  return true;
}



//-----------------------------------------------------------------------------
bool HalfEdgeMesh::addVertex(const Vector3<float> & v, unsigned int &indx){
  std::map<Vector3<float>,unsigned int>::iterator it = mUniqueVerts.find(v);
  if (it != mUniqueVerts.end()){
		indx = (*it).second; // (*it).second == it.second, which to me was not as clear...
    return false;
  }

  mUniqueVerts[v] = indx = mVerts.size(); // op. [ ] constructs a new entry in map
  Vertex vert;
  vert.vec = v;
  mVerts.push_back(vert);
    
  return true;
}


bool HalfEdgeMesh::addHalfEdgePair(unsigned int v1, unsigned int v2, unsigned int &indx1, unsigned int &indx2)
{
  // Search for the HalfEdge pair among existing edges
  std::vector<HalfEdge>::reverse_iterator iter = mEdges.rbegin();
  std::vector<HalfEdge>::reverse_iterator iend = mEdges.rend();
  while (iter != iend) {
    if ((*iter).vert == v1 && mEdges[(*iter).pair].vert == v2) {
      indx1 = iter.base() - mEdges.begin() - 1;
      indx2 = (*iter).pair;
      return false;
    }
    iter++;
  }

  // If not found, add both half-edges
  indx1 = mEdges.size();
  indx2 = indx1+1;

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

  return true;
}


void HalfEdgeMesh::mergeBoundaryEdge(unsigned int indx)
	{
	//Find the previous border
	int prev(0);
	int prevPair = mEdges[indx].pair;
	while( BORDER != mEdges[prevPair].face )
		{
		prev = mEdges[prevPair].prev;
		prevPair = mEdges[prev].pair;

		//make sure we don't loop infinitely
		if( mEdges[indx].face == mEdges[prevPair].face )
			{
			return;
			}
		}

	//Find the next border
	int next(0);
	int nextPair = indx;
	while( BORDER != mEdges[nextPair].face && ( UNINITIALIZED != mEdges[nextPair].face ) )
		{
		next = mEdges[nextPair].next;
		nextPair = mEdges[next].pair;

		//make sure we don't loop infinitely
		if( mEdges[indx].face == mEdges[nextPair].face )
			{
			return;
			}
		}

	//link borders
	mEdges[prevPair].prev = nextPair;
	mEdges[nextPair].next = prevPair;

	}

void HalfEdgeMesh::insertBoundaryEdge( unsigned int indx )
	{
	//Set the face of the edge's pair to border
	int pair =	mEdges[indx].pair;
	mEdges[pair].face = BORDER;

	//Find the next border
	int prev(0);
	int next = indx;
	while( ( BORDER != mEdges[next].face  ) && ( UNINITIALIZED != mEdges[next].face  ) )
		{
		prev = mEdges[next].prev;
		next = mEdges[prev].pair;
		}
	//link PAIR of the current with the next
	mEdges[pair].next = next;
	mEdges[next].prev = pair;



	//Find the previous border
	prev = indx;
	while( ( BORDER != mEdges[prev].face ) && ( UNINITIALIZED != mEdges[prev].face ) )
		{
		next = mEdges[prev].next;
		prev = mEdges[next].pair;
		}
	//link PAIR of the current with the previous
	mEdges[pair].prev = prev;
	mEdges[prev].next = pair;
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
  std::cerr << "Done with edge check (checked " << mEdges.size() << " edges)" << std::endl;

  std::vector<Face>::iterator iterFace = mFaces.begin();
  std::vector<Face>::iterator iterFaceEnd = mFaces.end();
  while (iterFace != iterFaceEnd) {
    if ((*iterFace).edge == UNINITIALIZED)
        std::cerr << "Face " << iterFace - mFaces.begin() << " not properly initialized" << std::endl;

    iterFace++;
  }
  std::cerr << "Done with face check (checked " << mFaces.size() << " faces)" << std::endl;

  std::vector<Vertex>::iterator iterVertex = mVerts.begin();
  std::vector<Vertex>::iterator iterVertexEnd = mVerts.end();
  while (iterVertex != iterVertexEnd) {
    if ((*iterVertex).edge == UNINITIALIZED)
        std::cerr << "Vertex " << iterVertex - mVerts.begin() << " not properly initialized" << std::endl;

    iterVertex++;
  }
  std::cerr << "Done with vertex check (checked " << mVerts.size() << " vertices)" << std::endl;

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
	unsigned int numTriangles = mFaces.size();
	for (unsigned int i = 0; i < numTriangles; i++)
		{
		totalArea += calculateFaceNormal( i ).length() / 2.0f;
		}		
	return totalArea;
	}

float HalfEdgeMesh::volume()
	{
	float totalVolume(0);

	unsigned int numTriangles = mFaces.size();
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
	int E = mEdges.size() / 2; //half edge is just a half edge
	int V = mVerts.size();
	int F = mFaces.size();
	int S = shells();

	std::cerr << "Shells: " << S << std::endl;

	return (E-V-F)/2 + S;
	}

int HalfEdgeMesh::shells() const 
	{

	int numberOfShells = 0;
	//	std::queue<int> vertexQueue;
	std::set<int> vertexQueue;
	std::set<int> vertexSet;
	//vertexQueue.push( 0 );
	vertexQueue.insert(0);


	std::cout << "Vertices: " << mVerts.size() << std::endl;

	int v;
	std::set<int>::iterator position;

	int totalVerts = mVerts.size();

	//Browse vertices until all are inserted to the set
	while (vertexSet.size() != totalVerts)
		{
		//OPTIMIZE!
		//Search one vertex which is still NOT in the set
		for (int vertIndex = 0; vertIndex<totalVerts; vertIndex++ )
			{
			if( vertexSet.end() == vertexSet.find( vertIndex ))
				{
				vertexQueue.insert(vertIndex);
				break;
				}
			}
		//fill the current set (shell)
		while(!vertexQueue.empty())
			{
			//		v = vertexQueue.front();
			position = vertexQueue.begin();
			v = *position;
			//		vertexQueue.pop();
			vertexQueue.erase( position );
			vertexSet.insert( v );

			//std::cout << "Value: " << v << std::endl;
			//std::cout << "vque: " << vertexQueue.size() << std::endl;
			//std::cout << "vertset: " << vertexSet.size() << std::endl;

			std::vector<unsigned int> faces;
			findNeighbourTriangles(v, faces);
			for (int i=0,endI=faces.size();i<endI;i++)
				{
				std::vector<unsigned int> verts;
				getTriangleVerts( faces.at(i), verts );

				for (int j=0, endJ=verts.size(); j<endJ; j++)
					{
					int vj = verts.at(j);
					if ( vertexSet.end() != vertexSet.find(vj) )
						{
						//Do nothing
						}
					else
						{
						//vertexQueue.push(vj);
						vertexQueue.insert(vj);
						}
					}
				}
			}
		numberOfShells++;
		}

	return numberOfShells;
	}

void HalfEdgeMesh::getTriangleVerts(int aTriangle, std::vector<unsigned int>& aVerts ) const
	{
	//mFaces.at( aTriangle ).ha
	int edge1 = mFaces.at( aTriangle).edge;
	aVerts.push_back( mEdges.at(edge1).vert );

	int edge2 = mEdges.at(edge1).next;
	aVerts.push_back( mEdges.at(edge2).vert );

	int edge3 = mEdges.at(edge2).next;
	aVerts.push_back( mEdges.at(edge3).vert );
	}


float HalfEdgeMesh::curvature(const unsigned int vertexIndex, const Vector3<float>& n)
{
  printf("Curvature calculation not implemented for half-edge mesh!\n");
  return 0;
}

void HalfEdgeMesh::calculateFaceNormals()
{
  std::cerr << "calculateFaceNormals() in HalfEdgeMesh.cpp not implemented" << std::endl;

  unsigned int numTriangles = mFaces.size();
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

	for (int i=0, endI=mVerts.size(); i<endI; i++)
		{
		findNeighbourTriangles(i, foundTriangles );

		Vector3<float> normal;
		int endJ=foundTriangles.size();
		for (int j=0; j<endJ; j++)
			{
			normal += calculateFaceNormal( foundTriangles[j] ).normalize();
			}
		normal = normal/endJ;

		mNormals.push_back( normal );
		}
	}


Vector3<float> HalfEdgeMesh::calculateFaceNormal( unsigned int aTriangle )
	{
	//For non manifold surfaces
	if( BORDER == aTriangle )
		{
		return Vector3<float>(0.0f,0.0f,0.0f);
		}

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
  const int numTriangles = mFaces.size();
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
			Vector3<float>& n0 = mNormals[v1];
			Vector3<float>& n1 = mNormals[v2];
			Vector3<float>& n2 = mNormals[v3];
			
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
