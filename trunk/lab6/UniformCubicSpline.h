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
#ifndef __uniform_cubic_spline_h__
#define __uniform_cubic_spline_h__

#include "Vector3.h"
#include <vector>
#include "Geometry.h"
#include <iostream>
#include <cmath>

class UniformCubicSpline : public Geometry {
protected:
	//! The coefficients dictating the shape
	std::vector<Vector3<float> > mCoefficients;
	//! The control polygon is simply a LineStrip
	LineStrip mControlPolygon;

	//! Decides the length of the linear approximating segments used when drawing
	float mDt;

	// display information
	Vector3<float> mLineColor;
	GLfloat mLineWidth;

public:
	UniformCubicSpline(const std::vector<Vector3<float> > &joints,
		Vector3<float> lineColor = Vector3<float>(0.f,1.f,0.f),
		GLfloat lineWidth = 2.f,
		float segmentLength = .01f) : mCoefficients(joints),
		mControlPolygon(joints)
	{
		this->mLineColor = lineColor;
		this->mLineWidth = lineWidth;
		this->mDt = segmentLength;
	}

	/*! The BSpline value is calculated from one of the four cardinal BSpline
	* segments
	*/
	float getBSplineValue(int i, float t) const 
	{
		// Find the offset from spline i
		t = t - (float)i;
		if(std::abs(t) >= 2)
		{
			// if outside of support return zero
			return 0;
		} 
		else if(0 <= t && t < 1)
		{
			// get the fractional part
			float ft = t;
			return 1/6.0*(3*ft*ft*ft - 6*ft*ft + 4);
		}
		else if(1 <= t && t < 2)
		{
			// get the fractional part
			float ft = t - 1;
			return 1/6.0*(-ft*ft*ft + 3*ft*ft - 3*ft + 1);
		}
		else if(-1 <= t && t < 0)
		{
			// get the fractional part
			float ft = t + 1;
			return 1/6.0*(-3*ft*ft*ft + 3*ft*ft + 3*ft + 1);
		}
		else if(-2 < t && t < -1)
		{
			// get the fractional part
			float ft = t + 2;
			return 1/6.0*(ft*ft*ft);
		}
		return 0;
	}

	inline Vector3<float> evaluateBSpline(float t) const
	{
		int knot = (int)t;
		Vector3<float> coef, sum;
		float bSplineVal;

		// iterate from max(0, knot) 
		int i = std::max(knot - 2, 0);
		for (; i < knot+3; i++)
		{
			coef = mCoefficients[ i ];
			bSplineVal = getBSplineValue(i, t);
			
			//printf("bSplineValue: %f\n", bSplineVal);

			Vector3<float> vr = coef * bSplineVal;
			//printf("Val: [%f, %f, %f] \n", vr.x(), vr.y(), vr.z());

			sum += coef * bSplineVal;
		}

		return sum;
	}

	/*! Evaluate the spline as the sum of the coefficients times the bsplines */
	Vector3<float> getValue(float t) const 
	{
		// get the corresponding knot
		int knot = (int)t;
		Vector3<float> coef, sum;
		float bSplineVal;

		// iterate from max(0, knot) 
		int i = std::max(knot - 2, 0);
		for (; i < knot+3; i++)
		{
			coef = mCoefficients[ i ];
			bSplineVal = getBSplineValue(i, t);

			Vector3<float> vr = coef * bSplineVal;
			sum += coef * bSplineVal;
		}

		return sum;

		//////////////////////////////////////////////////////////////////////////
		// OLD VERSION
		//////////////////////////////////////////////////////////////////////////
		//Vector3<float> v = evaluateBSpline( t );
		//printf("--------------------\n");

		//Vector3<float> val;
		//float sum = 0;
		//bool outOfRange = true;
		//for(std::vector<Vector3<float> >::const_iterator it(mCoefficients.begin());
		//	it!=mCoefficients.end();
		//	it++)
		//{
		//	// it - begin() is gives the distance between the two iterators which
		//	// can be used as a linear index
		//	float bval = getBSplineValue(it-mCoefficients.begin(), t);

		//	if(0!=bval && outOfRange )
		//	{
		//		outOfRange=false;
		//	}

		//	//If a valid number already got, and now reached 0
		//	if( !outOfRange && 0.0f == bval )
		//	{
		//		break;
		//	}

		//	Vector3<float> vr = (*it)*bval;
		//	printf("Val: [%f, %f, %f] \n", vr.x(), vr.y(), vr.z());

		//	val += (*it)*bval;
		//	sum += bval;
		//}
		//printf("--------------------\n");
		//return val;
	}

	virtual void draw(){

		mControlPolygon.draw();

		// save line point and color states
		glPushAttrib(GL_POINT_BIT | GL_LINE_BIT | GL_CURRENT_BIT);

		// draw segments
		glLineWidth(mLineWidth);
		glColor3fv(mLineColor.getArrayPtr());
		glBegin(GL_LINE_STRIP);

		// We only have full BSpline support from spline at index 1, thus we begin evaluating at 1.0
		for(float i = 1; i < mCoefficients.size()-2; i+= mDt)
		{
			glVertex3fv( this->getValue(i).getArrayPtr() );
		}
		glEnd();

		// restore attribs
		glPopAttrib();

	}
};
#endif
