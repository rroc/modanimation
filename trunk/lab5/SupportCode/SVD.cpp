#include "SVD.h"
#include "math.h"
#include <algorithm>

using namespace std;

SVD::SVD()
{
}

SVD::~SVD()
{
}

template<typename T>
T SVD::sign(T a)
{
  if (a >= (T)0)
  {
    return (T)1;
  }
  else
  {
    return (T)(-1);
  }
}

//C++ source code from http://users.pandora.be/paul.larmuseau/SVD.htm
float SVD::pythag(float a, float b)
{
  float absa,absb;
  absa=fabs(a);
  absb=fabs(b);
  if(absa>absb)
	  return absa*sqrt(1.0+sqr(absb/absa));
  else
    return(absb==0.0)?0.0:absb*sqrt(1.0+sqr(absa/absb));
}

void SVD::svdcmp(float **a, int m, int n, float *w, float **v)
{
  float g,scale,anorm;
  float *rv1=new float [n]-1;
  g=scale=anorm=0.0;
  for(int i=1;i<=n;++i){
    int l=i+1;
    rv1[i]=scale*g;
    g=scale=0.0;
    if(i<=m){
      for(int k=i;k<=m;++k) scale+=fabs(a[k][i]);
      if(scale){
        float s=0.0;
        for(int k=i;k<=m;++k){
          a[k][i]/=scale;
          s+=a[k][i]*a[k][i];
        }
        float f=a[i][i];
        g=-sign(sqrt(s),f);
        float h=f*g-s;
        a[i][i]=f-g;
        for(int j=l;j<=n;++j){
          float sum=0.0;
          for(int k=i;k<=m;++k) sum+=a[k][i]*a[k][j];
          float fct=sum/h;
          for(int k=i;k<=m;++k) a[k][j]+=fct*a[k][i];
        }
        for(int k=i;k<=m;++k) a[k][i]*=scale;
      }
    }
    w[i]=scale*g;
    g=scale=0.0;
    if((i<=m)&&(i!=n)){
      for(int k=l;k<=n;++k) scale+=fabs(a[i][k]);
      if(scale){
        float s=0.0;
        for(int k=l;k<=n;++k){
          a[i][k]/=scale;
          s+=a[i][k]*a[i][k];
        }
        float f=a[i][l];
        g=-sign(sqrt(s),f);
        float h=f*g-s;
        a[i][l]=f-g;
        for(int k=l;k<=n;++k) rv1[k]=a[i][k]/h;
        for(int j=l;j<=m;++j){
          float sum=0.0;
          for(int k=l;k<=n;++k) sum+=a[j][k]*a[i][k];
          for(int k=l;k<=n;++k) a[j][k]+=sum*rv1[k];
        }
        for(int k=l;k<=n;++k) a[i][k]*=scale;
      }
    }
    anorm=max(anorm,(fabs(w[i])+fabs(rv1[i])));
  }
  {
    float f;
    for(int i=n,l;i>=1;--i){
      if(i<n){       // this makes f and l not dependent
        if(f){
          for(int j=l;j<=n;++j) v[j][i]=(a[i][j]/a[i][l])/f;
            for(int j=l;j<=n;++j){
            float sum=0.0;
            for(int k=l;k<=n;++k) sum+=a[i][k]*v[k][j];
            for(int k=l;k<=n;++k) v[k][j]+=sum*v[k][i];
          }
        }
        for(int j=l;j<=n;++j) v[i][j]=v[j][i]=0.0;
      }
      v[i][i]=1.0;
      f=rv1[i];
      l=i;
    }
  }
  for(int i=min(m,n);i>=1;--i){
    int l=i+1;
    g=w[i];
    for(int j=l;j<=n;++j) a[i][j]=0.0;
    if(g){
      g=1.0/g;
      for(int j=l;j<=n;++j){
        float sum=0.0;
        for(int k=l;k<=m;++k) sum+=a[k][i]*a[k][j];
        float f=(sum/a[i][i])*g;
        for(int k=i;k<=m;++k) a[k][j]+=f*a[k][i];
      }
      for(int j=i;j<=m;++j) a[j][i]*=g;
    }else for(int j=i;j<=m;++j) a[j][i]=0.0;
    ++a[i][i];
  }
  for(int k=n;k>=1;--k){
    for(int its=1;its<=30;++its){
      int flag=1,nm,l;
      for(l=k;l>=1;--l){
        nm=l-1;
        if((float)(fabs(rv1[l])+anorm)==anorm){
          flag=0;
          break;
        }
        if((float)(fabs(w[nm])+anorm)==anorm) break;
      }
      if(flag){
        float c=0.0,s=1.0;
        for(int i=l;i<=k;++i){
          float f=s*rv1[i];
          rv1[i]=c*rv1[i];
          if((float)(fabs(f)+anorm)==anorm) break;
          g=w[i];
          float h=pythag(f,g);
          w[i]=h;
          h=1.0/h;
          c=g*h;
          s=-f*h;
          for(int j=1;j<=m;++j){
            float y=a[j][nm];
            float z=a[j][i];
            a[j][nm]=y*c+z*s;
            a[j][i]=z*c-y*s;
          }
        }
      }
      float z=w[k];
      if(l==k){
        if(z<0.0){
          w[k]=-z;
          for(int j=1;j<=n;++j) v[j][k]=-v[j][k];
        }
        break;
      }
      if(its==30) exit(fprintf(stderr,"no convergence in 30 svdcmp iterations"));
      float x=w[l];
      float y=w[nm=k-1];
      g=rv1[nm];
      float h=rv1[k];
      float f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
      g=pythag(f,1.0);
      f=((x-z)*(x+z)+h*((y/(f+sign(g,f)))-h))/x;
      float c=1.0,s=1.0;
      for(int j=l;j<=nm;++j){
        int i=j+1;
        g=rv1[i];
        y=w[i];
        h=s*g;
        g*=c;
        z=pythag(f,h);
        rv1[j]=z;
        c=f/z;
        s=h/z;
        f=x*c+g*s;
        g=g*c-x*s;
        h=y*s;
        y*=c;
        for(int jj=1;jj<=n;++jj){
          x=v[jj][j];
          z=v[jj][i];
          v[jj][j]=x*c+z*s;
          v[jj][i]=z*c-x*s;
        }
        z=pythag(f,h);
        w[j]=z;
        if(z){
          z=1.0/z;
          c=f*z;
          s=h*z;
        }
        f=c*g+s*y;
        x=c*y-s*g;
        for(int jj=1;jj<=m;++jj){
          y=a[jj][j];
          z=a[jj][i];
          a[jj][j]=y*c+z*s;
          a[jj][i]=z*c-y*s;
        }
      }
      rv1[l]=0.0;
      rv1[k]=f;
      w[k]=x;
    }
  }
  delete (rv1+1);
}
