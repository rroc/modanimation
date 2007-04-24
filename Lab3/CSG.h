
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
#ifndef __CSG_H__
#define __CSG_H__

#include "Implicit.h"


/*! \brief CSG Operator base class */
class CSG_Operator : public Implicit
{

protected:
  //! Constructor
  CSG_Operator(Implicit * l, Implicit * r) : left(l), right(r) { }

  //! Pointers to left and right child nodes
  Implicit * left, * right;
};


/*! \brief Union boolean operation */
class Union : public CSG_Operator
{
public:
  Union(Implicit * l, Implicit * r) : CSG_Operator(l, r) {
    // Compute the resulting (axis aligned) bounding box from
    // the left and right children
    mBox = boxUnion(l->getBoundingBox(), r->getBoundingBox());
  }

  virtual float getValue(float x, float y, float z) const {
    // Get values from left and right children and perform the
    // boolean operation. The coordinates (x,y,z) are passed in
    // from the world space, remember to transform them into
    // object space (Hint: Implicit::transformWorld2Obj())
    return 0;
  }
};


// Add more boolean operations here.

#endif
