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
#ifndef __HALF_EDGE_H__
#define __HALF_EDGE_H__

#include "Mesh.h"
#include <map>
/*! \brief A half edge triangle mesh class.
 * A mesh data structure with fast access to neighborhood entities.
 * The neighborhood links are stored through indices.
 */
class HalfEdgeMesh : public Mesh {
public:
  HalfEdgeMesh();
  ~HalfEdgeMesh();

  //! Adds a triangle to the mesh.
  virtual bool addTriangle(const Vector3<float> &v1, const Vector3<float> &v2, const Vector3<float> & v3);

  //! Calculates the area of the mesh
  virtual float area() const;

  //! Calculates the volume of the mesh
  virtual float volume() const;

  //! Calculates the genus of a mesh
  virtual int genus() const;

  //! Calculates the curvature at a vertex
  virtual float curvature(const unsigned int vertexIndex, const Vector3<float>& n);

  //! Calculates the face normals from info from each triangle.
  virtual void calculateFaceNormals();

  //! Calculates vertex normals using info given by face normals
  virtual void calculateVertexNormals();

  //! Implements an OpenGL draw routine
  virtual void draw();

  //! Checks to see if the mesh is valid
  void validate();

protected:

  //! Denotes a reference to a border, only for face pointers
  const static unsigned int BORDER;
  //! Denotes a reference to a non-existing object
  const static unsigned int UNINITIALIZED;

  /*! \brief The core half-edge struct
   *  Implements the linked data structure edge type
   */
  struct HalfEdge {
    HalfEdge() : vert(UNINITIALIZED), face(UNINITIALIZED), next(UNINITIALIZED), prev(UNINITIALIZED), pair(UNINITIALIZED) { }
    unsigned int vert;  //!< index into mVerts
    unsigned int face;  //!< index into mFaces
    unsigned int next;  //!< index into mEdges
    unsigned int prev;  //!< index into mEdges
    unsigned int pair;  //!< index into mEdges
  };

  /*! \brief A vertex is a point and an edge index
   */
  struct Vertex {
    Vertex() : edge(UNINITIALIZED) { }
    Vector3<float> vec;    //!< the vertex coordinates
    unsigned int edge;     //!< index into mEdges
    Vector3<float> normal; //!< the vertex normal
  };

  /*! \brief A face has a normal and an index to an edge
   */
  struct Face {
    Face() : edge(UNINITIALIZED) { }
    unsigned int edge; //!< index into mEdges
    Vector3<float> normal; //!< the face normal
  };

  //! The edges of the mesh
  std::vector<HalfEdge> mEdges;
  //! The vertices in the mesh
  std::vector<Vertex> mVerts;
  //! The faces in the mesh
  std::vector<Face> mFaces;
  //! The curvature for each vertex in the mesh.
  std::vector<float> mCurvature;

  //! A utility data structure to speed up removal of redundant vertices
  std::map<Vector3<float>, unsigned int> mUniqueVerts;

  //! Adds a vertex to the mesh
  virtual bool addVertex(const Vector3<float>& v, unsigned int &indx);

  //! Adds a half edge pair, from vertex 1 to vertex 2, to the mesh
  bool addHalfEdgePair(unsigned int v1, unsigned int v2, unsigned int &indx1, unsigned int &indx2);

  //! Merges an edge into the boundary of the mesh
  void mergeBoundaryEdge(unsigned int indx);

  //! Finds all triangles that includes a given a vertex.
  virtual bool findNeighbourTriangles(const unsigned int vertexIndex, std::vector<unsigned int>& foundTriangles) const;

};

#endif
