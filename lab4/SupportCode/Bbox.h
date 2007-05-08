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
#ifndef __bbox_h__
#define __bbox_h__

#include "Vector3.h"
#include "Matrix4x4.h"
#include <iostream>
#include <algorithm>

struct Bbox
{
  Vector3<float> pMin, pMax;

  //! Default constructor initializes to 'zero' cube
  Bbox(const Vector3<float> &pmin=Vector3<float>(0,0,0), const Vector3<float> & pmax=Vector3<float>(0,0,0))
    : pMin(pmin), pMax(pmax) { }

  friend std::ostream&operator << (std::ostream& os, const Bbox& b )
  {
    os << b.pMin << " -> " << b.pMax;
    return os;
  }

  //! Union of two Bbox :es
  friend Bbox boxUnion(const Bbox& b1, const Bbox &b2)
  {
    Bbox b;
    b.pMin.x() = std::min(b1.pMin.x(), b2.pMin.x());
    b.pMin.y() = std::min(b1.pMin.y(), b2.pMin.y());
    b.pMin.z() = std::min(b1.pMin.z(), b2.pMin.z());

    b.pMax.x() = std::max(b1.pMax.x(), b2.pMax.x());
    b.pMax.y() = std::max(b1.pMax.y(), b2.pMax.y());
    b.pMax.z() = std::max(b1.pMax.z(), b2.pMax.z());
    return b;
  }

  friend Bbox pointUnion(const Bbox& b, const Vector3<float> & v)
  {
    Bbox b2;
    b2.pMin.x() = std::min(b.pMin.x(), v.x());
    b2.pMin.y() = std::min(b.pMin.y(), v.y());
    b2.pMin.z() = std::min(b.pMin.z(), v.z());

    b2.pMax.x() = std::max(b.pMax.x(), v.x());
    b2.pMax.y() = std::max(b.pMax.y(), v.y());
    b2.pMax.z() = std::max(b.pMax.z(), v.z());
    return b2;
  }

  //! Intersection of two Bbox :es
  friend Bbox boxIntersection(const Bbox& b1, const Bbox &b2)
	  {
	  Bbox b;
	  b.pMin.x() = std::min(b1.pMin.x(), b2.pMin.x());
	  b.pMin.y() = std::min(b1.pMin.y(), b2.pMin.y());
	  b.pMin.z() = std::min(b1.pMin.z(), b2.pMin.z());

	  b.pMax.x() = std::max(b1.pMax.x(), b2.pMax.x());
	  b.pMax.y() = std::max(b1.pMax.y(), b2.pMax.y());
	  b.pMax.z() = std::max(b1.pMax.z(), b2.pMax.z());
	  return b;
	  }

  friend Bbox pointIntersection(const Bbox& b, const Vector3<float> & v)
	  {
	  Bbox b2;
	  if (b.pMin == 0 && b.pMax == 0) {
		  b2.pMin = b2.pMax = v;
		  }

	  b2.pMin.x() = std::max(b.pMin.x(), v.x());
	  b2.pMin.y() = std::max(b.pMin.y(), v.y());
	  b2.pMin.z() = std::max(b.pMin.z(), v.z());

	  b2.pMax.x() = std::min(b.pMax.x(), v.x());
	  b2.pMax.y() = std::min(b.pMax.y(), v.y());
	  b2.pMax.z() = std::min(b.pMax.z(), v.z());
	  return b2;
	  }

  //! Difference of two Bbox :es
  friend Bbox boxDifference(const Bbox& b1, const Bbox &b2)
	  {
	  return b1;
	  }

  Bbox transform(const Matrix4x4<float> & t) const
  {
    Bbox b;
    Vector4<float> v;

    v = t * Vector4<float>(pMin.x(), pMin.y(), pMin.z(), 1.f);
    b.pMax = b.pMin = Vector3<float>(v);

    v = t * Vector4<float>(pMax.x(), pMin.y(), pMin.z(), 1.f);  b = pointUnion(b, v);
    v = t * Vector4<float>(pMax.x(), pMax.y(), pMin.z(), 1.f);  b = pointUnion(b, v);
    v = t * Vector4<float>(pMin.x(), pMax.y(), pMin.z(), 1.f);  b = pointUnion(b, v);

    v = t * Vector4<float>(pMin.x(), pMin.y(), pMax.z(), 1.f);  b = pointUnion(b, v);
    v = t * Vector4<float>(pMax.x(), pMin.y(), pMax.z(), 1.f);  b = pointUnion(b, v);
    v = t * Vector4<float>(pMax.x(), pMax.y(), pMax.z(), 1.f);  b = pointUnion(b, v);
    v = t * Vector4<float>(pMin.x(), pMax.y(), pMax.z(), 1.f);  b = pointUnion(b, v);

    return b;
  }
};

#endif
