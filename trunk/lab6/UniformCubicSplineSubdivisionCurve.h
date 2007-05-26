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
#ifndef __uc_spline_subdiv_h__
#define __uc_spline_subdiv_h__

#include "Vector3.h"
#include <vector>
#include "Geometry.h"
#include <iostream>
#include <cmath>
#include <cassert>

class UniformCubicSplineSubdivisionCurve : public Geometry {
protected:
	//! The coefficients dictating the shape
	std::vector<Vector3<float> > mCoefficients;
	//! The control polygon is simply a LineStrip
	LineStrip mControlPolygon;

	// display information
	Vector3<float> mLineColor;
	GLfloat mLineWidth;

public:
	UniformCubicSplineSubdivisionCurve(const std::vector<Vector3<float> > &joints,
		Vector3<float> lineColor = Vector3<float>(1.f,1.f,0.2f),
		GLfloat lineWidth = 2.f) : mCoefficients(joints),
		mControlPolygon(joints){

			this->mLineColor = lineColor;
			this->mLineWidth = lineWidth;
		}

	void subdivide(unsigned int i=1)
		{
		for(unsigned int j=0; j<i; j++ )
			{
			mCoefficients = computeNewCoefficients();
			}
		}

	/*! Calculate new coefficients by means of subdivision.
	* Applies natural cubic boundary conditions.
	*/
	std::vector<Vector3<float> > computeNewCoefficients() 
		{
		// allocate space for new coefficients
		std::vector<Vector3<float> > newc;

		//note the -2 (the optimal size would be -3, but we remove the extra element caused by the padding later)
		newc.resize( mCoefficients.size()*2 ); 

		assert(mCoefficients.size() > 3 && "Need at least 4 points to subdivide");

		//Add padding to overcome boundaries
		mCoefficients.insert( mCoefficients.begin(), mCoefficients[0] );
		mCoefficients.push_back( mCoefficients.back() );

		//end before the padding
		unsigned int end = mCoefficients.size()-1;

		float oneByEight = 1.0f/8.0f;
		// Implement the subdivision scheme for a natural cubic spline here
		for(int i=1; i<end; i++)
			{
			newc[2*i-2] = oneByEight * ( mCoefficients[i-1] + 6.0f * mCoefficients[i] + mCoefficients[i+1] );
			newc[2*i-1] = oneByEight * ( 4.0f*mCoefficients[i] + 4.0f * mCoefficients[i+1] );
			}

		//remove the last element caused by the padding (otherwise we would require nasty if inside the for loop)
		newc.pop_back();

		//remove boundary padding
		mCoefficients.erase( mCoefficients.begin() );
		mCoefficients.pop_back();

		return newc;
		}


	virtual void draw()
		{

		mControlPolygon.draw();

		// save line point and color states
		glPushAttrib(GL_POINT_BIT | GL_LINE_BIT | GL_CURRENT_BIT);

		// draw segments
		glLineWidth(mLineWidth);
		glColor3fv(mLineColor.getArrayPtr());
		glBegin(GL_LINE_STRIP);
		// just draw the spline as a series of connected linear segments
		for(unsigned int i = 0; i < mCoefficients.size(); i++){
			glVertex3fv( mCoefficients.at(i).getArrayPtr() );
			}
		glEnd();

		// restore attribs
		glPopAttrib();

		}
	};
#endif
