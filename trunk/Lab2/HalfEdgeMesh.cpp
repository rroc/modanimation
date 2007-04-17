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
#include "HalfEdgeMesh.h"
#include "ColorMap.h"
#ifdef __APPLE__
#include "GLUT/glut.h"
#else
#include "GL/glut.h"
#endif
#include <limits>
#include <cassert>

const unsigned int HalfEdgeMesh::BORDER = std::numeric_limits<unsigned int>::max();
const unsigned int HalfEdgeMesh::UNINITIALIZED = std::numeric_limits<unsigned int>::max()-1;

HalfEdgeMesh::HalfEdgeMesh()
{
  setShadingFlag(FLAT_SHADING);
}

HalfEdgeMesh::~HalfEdgeMesh()
{
}


/*!
 * \param[in] v1 vertex 1, Vector3<float>
 * \param[in] v2 vertex 2, Vector3<float>
 * \param[in] v3 vertex 3, Vector3<float>
 */
bool HalfEdgeMesh::addTriangle(const Vector3<float> &v1, const Vector3<float> &v2, const Vector3<float> & v3){
  // Add your code here
  std::cerr << "ADD TRIANGLE NOT IMPLEMENTED. ";

  // Add the vertices of the face/triangle

  // Add all half-edge pairs

  // Connect inner ring

  // Finally, create the face

  // All half-edges share the same left face (previously added)

  // Optionally, track the (outer) boundary half-edges
  // to represent non-closed surfaces

  return true;
}


/*!
 * \param [in] v the vertex to add, Vector3<float>
 * \param [out] indx  the index to the vertex, unsigned int
 * \return a bool indicating whether the HalfEdgeMesh::Vertex was successfully inserted (true) or already existed (false)
 */
bool HalfEdgeMesh::addVertex(const Vector3<float> & v, unsigned int &indx){
  std::map<Vector3<float>,unsigned int>::iterator it = mUniqueVerts.find(v);
  if (it != mUniqueVerts.end()){
    indx = (*it).second; // get the index of the already existing vertex
    return false;
  }

  mUniqueVerts[v] = indx = mVerts.size(); // op. [ ] constructs a new entry in map
  Vertex vert;
  vert.vec = v;
  mVerts.push_back(vert); // add it to the vertex list

  return true;
}


/*!
 * Inserts a half edge pair between HalfEdgeMesh::Vertex pointed to by v1 and v2. The first HalfEdgeMesh::HalfEdge (v1->v2) is the inner one, and the second (v2->v1) is the outer.
 * \param [in] v1 vertex 1, Vector3<float>
 * \param [in] v2 vertex 2, Vector3<float>
 * \param [out] indx1  the index to the half-edge from v1 to v2, unsigned int
 * \param [out] indx2  the index to the half-edge from v2 to v1, unsigned int
 * \return a bool indicating whether the half-edge pair was successfully inserted (true) or already existed (false)
 */
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

  // If not found, calculate both half-edges indices
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

  // Store the edges in mEdges
  mEdges.push_back(edge1);
  mEdges.push_back(edge2);

  return true;
}

/*!
 * Updates the boundary of the mesh to contain the HalfEdgeMesh::HalfEdge pointed to by indx
 * \param [in] indx the index to the half-edge, unsigned int
 */
void HalfEdgeMesh::mergeBoundaryEdge(unsigned int indx)
{
  // Add your code here
}

/*! Proceeds to check if the mesh is valid. All indices are inspected and
 * checked to see that they are initialized. The method checks: mEdges, mFaces and mVerts.
 * Also checks to see if all verts have a neighborhood using the findNeighbourTriangles method.
 */
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


/*! Loops over the neighborhood of a vertex and collects all the faces.
 * \param [in] vertexIndex  the index to vertex, unsigned int
 * \param [out] foundTriangles  a vector containing the indices to all the found faces.
 * \return True if the number of found triangles is larger than 0, false otherwise.
 */
bool HalfEdgeMesh::findNeighbourTriangles(const unsigned int vertexIndex, std::vector<unsigned int>& foundTriangles) const
{
  foundTriangles.clear();

  // Add your code here
  return !(foundTriangles.size() == 0);
}

float HalfEdgeMesh::area() const
{
  // Add code here
  std::cerr << "Area calculation not implemented for half-edge mesh!\n";
  return 0;
}

float HalfEdgeMesh::volume() const
{
  // Add code here
  std::cerr << "Volume calculation not implemented for half-edge mesh!\n";
  return 0;
}

int HalfEdgeMesh::genus() const
{
  // Add code here
  std::cerr << "Genus calculation not implemented for half-edge mesh!\n";
  return 0;
}

float HalfEdgeMesh::curvature(const unsigned int vertexIndex, const Vector3<float>& n)
{
  // Add code here
  std::cerr << "Curvature calculation not implemented for half-edge mesh!\n";
  return 0;
}

void HalfEdgeMesh::calculateFaceNormals()
{
  // Add code here
  std::cerr << "Face normals calculation not implemented for half-edge mesh!\n";

}

void HalfEdgeMesh::calculateVertexNormals()
{
  // Add code here
  std::cerr << "Vertex normals calculation not implemented for half-edge mesh!\n";

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

      // First you need to calculate the vertex normals in calculateVertexNormals
      // Then you need to get correct vertex normals here
      std::cerr << "Vertex normal shading in HalfEdgeMesh::draw() not implemented. ";
      Vector3<float> n0, n1, n2;

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
