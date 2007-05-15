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
#ifndef __MESH_H__
#define __MESH_H__

#include "Geometry.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include <vector>


class Mesh : public Geometry{
public:
  enum SHADING {SMOOTH_SHADING, FLAT_SHADING};
protected:
  //! Adds a vertex to the mesh
  virtual bool addVertex(const Vector3<float>& v, unsigned int &indx) = 0;

  // Given a vertex, find all triangles that includes this vertex
  virtual bool findNeighbourTriangles(const unsigned int vertexIndex, std::vector<unsigned int>& foundTriangles) const = 0;

  SHADING mShadingFlag;
public:
  Mesh() : mShadingFlag(SMOOTH_SHADING) { }
  ~Mesh() { }

  //! Adds a triangle to the mesh. \sa addTriangle
  virtual bool addTriangle(const Vector3<float> &v1, const Vector3<float> &v2, const Vector3<float> & v3) = 0;

  //! Compute area of mesh
  virtual float area() const;
  //! Compute volume of mesh
  virtual float volume() const;

  //! Compute genus of mesh
  virtual int genus() const;

  //! Calculate the area of the triangle given by 3 vertices
  static float triArea(const Vector3<float> &v1, const Vector3<float> &v2, const Vector3<float> & v3);
  virtual float curvature(const unsigned int vertexIndex, const Vector3<float>& n) = 0;

  //! Methods for calculating normals
  virtual void calculateFaceNormals();
  virtual void calculateVertexNormals();

  //! Draw call
  virtual void draw() = 0;
  void setShadingFlag(SHADING s);
  SHADING getShadingFlag() const;
};


#endif
