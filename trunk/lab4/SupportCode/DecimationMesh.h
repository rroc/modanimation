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
#ifndef _DECIMATION_MESH
#define _DECIMATION_MESH

#include "DecimationInterface.h"
#include "HalfEdgeMesh.h"
#include "Heap.h"

//Defines
static const unsigned int UNCOLLAPSED = ( std::numeric_limits<unsigned int>::max()-2 );
static const unsigned int COLLAPSED = ( std::numeric_limits<unsigned int>::max()-3 );

class DecimationMesh : public DecimationInterface, public HalfEdgeMesh
	{
	public :

		DecimationMesh() : mNumCollapsedVerts(0), mNumCollapsedEdges(0), mNumCollapsedFaces(0) { }
		virtual ~DecimationMesh() { }

		/*! The EdgeCollapse is a Heapable type */
		struct EdgeCollapse : public Heap::Heapable
			{
			unsigned int halfEdge;
			Vector3<float> position;
			};

		virtual void initialize();

		virtual bool decimate();
		virtual bool decimate(unsigned int targetFaces);

		virtual bool cleanup();

		virtual void draw();

	protected :

		virtual void vertexCollapsed( unsigned int aIndex);

		virtual void updateVertexProperties(unsigned int ind);

		virtual void updateFaceProperties(unsigned int ind);

		virtual void computeCollapse(EdgeCollapse * collapse) = 0;

		bool isValidCollapse(EdgeCollapse * collapse);

		inline bool isVertexCollapsed(unsigned int ind) { return (COLLAPSED == mCollapsedVerts[ind]); }
		inline bool isEdgeCollapsed(unsigned int ind)	{ return (COLLAPSED == mCollapsedEdges[ind]); }
		inline bool isFaceCollapsed(unsigned int ind)	{ return (COLLAPSED == mCollapsedFaces[ind]); }


		inline void collapseVertex(unsigned int ind) {
			mCollapsedVerts[ind] = COLLAPSED;
			mNumCollapsedVerts++;
			}
		inline void collapseEdge(unsigned int ind) {
			mHalfEdge2EdgeCollapse[ind] = NULL;
			mCollapsedEdges[ind] = COLLAPSED;
			mNumCollapsedEdges++;
			}
		inline void collapseFace(unsigned int ind) {
			mCollapsedFaces[ind] = COLLAPSED;
			mNumCollapsedFaces++;
			}

		//! State array of 'active' verts
		std::vector<unsigned int> mCollapsedVerts;
		//! State array of 'active' edges
		std::vector<unsigned int> mCollapsedEdges;
		//! State array of 'active' faces
		std::vector<unsigned int> mCollapsedFaces;

		//! Number of collapsed verts
		unsigned int mNumCollapsedVerts;
		//! Number of collapsed edges
		unsigned int mNumCollapsedEdges;
		//! Number of collapsed faces
		unsigned int mNumCollapsedFaces;

		//! Utility mapping between half edges and collapses
		std::vector<EdgeCollapse *> mHalfEdge2EdgeCollapse;

		//! The heap that stores the edge collapses
		Heap mHeap;
	};

#endif
