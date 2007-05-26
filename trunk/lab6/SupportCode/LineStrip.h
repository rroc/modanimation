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
#ifndef __line_strip_h__
#define __line_strip_h__

#include "Geometry.h"
#include "Vector3.h"
#include <vector>
#include <iostream>

class LineStrip : public Geometry{
protected:
  std::vector<Vector3<float> > mJoints;

  // display information
  Vector3<float> mJointColor;
  Vector3<float> mLineColor;
  GLfloat mJointSize;
  GLfloat mLineWidth;

public:
  LineStrip(const std::vector<Vector3<float> > &joints) : mJoints(joints) {
    mJointColor = Vector3<float>(1, 0, 0);
    mLineColor = Vector3<float>(0, 0, 1);
    mLineWidth = 3;
    mJointSize = 5;
  }

  virtual void draw(){

    // save line point and color states
    glPushAttrib(GL_POINT_BIT | GL_LINE_BIT | GL_CURRENT_BIT);

    // draw joints
    glPointSize(mJointSize);
    glColor3fv(mJointColor.getArrayPtr());
    glBegin(GL_POINTS);
    for(std::vector<Vector3<float> >::const_iterator it(mJoints.begin());
	it!=mJoints.end();
	it++){
      glVertex3fv((*it).getArrayPtr());
    }
    glEnd();

    // draw segments
    glLineWidth(mLineWidth);
    glColor3fv(mLineColor.getArrayPtr());
    glBegin(GL_LINE_STRIP);
    for(std::vector<Vector3<float> >::const_iterator it(mJoints.begin());
	it!=mJoints.end();
	it++){
      glVertex3fv((*it).getArrayPtr());
    }
    glEnd();

    // restore attribs
    glPopAttrib();

  }
};
#endif
