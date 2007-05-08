
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

			return std::min( left->getValue(x,y,z), right->getValue(x,y,z) );
			}
	};


/*! \brief Intersection boolean operation */
class Intersection : public CSG_Operator
	{
	public:
		Intersection(Implicit * l, Implicit * r) : CSG_Operator(l, r) {
			// Compute the resulting (axis aligned) bounding box from
			// the left and right children
			mBox = boxIntersection(l->getBoundingBox(), r->getBoundingBox());
			}

		virtual float getValue(float x, float y, float z) const {
			// Get values from left and right children and perform the
			// boolean operation. The coordinates (x,y,z) are passed in
			// from the world space, remember to transform them into
			// object space (Hint: Implicit::transformWorld2Obj())

			return std::max( left->getValue(x,y,z), right->getValue(x,y,z) );
			}
	};

/*! \brief Difference boolean operation */
class Difference : public CSG_Operator
	{
	public:
		Difference(Implicit * l, Implicit * r) : CSG_Operator(l, r) {
			// Compute the resulting (axis aligned) bounding box from
			// the left and right children
			mBox = boxDifference(l->getBoundingBox(), r->getBoundingBox());
			} 

		virtual float getValue(float x, float y, float z) const {
			// Get values from left and right children and perform the
			// boolean operation. The coordinates (x,y,z) are passed in
			// from the world space, remember to transform them into
			// object space (Hint: Implicit::transformWorld2Obj())

			return std::max( left->getValue(x,y,z), -(right->getValue(x,y,z)) );
			}
	};

//Blended
/*! \brief Union boolean operation */
class BlendedUnion : public CSG_Operator
	{
	private:
		float p;
	public:
		BlendedUnion(Implicit * l, Implicit * r, float aP) 
			: CSG_Operator(l, r) 
			, p( aP )
			{
			// Compute the resulting (axis aligned) bounding box from
			// the left and right children
			mBox = boxUnion(l->getBoundingBox(), r->getBoundingBox());
			}

		virtual float getValue(float x, float y, float z) const {
			// Get values from left and right children and perform the
			// boolean operation. The coordinates (x,y,z) are passed in
			// from the world space, remember to transform them into
			// object space (Hint: Implicit::transformWorld2Obj())

			//return std::min( left->getValue(x,y,z), right->getValue(x,y,z) );

			float Da = exp(-left->getValue(x,y,z) * p );
			float Db = exp(-right->getValue(x,y,z)* p );
			return -log( pow( Da + Db, 1.0f/p ) );

			//float Da = exp(-left->getValue(x,y,z)  );
			//float Db = exp(-right->getValue(x,y,z) );
			//return  log( pow( pow(Da, p) + pow(Db, p), 1.0f/p ) );
			}
	};


/*! \brief BlendedIntersection boolean operation */
class BlendedIntersection : public CSG_Operator
	{
	private:
		float p;
	public:
		BlendedIntersection(Implicit * l, Implicit * r, float aP)
			: CSG_Operator(l, r) 
			, p( aP )
			{
			// Compute the resulting (axis aligned) bounding box from
			// the left and right children
			mBox = boxIntersection(l->getBoundingBox(), r->getBoundingBox());
			}

		virtual float getValue(float x, float y, float z) const {
			// Get values from left and right children and perform the
			// boolean operation. The coordinates (x,y,z) are passed in
			// from the world space, remember to transform them into
			// object space (Hint: Implicit::transformWorld2Obj())

			float Da = exp(-left->getValue(x,y,z) * -p );
			float Db = exp(-right->getValue(x,y,z)* -p );
			return -log( pow( Da + Db, 1.0f/-p ) );
			}
	};

/*! \brief BlendedDifference boolean operation */
class BlendedDifference : public CSG_Operator
	{
	private:
		float p;
	public:
		BlendedDifference(Implicit * l, Implicit * r, float aP)
			: CSG_Operator(l, r) 
			, p( aP )
			{
			// Compute the resulting (axis aligned) bounding box from
			// the left and right children
			mBox = boxDifference(l->getBoundingBox(), r->getBoundingBox());
			} 

		virtual float getValue(float x, float y, float z) const {
			// Get values from left and right children and perform the
			// boolean operation. The coordinates (x,y,z) are passed in
			// from the world space, remember to transform them into
			// object space (Hint: Implicit::transformWorld2Obj())

			float Da = exp(-left->getValue(x,y,z) * -p );
			float Db = exp( right->getValue(x,y,z)* -p );
			return -log( pow( Da + Db, 1.0f/-p ) );
			}
	};






#endif
