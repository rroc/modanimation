#include "AdaptiveLoopSubdivisionMesh.h"
#include <cmath>
#include <cassert>

AdaptiveLoopSubdivisionMesh::AdaptiveLoopSubdivisionMesh(const HalfEdgeMesh & m, unsigned int s) :
  LoopSubdivisionMesh(m,s)
{
}

AdaptiveLoopSubdivisionMesh::AdaptiveLoopSubdivisionMesh()
{
}

AdaptiveLoopSubdivisionMesh::~AdaptiveLoopSubdivisionMesh()
{
}

bool AdaptiveLoopSubdivisionMesh::subdivide()
{
  return LoopSubdivisionMesh::subdivide();
}

bool AdaptiveLoopSubdivisionMesh::subdivide(float flatAngle)
{
  HalfEdgeMesh subDivMesh;
  subDivMesh.setTransform(getTransform());

  // Preprocessing: Classify triangles as flat or no flat (flat = no subdivide)
  // and calculate flatness
  findFlatTriangles(flatAngle);
  calculateDegreeOfFlatness();

  std::vector<HalfEdgeMesh::Face>::const_iterator it = mFaces.begin();
  std::vector<HalfEdgeMesh::Face>::const_iterator iend = mFaces.end();
  std::vector<bool>::const_iterator flatFaceIter = mFaceIsFlat.begin();
  std::vector<unsigned int>::const_iterator flatnessIter = mFlatness.begin();

  // loop over each face and create 4 new ones
  while(it != iend){

    // Only subdivide if flatness is 0
    if (*flatnessIter == 0)
      {
        // get the inner halfedges
        unsigned int e0, e1, e2;
        e0 = (*it).edge;
        e1 = mEdges[(*it).edge].next;
        e2 = mEdges[(*it).edge].prev;


        // Compute positions of the vertices
        Vector3<float> pn0 = computeVertex(e0);
        Vector3<float> pn1 = computeVertex(e1);
        Vector3<float> pn2 = computeVertex(e2);


        // Compute positions of the edge vertices
        Vector3<float> pn3 = computeEdgeVertex(e0);
        Vector3<float> pn4 = computeEdgeVertex(e1);
        Vector3<float> pn5 = computeEdgeVertex(e2);


        // add the four new triangles to new mesh
        subDivMesh.addTriangle(pn0, pn3, pn5);
        subDivMesh.addTriangle(pn3, pn4, pn5);
        subDivMesh.addTriangle(pn3, pn1, pn4);
        subDivMesh.addTriangle(pn5, pn4, pn2);
      }

    ++it;
    ++flatFaceIter;
    ++flatnessIter;
  }

  // Treat all faces with flatness != 0
  treatFlatFaces(subDivMesh);


  // Assigns the new mesh
  *this = AdaptiveLoopSubdivisionMesh(subDivMesh, ++mNumSubDivs);
  // Change return value....
  return mNumSubDivs;
}

void AdaptiveLoopSubdivisionMesh::treatFlatFaces(HalfEdgeMesh& subDivMesh)
{
  // Process all faces that has a flatness != 0
  std::vector<HalfEdgeMesh::Face>::const_iterator it = mFaces.begin();
  std::vector<HalfEdgeMesh::Face>::const_iterator iend = mFaces.end();
  std::vector<bool>::const_iterator flatFaceIter = mFaceIsFlat.begin();
  std::vector<unsigned int>::const_iterator flatnessIter = mFlatness.begin();
  while(it != iend){
    if (*flatnessIter > 0){

      // Loop trough edges and count the number of edges that are shared with neighbour subdivided triangles
      unsigned int numShared = 0;
      unsigned int edge = (*it).edge;
      std::vector<unsigned int> sharedEdges;  // To be set in the algorithm
      for (unsigned int i = 0; i < 3; i++){
        // See if edge is shared by subdivided triangle
        unsigned int neighbourFace = mEdges[mEdges[edge].pair].face;
        if (mFlatness[neighbourFace] == 0){
          sharedEdges.push_back(edge);
          numShared++;
        }
        edge = mEdges[edge].next;
      }

      assert(numShared < 3);

      // If all neighbours are flat or this face is flat
      if (numShared == 0 || *flatnessIter == 3){
        Vector3<float> verts[3];

        // Check if a vertex is shared by a subdivided triangle and calculate the new vertex position in that case.
        unsigned int edge = (*it).edge;
        for (int i = 0; i < 3; i++){
          unsigned int vertexIndex = mEdges[edge].vert;
          bool subdividedNeighbour = isSharedBySubdividedTriangle(vertexIndex);

          // If the vertex associated to this edge is shared by a
          // subdivided triangle we need to find the new position of
          // that vertex (otherwise we get cracks)
          if (subdividedNeighbour){
            verts[i] = computeVertex(edge);
          }
          else{
            verts[i] = mVerts[mEdges[edge].vert].vec;
          }

          edge = mEdges[edge].next;
        }

        // Add the triangle
        subDivMesh.addTriangle(verts[0], verts[1], verts[2]);
      }
      else if (numShared == 1){
        // Strategy 1: only one edge is shared
        Vector3<float> verts[4];

        // Add your code here..

        // Add triangles to the new mesh
      }
      else{
        // Strategy 2: two edges are shared
        Vector3<float> verts[5];


        // Add triangles to the new mesh
      }
    }

    ++it;
    ++flatFaceIter;
    ++flatnessIter;
  }

}

