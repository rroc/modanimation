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
#include "DecimationMesh.h"
#include "ColorMap.h"
#include "GUI.h"
#include <cassert>

#include "Heap.h"

void DecimationMesh::initialize()
	{
	//// Allocate memory for the 'collapsed flags'
	//mCollapsedVerts.reserve(mVerts.size());
	//mCollapsedEdges.reserve(mEdges.size());
	//mCollapsedFaces.reserve(mFaces.size());

	//// Set all flags to false
	//mCollapsedVerts.assign(mVerts.size(), false);
	//mCollapsedEdges.assign(mEdges.size(), false);
	//mCollapsedFaces.assign(mFaces.size(), false);

	mCollapsedVerts.resize(mVerts.size(), UNCOLLAPSED );
	mNumCollapsedVerts = 0;
	mCollapsedEdges.resize(mEdges.size(), UNCOLLAPSED );
	mNumCollapsedEdges = 0;
	mCollapsedFaces.resize(mFaces.size(), UNCOLLAPSED );
	mNumCollapsedFaces = 0;

	// Allocate memory for the references from half-edge
	// to edge collapses
	mHalfEdge2EdgeCollapse.reserve(mEdges.size());
	mHalfEdge2EdgeCollapse.assign(mEdges.size(), NULL);

	// Loop through the half-edges (we know they are stored
	// sequentially) and create an edge collapse operation
	// for each pair
	unsigned int numCollapses = mEdges.size()/2;
	for (unsigned int i = 0; i < numCollapses; i++) {
		EdgeCollapse * collapse = new EdgeCollapse();

		// Connect the edge collapse with the half-edge pair
		collapse->halfEdge = i*2;

		// Check if the collapse is valid
		if (!isValidCollapse(collapse))
			delete collapse;

		else {
			mHalfEdge2EdgeCollapse[i*2] = collapse;
			mHalfEdge2EdgeCollapse[i*2+1] = collapse;

			// Compute the cost and push it to the heap
			computeCollapse(collapse);
			mHeap.push(collapse);
			}
		}
	//mHeap.print(std::cout);
	}


bool DecimationMesh::decimate(unsigned int targetFaces)
	{
	// We can't collapse down to less than two faces
	if (targetFaces < 4) targetFaces = 4;

	// Keep collapsing one edge at a time until the target is reached
	// or the heap is empty (when we have no possible collapses left)
	while (mFaces.size() - mNumCollapsedFaces > targetFaces && !mHeap.isEmpty())
		decimate();

	// Return true if target is reached
	std::cout << "Collapsed mesh to " << mFaces.size() - mNumCollapsedFaces << " faces" << std::endl;
	return mFaces.size() - mNumCollapsedFaces == targetFaces;
	}


