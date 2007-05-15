#ifndef __SVD_H__
#define __SVD_H__

#include "Matrix4x4.h"

class SVD{
  public:
    //typedef float float;
    SVD();
    ~SVD();

    void computeSVD() {};

    protected:
      template<typename T> T sign(T a);

      float pythag(float a, float b);
      void svdcmp(float **a, int m, int n, float *w, float **v);

	  inline float sqr(const float& a){ return (a*a);};
};


#endif

