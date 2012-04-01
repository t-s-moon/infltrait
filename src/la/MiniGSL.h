/**
 * @file   mini_gsl.h
 * @author Taesun Moon <tsmoon@wireless-128-62-123-101.public.utexas.edu>
 * @date   Fri Feb  6 22:16:38 2009
 * 
 * @brief  
 * 
 * 
 */

#ifndef __MINI_GSL_H__
#define __MINI_GSL_H__

#include <cmath>

#ifdef __cplusplus
extern "C" {
#endif
#include <cblas.h>
#ifdef  __cplusplus
}
#endif

#include "la/Matrix.h"
#include "la/Vector.h"

namespace infl
{

  /** 
   * 
   * 
   * @param X 
   * 
   * @return 
   */
  double gsl_blas_dnrm2(const Vector& X);

  /** 
   * 
   * 
   * @param X 
   * 
   * @return 
   */
  double gsl_blas_dnrm2(const Matrix& X);

  /** 
   * 
   * 
   * @param X 
   * @param Y 
   * 
   * @return 
   */
  double gsl_blas_ddot (const Vector& X, const Vector& Y);

  /** 
   * 
   * 
   * @param X 
   * @param Y 
   * 
   * @return 
   */
  double gsl_blas_ddot_nofuss (const Vector& X, const Vector& Y);

  /** 
   * 
   * 
   * @param X 
   * @param Y 
   * 
   * @return 
   */
  double gsl_cos(const Vector& X, const Vector& Y);

  /** 
   * 
   * 
   * @param X 
   * @param Y 
   * 
   * @return 
   */
  double gsl_cos_nofuss(const Vector& X, const Vector& Y);

  /** 
   * 
   * 
   * @param TransA 
   * @param alpha 
   * @param A 
   * @param X 
   * @param beta 
   * @param Y 
   */
  void gsl_blas_dgemv (CBLAS_TRANSPOSE TransA, double alpha, const Matrix& A,
		       const Vector& X, double beta, Vector& Y);
  /** 
   * 
   * 
   * @param TransA 
   * @param TransB 
   * @param alpha 
   * @param A 
   * @param B 
   * @param beta 
   * @param C 
   */
  void gsl_blas_dgemm (CBLAS_TRANSPOSE TransA, CBLAS_TRANSPOSE TransB,
		       double alpha, const Matrix& A, const Matrix& B,
		       double beta, Matrix& C);

  /** 
   * 
   * 
   * @param A 
   * @param S 
   * @param U 
   * @param VT 
   */
  void svd(Matrix* A, Matrix* S, Matrix* U, Matrix* VT);
}

#endif
