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
#include "SimpleMesh.h"
#include "ColorMap.h"

using namespace std;

//-----------------------------------------------------------------------------
SimpleMesh::SimpleMesh()
{
}

//-----------------------------------------------------------------------------
SimpleMesh::~SimpleMesh()
{
}

//-----------------------------------------------------------------------------
bool SimpleMesh::addTriangle(const Vector3<float> &v1, const Vector3<float> &v2, const Vector3<float> & v3){

  unsigned int ind1, ind2, ind3;
  addVertex(v1, ind1);
  addVertex(v2, ind2);
  addVertex(v3, ind3);

  Face face;
  face.v1 = ind1;
  face.v2 = ind2;
  face.v3 = ind3;
  mFaces.push_back(face);

  return true;
}

//-----------------------------------------------------------------------------
bool SimpleMesh::addVertex(const Vector3<float> & v, unsigned int &indx){
  std::map<Vector3<float>,unsigned int>::iterator it = mUniqueVerts.find(v);
  if (it != mUniqueVerts.end()){
    indx = (*it).second; // (*it).second == it->second, which to me was not as clear...
    return false;
  }

  mUniqueVerts[v] = indx = mVerts.size(); // op. [ ] constructs a new entry in map
  mVerts.push_back(v);

  return true;
}

//-----------------------------------------------------------------------------
bool SimpleMesh::findNeighbourTriangles(const unsigned int vertexIndex, std::vector<unsigned int>& foundTriangles) const
{
  foundTriangles.clear();

  // Find other triangles that include this vertex
  std::vector<Face>::const_iterator iter = mFaces.begin();
  std::vector<Face>::const_iterator iend = mFaces.end();
  while (iter != iend) {
    if ((*iter).v1 == vertexIndex ||
      (*iter).v2 == vertexIndex ||
      (*iter).v3 == vertexIndex)
      {
        foundTriangles.push_back( iter - mFaces.begin());
      }

    iter++;
  }

  return !(foundTriangles.size() == 0);
}



//-----------------------------------------------------------------------------
void SimpleMesh::calculateFaceNormals()
{
  mNormals.clear();

  // Find other triangles that include this vertex
  std::vector<Face>::iterator iter = mFaces.begin();
  std::vector<Face>::iterator iend = mFaces.end();
  while (iter != iend) {

    // Calculate face normal
    Vector3<float>& p0 = mVerts[(*iter).v1];
    Vector3<float>& p1 = mVerts[(*iter).v2];
    Vector3<float>& p2 = mVerts[(*iter).v3];

    Vector3<float> v1 = p1-p0;
    Vector3<float> v2 = p2-p0;
    Vector3<float> n = cross(v1,v2);
    n.normalize();
    mNormals.push_back(n);

    iter++;
  }
}

//-----------------------------------------------------------------------------
void SimpleMesh::calculateVertexNormals()
{
  mNormals.clear();

  std::vector<unsigned int> neighbourTriangles;
  const unsigned int numVertices = mVerts.size();
  for (unsigned int v = 0; v < numVertices; v++){

    findNeighbourTriangles(v, neighbourTriangles);

    // Approximate vertex normal
    Vector3<float> n(0,0,0);

    const unsigned int numCandidates = neighbourTriangles.size();
    for (unsigned int i = 0; i < numCandidates; i++){
      Face& triangle = mFaces[neighbourTriangles[i]];

      // Calculate face normal
      Vector3<float>& p0 = mVerts[triangle.v1];
      Vector3<float>& p1 = mVerts[triangle.v2];
      Vector3<float>& p2 = mVerts[triangle.v3];

      Vector3<float> v1 = p1-p0;
      Vector3<float> v2 = p2-p0;
      Vector3<float> faceNormal = cross(v1,v2);
      faceNormal.normalize();

      n += faceNormal;
    }

    n.normalize();
    mNormals.push_back(n);
  }

}

//-----------------------------------------------------------------------------
void SimpleMesh::draw()
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  GLfloat opacity = 1.0;

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
    Face& triangle = mFaces[i];

    Vector3<float>& p0 = mVerts[triangle.v1];
    Vector3<float>& p1 = mVerts[triangle.v2];
    Vector3<float>& p2 = mVerts[triangle.v3];



    if (getShadingFlag() == FLAT_SHADING || mNormals.size() == mFaces.size()){;
      Vector3<float>& n = mNormals[i];

      Vector3<float> color = colorMap.map(n, -1, 1);

      glColor4f(color[0],color[1], color[2], opacity);
      glNormal3f(n[0], n[1], n[2]);
      glVertex3f(p0[0], p0[1], p0[2]);
      glVertex3f(p1[0], p1[1], p1[2]);
      glVertex3f(p2[0], p2[1], p2[2]);
    }
    else if (getShadingFlag() == SMOOTH_SHADING && mNormals.size() == mVerts.size()){

      // Fetching normals - the normal index is the same as the vertex index
      Vector3<float>& n0 = mNormals[triangle.v1];
      Vector3<float>& n1 = mNormals[triangle.v2];
      Vector3<float>& n2 = mNormals[triangle.v3];

      // Color mapping
      Vector3<float> color0;
      Vector3<float> color1;
      Vector3<float> color2;

      /*color0 = colorMap.map(curvatureArray[triangle.v1], -3, 3);
      color1 = colorMap.map(curvatureArray[triangle.v2], -3, 3);
      color2 = colorMap.map(curvatureArray[triangle.v3], -3, 3);*/

      color0 = colorMap.map(n0, -1, 1);
      color1 = colorMap.map(n1, -1, 1);
      color2 = colorMap.map(n2, -1, 1);


      glColor4f(color0[0],color0[1], color0[2], opacity);
      glNormal3f(n0[0], n0[1], n0[2]);
      glVertex3f(p0[0], p0[1], p0[2]);

      glColor4f(color1[0],color1[1], color1[2], opacity);
      glNormal3f(n1[0], n1[1], n1[2]);
      glVertex3f(p1[0], p1[1], p1[2]);

      glColor4f(color2[0],color2[1], color2[2], opacity);
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

  glDisable(GL_BLEND);
}