bool DecimationMesh::decimate()
	{
	EdgeCollapse * collapse = static_cast<EdgeCollapse *>(mHeap.pop());
	if (collapse == NULL) return false;

	// Stop the collapse when we only have two triangles left
	// (the smallest entity representable)
	if (mFaces.size() - mNumCollapsedFaces == 4) return false;

	unsigned int e1 = collapse->halfEdge;
	unsigned int e2 = mEdges[e1].pair;

	unsigned int v1 = mEdges[e1].vert;
	unsigned int v2 = mEdges[e2].vert;
	unsigned int v3 = mEdges[mEdges[e1].prev].vert;
	unsigned int v4 = mEdges[mEdges[e2].prev].vert;

	unsigned int f1 = mEdges[e1].face;
	unsigned int f2 = mEdges[e2].face;

#ifdef _DEBUG
	std::cout << "Collapsing faces " << f1 << " and " << f2 << std::endl;
	std::cout << "Collapsing edges " << e1 << ", " << mEdges[e1].next << ", " << mEdges[e1].prev;
	std::cout << ", " << e2 << ", " << mEdges[e2].next << " and " << mEdges[e2].prev << std::endl;
	std::cout << "Collapsing vertex " << v1 << std::endl;
#endif // _DEBUG

	// Verify that the collapse is valid, exit if not so
	if (!isValidCollapse(collapse)) {
		delete collapse;
		mHalfEdge2EdgeCollapse[e1] = NULL;
		mHalfEdge2EdgeCollapse[e2] = NULL;
#ifdef _DEBUG
		std::cout << "failed..." << std::endl;
#endif // _DEBUG
		return false;
		}


	// We want to remove v1, so we need to connect all of v1's half-edges to v2
	unsigned int edge = mVerts[v1].edge;
	do {
		mEdges[edge].vert = v2;
		edge = mEdges[mEdges[edge].pair].next;
		} while (edge != mVerts[v1].edge);

	// Make sure v2 points to a valid edge
	while (mEdges[mVerts[v2].edge].face == f1 || mEdges[mVerts[v2].edge].face == f2)
		mVerts[v2].edge = mEdges[mEdges[ mVerts[v2].edge ].pair].next;

	// Make sure v3 points to a valid edge
	while (mEdges[mVerts[v3].edge].face == f1)
		mVerts[v3].edge = mEdges[mEdges[ mVerts[v3].edge ].pair].next;

	// Make sure v4 points to a valid edge
	while (mEdges[mVerts[v4].edge].face == f2)
		mVerts[v4].edge = mEdges[mEdges[ mVerts[v4].edge ].pair].next;

	// Redirect pair pointers
	mEdges[mEdges[mEdges[e1].next].pair].pair = mEdges[mEdges[e1].prev].pair;
	mEdges[mEdges[mEdges[e1].prev].pair].pair = mEdges[mEdges[e1].next].pair;

	mEdges[mEdges[mEdges[e2].next].pair].pair = mEdges[mEdges[e2].prev].pair;
	mEdges[mEdges[mEdges[e2].prev].pair].pair = mEdges[mEdges[e2].next].pair;

	// Move v2 to its new position
	mVerts[v2].vec = collapse->position;

	// One edge collapse further removes 2 additional collapse
	// candidates from the heap
	if (mHalfEdge2EdgeCollapse[mEdges[e1].prev] != NULL)
		delete mHeap.remove(mHalfEdge2EdgeCollapse[mEdges[e1].prev]);
	mHalfEdge2EdgeCollapse[mEdges[mEdges[e1].prev].pair] = mHalfEdge2EdgeCollapse[mEdges[e1].next];

	if (mHalfEdge2EdgeCollapse[mEdges[e2].next] != NULL)
		delete mHeap.remove(mHalfEdge2EdgeCollapse[mEdges[e2].next]);
	mHalfEdge2EdgeCollapse[mEdges[mEdges[e2].next].pair] = mHalfEdge2EdgeCollapse[mEdges[e2].prev];

	// Make sure the edge collapses point to valid edges
	if (mHalfEdge2EdgeCollapse[mEdges[e1].next] != NULL)
		mHalfEdge2EdgeCollapse[mEdges[e1].next]->halfEdge = mEdges[mEdges[e1].prev].pair;
	if (mHalfEdge2EdgeCollapse[mEdges[e2].prev] != NULL)
		mHalfEdge2EdgeCollapse[mEdges[e2].prev]->halfEdge = mEdges[mEdges[e2].next].pair;

	delete collapse;

	// Collapse the neighborhood
	collapseFace(f1);
	collapseFace(f2);

	collapseEdge(e1);
	collapseEdge(mEdges[e1].next);
	collapseEdge(mEdges[e1].prev);

	collapseEdge(e2);
	collapseEdge(mEdges[e2].next);
	collapseEdge(mEdges[e2].prev);

	collapseVertex(v1);

	// Finally, loop through neighborhood of v2 and update all edge collapses
	// (and remove possible invalid cases)
	updateVertexProperties(v2);
	edge = mVerts[v2].edge;
	do {
		unsigned int face = mEdges[edge].face;
		unsigned int vert = mEdges[mEdges[edge].pair].vert;
		if (!isFaceCollapsed(face))    updateFaceProperties(face);
		if (!isVertexCollapsed(vert))  updateVertexProperties(vert);

		collapse = mHalfEdge2EdgeCollapse[edge];
		if (collapse != NULL) {
			if (!isValidCollapse(collapse)) {
				delete mHeap.remove(collapse);
				mHalfEdge2EdgeCollapse[edge] = NULL;
				mHalfEdge2EdgeCollapse[mEdges[edge].pair] = NULL;
#ifdef _DEBUG
				std::cout << "Removed one invalid edge collapse" << std::endl;
#endif // _DEBUG
				}
			else {
				computeCollapse(collapse);
				mHeap.update(collapse);
				}
			}

		edge = mEdges[mEdges[edge].pair].next;
		} while (edge != mVerts[v2].edge);

	//mHeap.print(std::cout);

	return true;
	}


