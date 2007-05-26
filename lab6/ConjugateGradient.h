/*************************************************************************************************
*
*
*
*
*************************************************************************************************
* Contributors:
*                  1) IML++
*                  2) Ken Museth (ken.museth@itn.liu.se)
*                  3) Andreas Söderström (andreas.soderstrom@itn.liu.se)
*                  3) Ola Nilsson (ola.nilsson@itn.liu.se)
*************************************************************************************************/

#ifndef __conjugate_gradient_h__
#define __conjugate_gradient_h__



/*! \brief Conjugate gradient iterative solver.
*
*  Taken from Sparselib++ modified and made into class.
*  Uses no preconditioning.
*/


#include "Volume.h"
#include "VolumeLevelSet.h"
template <class Matrix, class Vector, typename Real>
class ConjugateGradient{
	unsigned int mMaxIter, mIter;
	Real mMaxTolerance, mTolerance;
	Vector p;
	Vector q;
	Vector r;

public:
	ConjugateGradient(unsigned int maxiter, Real tolerance, Volume<float>* pVol, Volume<float>* qVol, Volume<float>* rVol, VolumeLevelSet* volLS)
		: mMaxIter(maxiter), mIter(0), mMaxTolerance(tolerance), mTolerance(-1)
		{
		p.buildVector(pVol, volLS);
		q.buildVector(qVol, volLS);
		r.buildVector(rVol, volLS);
		}
	unsigned int getNumIter() const { return mIter; }
	unsigned int getMaxNumIter() const { return mMaxIter; }
	Real getTolerance() const { return mTolerance; }
	Real getMaxTolerance() const { return mMaxTolerance; }
	/*! The actual algorithm, returns a boolean indicating whether the method converged
	* to the given threshold within the maximum allowed iterations.
	* Use get/set methods above to query statistics.
	*/
	bool solve(const Matrix &A, Vector& x, const Vector &b){
		Real resid;

		Real alpha, beta, rho, rho_1 = 0.0;
		unsigned int max_iter = mMaxIter;
		Real tol = mMaxTolerance;

		Real normb = norm(b);

		//r = b - A*x; // N + M*N*fill
		a_equals_b_minus_C_times_d(r,b,A,x);

		if (normb == 0.0)
			normb = 1;
		if ((resid = norm(r) / normb) <= tol) { // N
			mTolerance = resid;
			mIter = 0;
			return true;
			}

		for (unsigned int i = 1; i <= max_iter; i++) {
			rho = dot(r, r); // N
			if (i == 1)
				{
				//p = r; // N
				a_equals_b(p,r);
				}
			else {
				beta = rho / rho_1;

				//p = r + beta * p; // N + N + N
				a_equals_b_plus_c_times_d(p,r,beta,p);
				}

			//q = A*p; // N + M*N*fill
			a_equals_B_times_c(q,A,p);

			alpha = rho / dot(p, q); // N

			//x += alpha * p; // N + N
			//printf("Alpha: %e\n", alpha);
			//printf("Normb: %e\n", normb);
			a_equals_a_plus_b_times_c(x,alpha,p);

			//r -= alpha * q; // N + N
			a_equals_a_plus_b_times_c(r,-alpha,q);

			if ((resid = norm(r) / normb) <= tol) { // N
				mTolerance = resid;
				mIter = i;

				return true;
				}
			rho_1 = rho;

			//printf("Resid: %f \n", resid);
			//system("pause");
			}

		mTolerance = resid;
		mIter = mMaxIter;
		return false;
		}
	};

#endif
