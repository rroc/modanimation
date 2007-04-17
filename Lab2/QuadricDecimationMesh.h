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
#ifndef _QUADRIC_DECIMATION_MESH
#define _QUADRIC_DECIMATION_MESH

#include <iomanip>
#include "DecimationMesh.h"
#include "Matrix4x4.h"

#ifdef __APPLE__
#include "GLUT/glut.h"
#else
#include "GL/glut.h"
#endif

class QuadricDecimationMesh : public DecimationMesh
{
public :
  QuadricDecimationMesh()
  {
    mQuadratic = NULL;
  }
  virtual ~QuadricDecimationMesh() { }

  //! Initialize member data (error quadrics)
  virtual void initialize();

  //! Draws the mesh
  virtual void draw();

protected :
  //! Draws the error ellipsoids
  void drawQuadrics();

  //! The quadric object used to draw the error ellipsoids
  GLUquadricObj *mQuadratic;

  //! Compute the cost and new position for an edge collapse
  virtual void computeCollapse(EdgeCollapse * collapse);
  //! Update vertex properties. Used after an edge collapse
  virtual void updateVertexProperties(unsigned int ind);
  //! Compute the quadric for a vertex
  Matrix4x4<float> createQuadricForVert(unsigned int indx) const;
  //! Copmute the quadric for a face
  Matrix4x4<float> createQuadricForFace(unsigned int indx) const;

  //! The quadrics used in the decimation
  std::vector<Matrix4x4<float> > mQuadrics;

};

#endif