void AdaptiveLoopSubdivisionMesh::findFlatTriangles(float flatAngle)
{
  const int numFaces = mFaces.size();

  // Convert angle to radians
  const float limitAngleRad = flatAngle*M_PI/180.0;


  // Classify all faces as either flat or not flat
  mFaceIsFlat.clear();
  mFaceIsFlat.resize(numFaces, false);
  std::vector<bool>::iterator flatFaceIter = mFaceIsFlat.begin();

  std::vector<HalfEdgeMesh::Face>::const_iterator it = mFaces.begin();
  std::vector<HalfEdgeMesh::Face>::const_iterator iend = mFaces.end();
  while(it != iend){

    // Find neighbour faces
    unsigned int neighbourFaceIndex1;
    unsigned int neighbourFaceIndex2;
    unsigned int neighbourFaceIndex3;
    findTriangleNeighbours(*it, neighbourFaceIndex1, neighbourFaceIndex2, neighbourFaceIndex3);

    const Vector3<float>& thisNormal = (*it).normal;
    const Vector3<float>& face1Normal = mFaces[neighbourFaceIndex1].normal;
    const Vector3<float>& face2Normal = mFaces[neighbourFaceIndex2].normal;
    const Vector3<float>& face3Normal = mFaces[neighbourFaceIndex3].normal;

    //Test angles
    bool flat = true; // flat face?
    if (angle(thisNormal, face1Normal) > limitAngleRad){
      flat = false;
    }
    else if (angle(thisNormal, face2Normal) > limitAngleRad){
      flat = false;
    }
    else if (angle(thisNormal, face3Normal) > limitAngleRad){
      flat = false;
    }

    (*flatFaceIter) = flat;

    ++it;
    ++flatFaceIter;
  }
}

void AdaptiveLoopSubdivisionMesh::calculateDegreeOfFlatness()
{
  // Find all faces that has a flat neighbour
  const int numFaces = mFaces.size();
  mFlatness.clear();
  mFlatness.resize(numFaces, 0);
  std::vector<unsigned int>::iterator flatnessIter = mFlatness.begin();
  std::vector<bool>::const_iterator flatFaceIter = mFaceIsFlat.begin();
  std::vector<HalfEdgeMesh::Face>::const_iterator it = mFaces.begin();
  std::vector<HalfEdgeMesh::Face>::const_iterator iend = mFaces.end();
  while(it != iend){

    // Find neighbour faces
    unsigned int neighbourFaceIndex1;
    unsigned int neighbourFaceIndex2;
    unsigned int neighbourFaceIndex3;
    findTriangleNeighbours(*it, neighbourFaceIndex1, neighbourFaceIndex2, neighbourFaceIndex3);

    //Test neighbours
    unsigned int flatness = 0; // flat neighbour?
    if (mFaceIsFlat[neighbourFaceIndex1]){
      flatness++;
    }

    if (mFaceIsFlat[neighbourFaceIndex2]){
      flatness++;
    }

    if (mFaceIsFlat[neighbourFaceIndex3]){
      flatness++;
    }

    if (*flatFaceIter){
      // Treat as if all neighbours are flat
      flatness = 3;
    }

    (*flatnessIter) = flatness;

    ++it;
    ++flatnessIter;
    ++flatFaceIter;
  }
}

void AdaptiveLoopSubdivisionMesh::findTriangleNeighbours(const Face& face, unsigned int& neighbourIndex1, unsigned int& neighbourIndex2, unsigned int& neighbourIndex3)
{
  // Find neighbour faces
  unsigned int edgeIndex = face.edge;

  neighbourIndex1 = mEdges[mEdges[edgeIndex].pair].face;
  edgeIndex = mEdges[edgeIndex].next;
  neighbourIndex2 = mEdges[mEdges[edgeIndex].pair].face;
  edgeIndex = mEdges[edgeIndex].next;
  neighbourIndex3 = mEdges[mEdges[edgeIndex].pair].face;

}

unsigned int AdaptiveLoopSubdivisionMesh::findSharedEdge(const Face& face1, const Face& face2)
{
  unsigned int sharedEdge = std::numeric_limits<unsigned int>::max();
  unsigned int numShared = 0;

  // Loop trough edges of triangle 1
  unsigned int f1EdgeIndex = face1.edge;
  for (int i = 0; i < 3; i++){

    // Loop trough edges of triangle 2 and compare their pair with triangle 1
    unsigned int f2EdgeIndex = face2.edge;
    for (int j = 0; j < 3; j++){
      if (f1EdgeIndex == mEdges[f2EdgeIndex].pair){
        sharedEdge = f1EdgeIndex;
        numShared++;
      }
      f2EdgeIndex = mEdges[f2EdgeIndex].next;
    }

    f1EdgeIndex = mEdges[f1EdgeIndex].next;
  }


  assert(numShared == 1);

  return sharedEdge;
}

bool AdaptiveLoopSubdivisionMesh::isSharedBySubdividedTriangle(unsigned int vertexIndex)
{
  bool subdividedNeighbour = false;
  std::vector<unsigned int> neighbourTriangles;
  findNeighbourTriangles(vertexIndex, neighbourTriangles);

  for (unsigned int j = 0; j < neighbourTriangles.size(); j++){
    if (!mFaceIsFlat[neighbourTriangles[j]] && mFlatness[neighbourTriangles[j]] == 0)
      {
        subdividedNeighbour = true;
        break;
      }
  }

  return subdividedNeighbour;
}
