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
#ifndef __camera_h__
#define __camera_h__

#include "Vector3.h"

class Camera{
private:
  Vector3<float> mCameraAngle;  //spherical coordinates (r, phi, theta)
  Vector3<float> mCameraPos;
  Vector3<float> mCameraVel; // observer velocity
  Vector3<float> mCameraAcc; // observer acceleration
  Vector3<float> mLastCameraPos;
  float mAcc; // acc. scaling
public:
  Camera(Vector3<float> v=Vector3<float>(2, 1, -.1), float accScaling=.5){
    mCameraPos = v;
    mLastCameraPos = v;
    mCameraVel = Vector3<float>(0,0,0);
    mCameraAcc = Vector3<float>(0,0,0);
    mCameraVel = Vector3<float>(0,0,0);
    Vector3<float> direction = -mCameraPos;
    float r = sqrt(direction.x()*direction.x() + direction.y()*direction.y() + direction.z()*direction.z());
    float phi = sign(direction.x())*M_PI/2 - atan(direction.z()/direction.x());
    float theta = acos(direction.y()/r);
    mCameraAngle = Vector3<float>(1.0, phi/M_PI, theta/M_PI);
    mAcc = accScaling;
  }
  Vector3<float> getPosition() const { return mCameraPos; }
  Vector3<float> getLookAtVector() const{
    float x,y,z,theta,phi, r;
    r = mCameraAngle[0];
    phi = mCameraAngle[1];
    theta = mCameraAngle[2];
    x = sin(M_PI*theta)*sin(M_PI*phi);
    y = cos(M_PI*theta);
    z = sin(M_PI*theta)*cos(M_PI*phi);
    return Vector3<float>(x, y, z);
  }
  Vector3<float> getSideVector() const{
    return Vector3<float>(cross(getLookAtVector(), Vector3<float>(0,1.0,0)));
  }
  Vector3<float> getUpVector() const{
    return Vector3<float>(cross(getSideVector(), getLookAtVector()));
  }
  Vector3<float> getLookAtPoint() const{
    return Vector3<float>(mCameraPos + getLookAtVector());
  }
  bool moving() const {
    return (mLastCameraPos == mCameraPos);
  }
  void advect(float dt){
    mLastCameraPos = mCameraPos;
    mCameraVel += mCameraAcc*dt;
    mCameraPos += mCameraVel*dt;
  }
  void accelerate(Vector3<float> v){
    mCameraAcc += v*mAcc;
  }
  void rotateXY(float xr, float yr){
    // move direction angles
    mCameraAngle[1] += xr;
    mCameraAngle[2] = clamp(mCameraAngle[2]+=yr, .0001f, .9999f);
  }
  void stopAcc(){
    mCameraAcc = 0;
  }
  void stop(){
    mCameraVel = 0;
    mCameraAcc = 0;
  }
  void accUp(){
    accelerate(getUpVector());
  }
  void accDown(){
    accelerate(-getUpVector());
  }
  void accForward(){
    accelerate(getLookAtVector());
  }
  void accBackward(){
    accelerate(-getLookAtVector());
  }
  void accLeft(){
    accelerate(-getSideVector());
  }
  void accRight(){
    accelerate(getSideVector());
  }
  void lookAtOrigo(){
    Vector3<float> direction = -mCameraPos;
    float r = direction.length();
    float phi = sign(direction.x())*M_PI/2 - atan(direction.z()/direction.x());
    float theta = acos(direction.y()/r);
    mCameraAngle = Vector3<float>(1.0, phi/M_PI, theta/M_PI);
  }
  void reset(){
    *this = Camera();
  }
  void dolly(float r){
    mCameraPos += getLookAtVector()*r;
  }
};

#endif
