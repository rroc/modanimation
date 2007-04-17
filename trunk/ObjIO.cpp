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
#include "ObjIO.h"
#include <cassert>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>

bool ObjIO::load(Mesh *mesh, std::istream & is){	
  //std::cerr << "Reading obj file.\nOutputting any skipped line(s) for reference.\n";
  bool success = readHeader(is);
  if(!success) { return false; }

  success = readData(is); 
  if(!success) { return false; }

  loadData.normals.clear();
  loadData.hasNormals = false;

  //  mesh->clear();
  // Build mesh  
  const unsigned int numTris = loadData.tris.size();
  for (unsigned int t = 0; t < numTris; t++){
    Vector3<unsigned int>& triangle = loadData.tris[t];
		
    Vector3<float>& v0 = loadData.verts[triangle[0]];
    Vector3<float>& v1 = loadData.verts[triangle[1]];
    Vector3<float>& v2 = loadData.verts[triangle[2]];
		
    mesh->addTriangle(v0,v1,v2);
  }   
  return true;
}

bool ObjIO::readHeader(std::istream & is){
  std::string buf;
  // just read to the first line starting with a 'v'
  while(!is.eof() && is.peek() != 'v'){
    getline(is, buf);
    //std::cerr << "\"" << buf << "\"\n";
  }
  if (is.good())
    return true;
  else
    return false;
}

bool ObjIO::readData(std::istream & is){
  std::string lineBuf;
  int c;
  int i=0;
  while(!is.eof()){
    c = is.peek();
    switch (c) {
    case 'V':
    case 'v':{
      std::string startBuf;
      is >> startBuf; // get the start of the line
      getline(is, lineBuf); // get the rest of the line
      if(startBuf == "v"){
	loadData.verts.push_back(Vector3<float>(lineBuf));
      } else if (startBuf == "vn"){
	loadData.hasNormals = true;
	loadData.normals.push_back(Vector3<float>(lineBuf));
      }
    }
      break;
    case 'F':
    case 'f':
      {
        std::stringstream buf;
	is.get(*buf.rdbuf(), '\n'); // read a line into buf
	is.get(); // read the not extracted \n
	buf << "\n"; // and add it to the string stream

	std::string tmp;
	buf >> tmp; // get the first f or F (+ whitespace)

	// count the number of faces, delimited by whitespace
        int count = 0;
        while (buf >> tmp){
          count++;
        }
	// reset stream
	buf.clear();
	buf.seekg(0, std::ios::beg);
                
        // Determine wheter we have a triangle or a quad
        if (count == 3){
          loadData.tris.push_back(readTri(buf));
        }
        else if (count == 4){
          // Assume quad
          Vector3<unsigned int> tri1, tri2;
          splitQuad(buf, tri1, tri2);
          loadData.tris.push_back(tri1);
          loadData.tris.push_back(tri2);
        }
	else {
	  std::cerr << "Encountered polygon with " << count << " faces. Bailing out.\n";
	  return false;
	}
      }
      break;
    default: 
      // otherwise just skip the row
      getline(is, lineBuf); 
      // output it so we see what we miss :)
      // std::cerr << "\"" << lineBuf << "\"\n";
      break;
    }
    i++;
  }
  return true;
}

Vector3<unsigned int> ObjIO::readTri(std::istream &is){
  //  This is a simplified version of an obj reader that can't read normal and texture indices
  std::string buf, v;
  is >> buf;
  assert(buf == "f" || buf=="F");

  getline(is, v); // read indices
  return Vector3<unsigned int>(v)-1; // obj file format is 1-based
}
  
void ObjIO::splitQuad(std::istream &is, Vector3<unsigned int>& tri1, Vector3<unsigned int>& tri2)
{
  //  This is a simplified version of an obj reader that can't read normal and texture indices
  std::string buf;
  unsigned int indices[4];
  
  is >> buf;
  assert(buf == "f" || buf == "F");
  // only supports files where normal indices are identical to vert indices
  for (int i = 0; i < 4; i++){
    is >> indices[i];  
  }  
  // Add code to split the quad here (indices[4] contains the 4 vertex indices)


}