//-----------------------------------------------------------------------------
float SimpleMesh::curvature(const unsigned int vertexIndex, const Vector3<float>& n)
{
  const Vector3<float>& v = mVerts[vertexIndex];
  float curvature = 0;

  std::vector<unsigned int> neighbourTriangles;
  std::map<Vector3<float>, unsigned int> localVertexList;

  findNeighbourTriangles(vertexIndex, neighbourTriangles);
  assert(neighbourTriangles.size() != 0);

  // calculate area
  float A = 0;
  for (unsigned int i = 0; i < neighbourTriangles.size(); i++){
    Face& tri = mFaces[neighbourTriangles[i]];
    Vector3<float>& v0 = mVerts[tri.v1];
    Vector3<float>& v1 = mVerts[tri.v2];
    Vector3<float>& v2 = mVerts[tri.v3];
    A += triArea(v0,v1,v2);

    // Add vertices to localList
    std::map<Vector3<float>,unsigned int>::iterator it = localVertexList.find(v0);
    if (it == localVertexList.end()){
      localVertexList[v0] = 1;
    }
    it = localVertexList.find(v1);
    if (it == localVertexList.end()){
      localVertexList[v1] = 1;
    }
    it = localVertexList.find(v2);
    if (it == localVertexList.end()){
      localVertexList[v2] = 1;
    }
  }
  assert(localVertexList.size() == neighbourTriangles.size()+1);

  // Calculate curvature vector
  Vector3<float> curv(0,0,0);

  std::map<Vector3<float>,unsigned int>::iterator iter, iend;
  iter = localVertexList.begin();
  iend = localVertexList.end();

  float Avor = 0;
  while (iter != iend){
    // If not this vertex
    const Vector3<float>& localV = (*iter).first;

    if (localV != v){
      // Find the two triangles that contain localV
      std::vector<Vector3<float> > neighbourVertices;
      for (unsigned int i = 0; i < neighbourTriangles.size(); i++){
        const Face& f = mFaces[neighbourTriangles[i]];
        const Vector3<unsigned int> t(f.v1, f.v2, f.v3);

        bool vFound = false;
        bool localVFound = false;
        Vector3<float> otherVertex;
        for (int j = 0; j < 3; j++){
          if (mVerts[t[j]] == v){
            vFound = true;
          }
          else if (mVerts[t[j]] == localV){
            localVFound = true;
          }
          else{
            otherVertex = mVerts[t[j]];
          }
        }

        if (vFound && localVFound){
          neighbourVertices.push_back(otherVertex);
        }
      }

      assert(neighbourVertices.size() == 2);
      assert(neighbourVertices[0] != neighbourVertices[1]);
      assert(neighbourVertices[0] != v);
      assert(neighbourVertices[0] != localV);
      assert(neighbourVertices[1] != v);
      assert(neighbourVertices[1] != localV);

      // Calculate angles:
      float angle[2];
      for (int i = 0; i < 2; i++){
        Vector3<float> v1 = (v -  neighbourVertices[i]);
        Vector3<float> v2 = (localV - neighbourVertices[i]);
        v1.normalize();
        v2.normalize();
        angle[i] = acos(v1*v2);

        assert(angle[i] >= 0.0);
        assert(angle[i] <= M_PI);
      }
      Vector3<float> resVec = v - localV;
      //std::cerr << "Angles: " << angle[0] << ", " << angle[1] << ", l: " << resVec.length() << "\n";
      curv += resVec*(1.0/tan(angle[0]) + 1.0/tan(angle[1]));

      Avor += 1.0/8.0*(1.0/tan(angle[0]) + 1.0/tan(angle[1]))*resVec.length()*resVec.length();
    }

    iter++;
  }
  //  std::cerr << "Area: " << A << ", voronoi area: " << Avor << "\n";

  // Scaling
  curv *= 1.0/(4.0*Avor);
  curvature = curv.length();

  // Find sign (requires vertex normals)
  curv.normalize();
  Vector3<float> locn = n;
  locn.normalize();
  //std::cerr << "Kappa: " << curvature << ", kappa2: " <<  curvature*4*Avor/A << "\n";
  if (curv*locn < 0){
    curvature *= -1.0;
  }
  return curvature;
}

void SimpleMesh::calcCurv()
{
  mMinCurv = std::numeric_limits<float>::max();
  mMaxCurv = std::numeric_limits<float>::min();

  float area = 0;
  for (unsigned int i = 0; i < mFaces.size(); i++){
    Face& tri = mFaces[i];
    Vector3<float>& v0 = mVerts[tri.v1];
    Vector3<float>& v1 = mVerts[tri.v2];
    Vector3<float>& v2 = mVerts[tri.v3];
    area += triArea(v0,v1,v2);
  }

  curvatureArray.clear();

  const unsigned int numVerts = mVerts.size();
  for (unsigned int i = 0; i < numVerts; i++){
    float k = curvature(i, mNormals[i]);
    mMinCurv = min(mMinCurv, k);
    mMaxCurv = max(mMaxCurv, k);

    //printf("K: %f   r: %f\n", k, 1/k);
    curvatureArray.push_back(k);
  }

  //printf("%f %f\n", mMinCurv, mMaxCurv);

  //cerr << area << endl;
  // Ta hänsyn till skalning???
}