void DecimationMesh::updateVertexProperties(unsigned int ind)
	{
	std::vector<unsigned int> neighbourTriangles;
	findNeighbourTriangles(ind, neighbourTriangles);

	// Approximate vertex normal
	Vector3<float> n(0,0,0);

	const unsigned int numCandidates = neighbourTriangles.size();
	for (unsigned int i = 0; i < numCandidates; i++){
		Face& triangle = mFaces[neighbourTriangles[i]];

		// Calculate face normal
		HalfEdge* edge = &mEdges[triangle.edge];

		Vector3<float>& p0 = mVerts[edge->vert].vec;
		edge = &mEdges[edge->next];

		Vector3<float>& p1 = mVerts[edge->vert].vec;
		edge = &mEdges[edge->next];

		Vector3<float>& p2 = mVerts[edge->vert].vec;

		Vector3<float> v1 = p1-p0;
		Vector3<float> v2 = p2-p0;
		Vector3<float> faceNormal = cross(v1,v2);
		faceNormal.normalize();

		n += faceNormal;
		}

	n.normalize();
	mVerts[ind].normal = n;
	}


void DecimationMesh::updateFaceProperties(unsigned int ind)
	{
	HalfEdge* edge = &mEdges[mFaces[ind].edge];

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

	mFaces[ind].normal = n;
	}


bool DecimationMesh::isValidCollapse(EdgeCollapse * collapse)
	{
	unsigned int e1 = collapse->halfEdge;
	unsigned int e2 = mEdges[e1].pair;

	unsigned int v1 = mEdges[e1].vert;
	unsigned int v2 = mEdges[e2].vert;
	unsigned int v3 = mEdges[mEdges[e1].prev].vert;
	unsigned int v4 = mEdges[mEdges[e2].prev].vert;

	// Do a dummy check
	if (isEdgeCollapsed(e1) || isEdgeCollapsed(e2) || isVertexCollapsed(v1) || isVertexCollapsed(v2)) return false;

	unsigned int edge = mVerts[v2].edge;
	std::vector<unsigned int> neighbors;
	do {
		unsigned int ind = mEdges[mEdges[edge].pair].vert;
		if (ind != v3 && ind != v4)
			neighbors.push_back(ind);
		edge = mEdges[mEdges[edge].pair].next;
		} while (edge != mVerts[v2].edge);

	edge = mVerts[v1].edge;
	do {
		unsigned int ind = mEdges[mEdges[edge].pair].vert;
		if (find(neighbors.begin(), neighbors.end(), ind) != neighbors.end())
			return false;

		edge = mEdges[mEdges[edge].pair].next;
		} while (edge != mVerts[v1].edge);

	return true;
	}




