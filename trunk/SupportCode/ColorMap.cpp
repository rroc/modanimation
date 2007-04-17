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
#include "ColorMap.h"

//-----------------------------------------------------------------------------
ColorMap::ColorMap()
{
  /* % A matlab script to export colormaps
     function f=pcmap(cmap)
       s = size(cmap);
       for i=1:s(1,1)
         fprintf(1, 'colors.push_back(Vector3<float>(%f, %f, %f));\n', cmap(i,1), cmap(i,2), cmap(i,3));
       end
  */
  // run the above script with for example the colormap jet: pcmap(jet)
  // and paste the result below to change colormap
  
  // A simple red-black-blue colormap
  colors.push_back(Vector3<float>(1, 0, 0));
  colors.push_back(Vector3<float>(0, 0, 0));
  colors.push_back(Vector3<float>(1, 0, 1));
}
//-----------------------------------------------------------------------------
Vector3<float> ColorMap::map(float val, float low, float high) const {
  float h = switch1(val, low, high);
  h = clamp(h, 0.f, 1.f);
  float pos = h*(colors.size()-1);
  float t  = pos - floorf(pos);
  unsigned int index = (unsigned int) floorf(pos);
  // linear interpolation
  return colors[index]*(1-t) + colors[index+1]*t;
}
//-----------------------------------------------------------------------------
Vector3<float> ColorMap::map(Vector3<float> vec, float low, float high) const {
  return Vector3<float>(switch1(vec.x(), low, high),switch1(vec.y(), low, high),switch1(vec.z(), low, high));
}
//-----------------------------------------------------------------------------
