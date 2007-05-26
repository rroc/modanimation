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
                                     Vector3<float> lineColor = Vector3<float>(0.f,1.f,0.2f),
                                     GLfloat lineWidth = 2.f) : mCoefficients(joints),
                                                                mControlPolygon(joints){

    this->mLineColor = lineColor;
    this->mLineWidth = lineWidth;
  }

  void subdivide(unsigned int i=1){
    for(unsigned int j=0; j<i; j++ )
      {
        mCoefficients = computeNewCoefficients();
      }
  }

  /*! Calculate new coefficients by means of subdivision.
   * Applies natural cubic boundary conditions.
   */
  std::vector<Vector3<float> > computeNewCoefficients() const {
    // allocate space for new coefficients
    std::vector<Vector3<float> > newc;
    newc.reserve(mCoefficients.size()*2);

    unsigned int end = mCoefficients.size()-1;

    assert(mCoefficients.size() > 3 && "Need at least 4 points to subdivide");

    // Implement the subdivision scheme for a natural cubic spline here

    return newc;
  }


  virtual void draw(){

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