//-----------------------------------------------------------------------------
void DecimationMesh::draw()
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
	const unsigned int numTriangles = mFaces.size();
	for (unsigned int i = 0; i < numTriangles; i++){

		if (isFaceCollapsed(i)) continue;

		// Render without notations
		HalfEdge* edge = &mEdges[mFaces[i].edge];
		Vector3<float>& p0 = mVerts[edge->vert].vec;
		edge = &mEdges[edge->next];
		Vector3<float>& p1 = mVerts[edge->vert].vec;
		edge = &mEdges[edge->next];
		Vector3<float>& p2 = mVerts[edge->vert].vec;


		// Render with notations
		/*  Uncomment this block, and comment the block above
		to render with notations
		char buffer[10];
		glColor3f(1.0, 0.0, 0.0);
		HalfEdge* edge = &mEdges[mFaces[i].edge];

		// draw face
		sprintf(buffer, "f%i\n", i);
		Vector3<float> vec = (mVerts[edge->vert].vec + mVerts[mEdges[edge->pair].vert].vec)*0.5;
		vec += 0.5 * (mVerts[mEdges[edge->prev].vert].vec - vec);
		GUI::drawText(vec, buffer);

		// draw e1
		sprintf(buffer, "e%i\n", mFaces[i].edge);
		vec = (mVerts[edge->vert].vec + mVerts[mEdges[edge->pair].vert].vec)*0.5;
		vec += 0.1 * (mVerts[mEdges[edge->prev].vert].vec - vec);
		GUI::drawText(vec, buffer);

		// draw v1
		Vector3<float>& p0 = mVerts[edge->vert].vec;
		sprintf(buffer, "v%i\n", edge->vert);
		GUI::drawText(vec, buffer);

		sprintf(buffer, "e%i\n", edge->next);
		edge = &mEdges[edge->next];

		// draw e2
		vec = (mVerts[edge->vert].vec + mVerts[mEdges[edge->pair].vert].vec)*0.5;
		vec += 0.1 * (mVerts[mEdges[edge->prev].vert].vec - vec);
		GUI::drawText(vec, buffer);

		// draw v2
		Vector3<float>& p1 = mVerts[edge->vert].vec;
		sprintf(buffer, "v%i\n", edge->vert);
		GUI::drawText(vec, buffer);

		sprintf(buffer, "e%i\n", edge->next);
		edge = &mEdges[edge->next];

		// draw e3
		vec = (mVerts[edge->vert].vec + mVerts[mEdges[edge->pair].vert].vec)*0.5;
		vec += 0.1 * (mVerts[mEdges[edge->prev].vert].vec - vec);
		GUI::drawText(vec, buffer);

		// draw v3
		Vector3<float>& p2 = mVerts[edge->vert].vec;
		sprintf(buffer, "v%i\n", edge->vert);
		GUI::drawText(vec, buffer);
		*/


		glBegin(GL_TRIANGLES);
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

			// First you need to calculate the vertex normals in calculateVertexNormals
			// Then you need to get correct vertex normals here
			//std::cerr << "Vertex normal shading in HalfEdgeMesh::draw() not implemented. ";
			Vector3<float> n0, n1, n2;

			edge = &mEdges[mFaces[i].edge];

			n0 = mVerts[edge->vert].normal;
			edge = &mEdges[edge->next];

			n1 = mVerts[edge->vert].normal;
			edge = &mEdges[edge->next];

			n2 = mVerts[edge->vert].normal;

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
		glEnd();

		}

	// Restore modelview matrix
	glPopMatrix();
	}

