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
#ifndef _matrix4x4_impl_h
#define _matrix4x4_impl_h

#include <cmath>
#include "Vector4.h"
#include <algorithm>

template <typename Real> Matrix4x4<Real> Matrix4x4<Real>::identityMatrix = Matrix4x4<Real>(); 

template <typename Real>
Matrix4x4<Real>::Matrix4x4()
{
  m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
  m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
  m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
  m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
}


template <typename Real>
Matrix4x4<Real>::Matrix4x4(const Real m[4][4])
{
  this->m[0][0] = m[0][0]; this->m[0][1] = m[0][1]; this->m[0][2] = m[0][2]; this->m[0][3] = m[0][3];
  this->m[1][0] = m[1][0]; this->m[1][1] = m[1][1]; this->m[1][2] = m[1][2]; this->m[1][3] = m[1][3];
  this->m[2][0] = m[2][0]; this->m[2][1] = m[2][1]; this->m[2][2] = m[2][2]; this->m[2][3] = m[2][3];
  this->m[3][0] = m[3][0]; this->m[3][1] = m[3][1]; this->m[3][2] = m[3][2]; this->m[3][3] = m[3][3];
}


template <typename Real>
Matrix4x4<Real> Matrix4x4<Real>::identity()
{
  return identityMatrix;
}


template <typename Real>
Vector4<Real> Matrix4x4<Real>::operator*(const Vector4<Real>& vec4) const
{
  Vector4<Real> res;

  res[0] = vec4[0] * m[0][0] + vec4[1] * m[0][1] + vec4[2] * m[0][2] + vec4[3] * m[0][3];
  res[1] = vec4[0] * m[1][0] + vec4[1] * m[1][1] + vec4[2] * m[1][2] + vec4[3] * m[1][3];
  res[2] = vec4[0] * m[2][0] + vec4[1] * m[2][1] + vec4[2] * m[2][2] + vec4[3] * m[2][3];
  res[3] = vec4[0] * m[3][0] + vec4[1] * m[3][1] + vec4[2] * m[3][2] + vec4[3] * m[3][3];

  return res;
}


template <typename Real>
Matrix4x4<Real> Matrix4x4<Real>::operator*(const Matrix4x4& m2) const
{
  Matrix4x4 res;
  unsigned int i, j, k;

  for (i=0; i<4; i++)
    {
      for (j=0; j<4; j++)
	{
	  res.m[i][j] = Real(0.0);
	  for (k=0; k<4; k++)
	    {
	      res.m[i][j] += m[i][k] * m2.m[k][j];
	    }
	}
    }

  return res;
}


// Input: i (row), j (column)
template <typename Real>
Real &Matrix4x4<Real>::operator()(unsigned int i, unsigned int j)
{
  return m[i][j];
}


