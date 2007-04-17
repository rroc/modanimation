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
