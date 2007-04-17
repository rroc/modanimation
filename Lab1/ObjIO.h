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
#ifndef __obj_io_h__
#define __obj_io_h__

#include "Mesh.h"
#include "Vector3.h"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

class ObjIO {
protected:
	struct LoadData{
		LoadData() : hasNormals(false){}
		bool hasNormals;
		std::vector<Vector3<float> > verts;
		std::vector<Vector3<float> > normals;
		std::vector<Vector3<unsigned int> > tris;
	} loadData;
public:
	ObjIO() {}

	bool load(Mesh *, std::istream & is); // NULL return on error
	bool save(Mesh *, std::ostream & os); // false return indicates error

protected:
	bool readHeader(std::istream &is);
	bool readData(std::istream &is);

	static Vector3<unsigned int> readTri(std::istream &is);
	static void splitQuad(std::istream &is, Vector3<unsigned int>& tri1, Vector3<unsigned int>& tri2, 
		const LoadData& data);
};

#endif