template <typename Real>
Matrix4x4<Real> Matrix4x4<Real>::inverse() const
{
  Matrix4x4 a = Matrix4x4(m);  // the inverse
  Matrix4x4 b = identity();
		

  /////////////////////////////////////////////////////////////////
  // Taken from Numerical Recipes in C++;
  // void NR::gaussj(Mat_IO_DP &a, Mat_IO_DP &b)

  int i,icol,irow,j,k,l,ll;
  Real big,dum,pivinv,temp;

  //int n=a.nrows();
  int n = 4;
  //int m=b.ncols();
  int m = 4;
  //Vec_INT indxc(n),indxr(n),ipiv(n);
  Vector4<int> indxc, indxr, ipiv;
  for (j=0;j<n;j++) ipiv[j]=0;
  for (i=0;i<n;i++) {
    big=0.0;
    for (j=0;j<n;j++)
      if (ipiv[j] != 1)
	for (k=0;k<n;k++) {
	  if (ipiv[k] == 0) {
	    if (fabs(a[j][k]) >= big) {
	      big=fabs(a[j][k]);
	      irow=j;
	      icol=k;
	    }
	  }
	}
    ++(ipiv[icol]);
    if (irow != icol) {
      for (l=0;l<n;l++) std::swap(a[irow][l],a[icol][l]);
      for (l=0;l<m;l++) std::swap(b[irow][l],b[icol][l]);
    }
    indxr[i]=irow;
    indxc[i]=icol;
    //if (a[icol][icol] == 0.0) nrerror("gaussj: Singular Matrix");
    pivinv=1.0/a[icol][icol];
    a[icol][icol]=1.0;
    for (l=0;l<n;l++) a[icol][l] *= pivinv;
    for (l=0;l<m;l++) b[icol][l] *= pivinv;
    for (ll=0;ll<n;ll++)
      if (ll != icol) {
	dum=a[ll][icol];
	a[ll][icol]=0.0;
	for (l=0;l<n;l++) a[ll][l] -= a[icol][l]*dum;
	for (l=0;l<m;l++) b[ll][l] -= b[icol][l]*dum;
      }
  }
  for (l=n-1;l>=0;l--) {
    if (indxr[l] != indxc[l])
      for (k=0;k<n;k++)
	std::swap(a[k][indxr[l]],a[k][indxc[l]]);
  }
  /////////////////////////////////////////////////////////////////


  return a;
}



// Static methods

template <typename Real>
Matrix4x4<Real> Matrix4x4<Real>::scale(Real sx, Real sy, Real sz)
{
  Real m[4][4];

  m[0][0] = sx; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
  m[1][0] = 0; m[1][1] = sy; m[1][2] = 0; m[1][3] = 0;
  m[2][0] = 0; m[2][1] = 0; m[2][2] = sz; m[2][3] = 0;
  m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;

  return Matrix4x4(m);
}


template <typename Real>
Matrix4x4<Real> Matrix4x4<Real>::scale(Real scale)
{
  Real m[4][4];

  m[0][0] = scale; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
  m[1][0] = 0; m[1][1] = scale; m[1][2] = 0; m[1][3] = 0;
  m[2][0] = 0; m[2][1] = 0; m[2][2] = scale; m[2][3] = 0;
  m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;

  return Matrix4x4(m);
}

template <typename Real>
Matrix4x4<Real> Matrix4x4<Real>::rotationXYZ(Real rx, Real ry, Real rz)
{
  Real m[4][4];

  m[0][0] = cos(ry) * cos(rz); 
  m[0][1] = cos(ry) * sin(rz); 
  m[0][2] = -sin(ry); 
  m[0][3] = 0;
  m[1][0] = sin(rx) * sin(ry) * cos(rz) - cos(rx) * sin(rz); 
  m[1][1] = sin(rx) * sin(ry) * sin(rz) + cos(rx) * cos(rz); 
  m[1][2] = cos(ry) * sin(rx); 
  m[1][3] = 0;
  m[2][0] = cos(rx) * sin(ry) * cos(rz) + sin(rx) * sin(rz);
  m[2][1] = cos(rx) * sin(ry) * sin(rz) - sin(rx) * cos(rz); 
  m[2][2] = cos(ry) * cos(rx); 
  m[2][3] = 0;
  m[3][0] = 0; 
  m[3][1] = 0; 
  m[3][2] = 0; 
  m[3][3] = 1;

  return Matrix4x4(m);
}

template <typename Real>
Matrix4x4<Real> Matrix4x4<Real>::translation(Real tx, Real ty, Real tz)
{
  Real m[4][4];

  m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = tx;
  m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = ty;
  m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = tz;
  m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;

  return Matrix4x4(m);
}

template <typename Real>
void Matrix4x4<Real>::toGLMatrix(float glMatrix[16]) const
{
  int glIndex = 0;
  for (int i = 0; i < 4; i++){
    for (int j = 0; j < 4; j++){
      glMatrix[glIndex] = m[j][i]; 
      glIndex++;
    }
  }	
}

#endif