bool DecimationMesh::cleanup()
	{
	//CALCULATE SHIFTs for FACES
	unsigned int tempFaceSize( mFaceSize );
	for(unsigned int i = 0; i<tempFaceSize; i++)
		{
		//If the face is collapsed, 
		//the last element replaces that one,
		//AND the new index is saved to collapsed faces vector
		if( isFaceCollapsed( i ) )
			{
			// while a UNCOLLAPSED end found... 
			while( isFaceCollapsed( --tempFaceSize ) )
				{
				mCollapsedFaces[ tempFaceSize ] = UNCOLLAPSED;
				mFaces.pop_back();
				mFaceSize--;
				}
			if(i <= tempFaceSize)
				{
				mFaces[ i ] = mFaces[ tempFaceSize ];
				mFaces.pop_back();
				mFaceSize--;
				mCollapsedFaces[ tempFaceSize ] = i;
				mCollapsedFaces[ i ] = UNCOLLAPSED;
				}
			}
		}

#ifdef _DEBUG
	assert( mFaceSize == mFaces.size() );
#endif // _DEBUG

	//CALCULATE SHIFTs for EDGES
	unsigned int tempEdgeSize( mEdgeSize );
	for(unsigned int i = 0; i<tempEdgeSize; i++)
		{
		//If the edge is collapsed, 
		//the last element replaces that one,
		//AND the new index is saved to collapsed edge vector
		if( isEdgeCollapsed( i ) )
			{
			// while a UNCOLLAPSED end found... 
			while( isEdgeCollapsed( --tempEdgeSize ) )
				{
				mCollapsedEdges[ tempEdgeSize ] = UNCOLLAPSED;
				mEdges.pop_back();
				
				mHalfEdge2EdgeCollapse.pop_back();
				mEdgeSize--;
				}

			if(i <= tempEdgeSize )
				{
				//copy from the end to replace the removed one
				mEdges[ i ] = mEdges[ tempEdgeSize ];
				mEdges.pop_back();

				//copy from the end to replace the removed one
				mHalfEdge2EdgeCollapse[ i ] = mHalfEdge2EdgeCollapse[ tempEdgeSize ];
				mHalfEdge2EdgeCollapse.pop_back();

				mEdgeSize--;
				mCollapsedEdges[ tempEdgeSize ] = i;
				mCollapsedEdges[ i ] = UNCOLLAPSED;
				}
			}
		}

#ifdef _DEBUG
	assert( mEdgeSize == mEdges.size() );
#endif // _DEBUG

	//CALCULATE SHIFTs for VERTS
	unsigned int tempVertSize( mVertSize );
	for(unsigned int i = 0; i<tempVertSize; i++)
		{
		//If the Vertex is collapsed, 
		//the last element replaces that one,
		//AND the new index is saved to collapsed Vertex vector
		if( isVertexCollapsed( i ) )
			{
			// while a UNCOLLAPSED end found... 
			while( isVertexCollapsed( --tempVertSize ) )
				{
				mCollapsedVerts[ tempVertSize ] = UNCOLLAPSED;
				mVerts.pop_back();
				mVertSize--;

				//pass information about the collapse to other parties
				vertexCollapsed( tempVertSize );
				}

			if(i <= tempVertSize )
				{
				mVerts[ i ] = mVerts[ tempVertSize ];

				mVerts.pop_back();
				mVertSize--;
				vertexCollapsed( i );

				mCollapsedVerts[ tempVertSize ] = i;
				mCollapsedVerts[ i ] = UNCOLLAPSED;
				}
			}
		}

#ifdef _DEBUG
	assert( mVertSize == mVerts.size() );
#endif // _DEBUG

	//-----------------------------------------
	//UPDATES:

	//UPDATE FACES
	for (unsigned int i=0; i<mFaceSize; i++ )
		{
		unsigned int edge = mFaces[i].edge;
		if( UNCOLLAPSED != mCollapsedEdges[edge] )
			{
			mFaces[i].edge = mCollapsedEdges[edge];
			}
		}
	//UPDATE EDGES
	for (unsigned int i=0; i<mEdgeSize; i++ )
		{
		//FACES
		unsigned int face = mEdges[i].face;
		if( UNCOLLAPSED != mCollapsedFaces[face] )
			{
			mEdges[i].face = mCollapsedFaces[face];
			}
		//VERTS
		unsigned int vert = mEdges[i].vert;
		if( UNCOLLAPSED != mCollapsedVerts[vert] )
			{
			mEdges[i].vert = mCollapsedVerts[vert];
			}
		//NEXT
		unsigned int edge = mEdges[i].next;
		if( UNCOLLAPSED != mCollapsedEdges[edge] )
			{
			mEdges[i].next = mCollapsedEdges[edge];
			}
		//PREV
		edge = mEdges[i].prev;
		if( UNCOLLAPSED != mCollapsedEdges[edge] )
			{
			mEdges[i].prev = mCollapsedEdges[edge];
			}
		//PAIR
		edge = mEdges[i].pair;
		if( UNCOLLAPSED != mCollapsedEdges[edge] )
			{
			mEdges[i].pair = mCollapsedEdges[edge];
			}
		}
	//UPDATE VERTICES
	for (unsigned int i=0; i<mVertSize; i++ )
		{
		unsigned int edge = mVerts[i].edge;
		if( UNCOLLAPSED != mCollapsedEdges[edge] )
			{
			mVerts[i].edge = mCollapsedEdges[edge];
			}
		}

	//UPDATE HEAPABLE	
	for (unsigned int i=0, endI = mHalfEdge2EdgeCollapse.size(); i<endI; i++ )
		{
		if( NULL != mHalfEdge2EdgeCollapse[i])
			{
			unsigned int edge = mHalfEdge2EdgeCollapse[i]->halfEdge;
			if( UNCOLLAPSED != mCollapsedEdges[edge] )
				{
				mHalfEdge2EdgeCollapse[i]->halfEdge = mCollapsedEdges[edge];
				}
			}
		}
	
	//UPDATE HEAP
	std::vector<Heap::Heapable* > heapNodes =  mHeap.getNodes();
	for (int i = 0; i < heapNodes.size() ; i++)
	{
		//HEAP NODES
		unsigned int edge = heapNodes[i]->position;
		if( edge != std::numeric_limits<unsigned int>::max() && 
			UNCOLLAPSED != mCollapsedEdges[edge] )
		{
			heapNodes[i]->position = mCollapsedEdges[edge];
		}
	}


	mNumCollapsedVerts=0;
	mNumCollapsedEdges=0;
	mNumCollapsedFaces=0;
	return true;
	}

void DecimationMesh::vertexCollapsed( unsigned int aIndex )
	{
	}
