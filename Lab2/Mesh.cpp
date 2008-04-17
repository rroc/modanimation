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
#include "Mesh.h"
#include <iostream>

float Mesh::triArea(const Vector3<float> & v1, const Vector3<float> & v2, const Vector3<float> & v3)
{
  Vector3<float> e1(v2 - v1);
  Vector3<float> e2(v3 - v1);
  return 0.5f*(cross(e1, e2).length());
}

float Mesh::area() const
{
	std::cerr << "Error: area() not implemented for this Mesh" << std::endl;
	return -1;
}

float Mesh::volume() const
{
	std::cerr << "Error: volume() not implemented for this Mesh" << std::endl;
	return -1;
}

int Mesh::genus() const
{
	std::cerr << "Error: genus() not implemented for this Mesh" << std::endl;
	return -1;
}

void Mesh::calculateFaceNormals()
{
	std::cerr << "Error: calculateFaceNormals() not implemented for this Mesh" << std::endl;
}

void Mesh::calculateVertexNormals()
{
	std::cerr << "Error: calculateVertexNormals() not implemented for this Mesh" << std::endl;
}

void Mesh::setShadingFlag(Mesh::SHADING s)
{
  mShadingFlag = s;
}

Mesh::SHADING Mesh::getShadingFlag() const
{
  return mShadingFlag;
}
