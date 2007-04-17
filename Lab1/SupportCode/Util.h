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
#ifndef __util_h__
#define __util_h__

#include <string> // for string
#include <algorithm> //for transform

//! Convert a string to lowercase
std::string& lower(std::string & str);
//! Construct a new string without whitespace at start or end
std::string trim(std::string & str);

//! Return the sign of a value
template<typename T>
inline T sign(T value)
{
  T mysign = -(T)1.0;
  if (value >= (T)0.0){
    mysign = (T)1.0;
  }
  return mysign;
}

//! Clamp value between low and high
template <typename Scalar>
inline Scalar clamp(Scalar val, Scalar low, Scalar high)
{
  if (val < low) return low;
 else if (val > high) return high;
 else return val;
}

//! 
template <class Scalar>
inline Scalar switch1(Scalar val, Scalar x1, Scalar x2)
{
  return ( val<x1 ? 0 : ( val>x2 ? 1 : (val-x1)/(x2-x1)) );
}

#endif
