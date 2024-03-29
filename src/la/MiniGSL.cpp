/**
 * @file   mini_gsl.cpp
 * @author Taesun Moon <tsmoon@wireless-128-62-123-101.public.utexas.edu>
 * @date   Fri Feb  6 22:16:08 2009
 * 
 * @brief  
 * 
 * 
 */

#include "la/MiniGSL.h"

namespace infl {

  double gsl_blas_dnrm2(const Vector& X) {
    return cblas_dnrm2(X.size, X.data, X.stride);
  }

  double gsl_blas_dnrm2(const Matrix& X) {
    return cblas_dnrm2(X.m * X.n, X.data, 1);
  }

  double
  gsl_blas_ddot (const Vector& X, const Vector& Y)
  {
    if (X.size == Y.size && X.order == CblasRowMajor && Y.order == CblasColMajor) {
      return cblas_ddot (X.size, X.data, X.stride, Y.data,
		  Y.stride);
    } else {
      throw 0;
    }
  }

  double
  gsl_blas_ddot_nofuss (const Vector& X, const Vector& Y)
  {
    return cblas_ddot (X.size, X.data, X.stride, Y.data,
		  Y.stride);
  }

  double gsl_cos(const Vector& X, const Vector& Y) {
    return abs(gsl_blas_ddot(X,Y))
      / (gsl_blas_dnrm2(X) * gsl_blas_dnrm2(Y));
  }

  double gsl_cos_nofuss(const Vector& X, const Vector& Y) {
#ifdef MDEBUG
    std::cerr << "NOFUSS DOT: " << gsl_blas_ddot_nofuss(X, Y) << std::endl;
    std::cerr << "X DNORM: " << gsl_blas_dnrm2(X) << std::endl;
    std::cerr << "Y DNORM: " << gsl_blas_dnrm2(Y) << std::endl;
#endif
    return gsl_blas_ddot_nofuss(X, Y)
      / (gsl_blas_dnrm2(X) * gsl_blas_dnrm2(Y));
  }

  void
  gsl_blas_dgemv (CBLAS_TRANSPOSE TransA, double alpha, const Matrix& A,
		  const Vector& X, double beta, Vector& Y)
  {
    const unsigned int M = A.m;
    const unsigned int N = A.n;

    if ((TransA == CblasNoTrans && N == X.size && M == Y.size)
	|| (TransA == CblasTrans && M == X.size && N == Y.size)) {
      cblas_dgemv (CblasColMajor, TransA, M, N, alpha, A.data,
		   A.ld, X.data, X.stride, beta, Y.data,
		   Y.stride);
    } else {
      throw 0;
    }
  }

  void 
  gsl_blas_dgemm (CBLAS_TRANSPOSE TransA, CBLAS_TRANSPOSE TransB,
		  double alpha, const Matrix& A, const Matrix& B,
		  double beta, Matrix& C)
  {
    const int M = C.m;
    const int N = C.n;
    const int MA = (TransA == CblasNoTrans) ? A.m : A.n;
    const int NA = (TransA == CblasNoTrans) ? A.n : A.m;
    const int MB = (TransB == CblasNoTrans) ? B.m : B.n;
    const int NB = (TransB == CblasNoTrans) ? B.n : B.m;

    if (M == MA && N == NB && NA == MB) {  /* [MxN] = [MAxNA][MBxNB] */
      cblas_dgemm (CblasColMajor, TransA, TransB,  M,  N,  NA,
		   alpha, A.data,  A.ld, B.data,  B.ld, beta,
		   C.data, C.ld);
    } else {
      throw 0;
    }
  }

  /*
   *  DGESDD computes the singular value decomposition (SVD) of a real
   *  M-by-N matrix A, optionally computing the left and right singular
   *  vectors.  If singular vectors are desired, it uses a
   *  divide-and-conquer algorithm.
   *
   *  The SVD is written
   *
   *       A = U * SIGMA * transpose(V)
   *
   *  where SIGMA is an M-by-N matrix which is zero except for its
   *  min(m,n) diagonal elements, U is an M-by-M orthogonal matrix, and
   *  V is an N-by-N orthogonal matrix.  The diagonal elements of SIGMA
   *  are the singular values of A; they are real and non-negative, and
   *  are returned in descending order.  The first min(m,n) columns of
   *  U and V are the left and right singular vectors of A.
   *
   *  Note that the routine returns VT = V**T, not V.
   *
   *  The divide and conquer algorithm makes very mild assumptions about
   *  floating point arithmetic. It will work on machines with a guard
   *  digit in add/subtract, or on those binary machines without guard
   *  digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or
   *  Cray-2. It could conceivably fail on hexadecimal or decimal machines
   *  without guard digits, but we know of none.
   *
   *  Arguments
   *  =========
   *
   *  JOBZ    (input) CHARACTER*1
   *          Specifies options for computing all or part of the matrix U:
   *          = 'A':  all M columns of U and all N rows of V**T are
   *                  returned in the arrays U and VT;
   *          = 'S':  the first min(M,N) columns of U and the first
   *                  min(M,N) rows of V**T are returned in the arrays U
   *                  and VT;
   *          = 'O':  If M >= N, the first N columns of U are overwritten
   *                  on the array A and all rows of V**T are returned in
   *                  the array VT;
   *                  otherwise, all columns of U are returned in the
   *                  array U and the first M rows of V**T are overwritten
   *                  in the array A;
   *          = 'N':  no columns of U or rows of V**T are computed.
   *
   *  M       (input) INTEGER
   *          The number of rows of the input matrix A.  M >= 0.
   *
   *  N       (input) INTEGER
   *          The number of columns of the input matrix A.  N >= 0.
   *
   *  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N)
   *          On entry, the M-by-N matrix A.
   *          On exit,
   *          if JOBZ = 'O',  A is overwritten with the first N columns
   *                          of U (the left singular vectors, stored
   *                          columnwise) if M >= N;
   *                          A is overwritten with the first M rows
   *                          of V**T (the right singular vectors, stored
   *                          rowwise) otherwise.
   *          if JOBZ .ne. 'O', the contents of A are destroyed.
   *
   *  LDA     (input) INTEGER
   *          The leading dimension of the array A.  LDA >= max(1,M).
   *
   *  S       (output) DOUBLE PRECISION array, dimension (min(M,N))
   *          The singular values of A, sorted so that S(i) >= S(i+1).
   *
   *  U       (output) DOUBLE PRECISION array, dimension (LDU,UCOL)
   *          UCOL = M if JOBZ = 'A' or JOBZ = 'O' and M < N;
   *          UCOL = min(M,N) if JOBZ = 'S'.
   *          If JOBZ = 'A' or JOBZ = 'O' and M < N, U contains the M-by-M
   *          orthogonal matrix U;
   *          if JOBZ = 'S', U contains the first min(M,N) columns of U
   *          (the left singular vectors, stored columnwise);
   *          if JOBZ = 'O' and M >= N, or JOBZ = 'N', U is not referenced.
   *
   *  LDU     (input) INTEGER
   *          The leading dimension of the array U.  LDU >= 1; if
   *          JOBZ = 'S' or 'A' or JOBZ = 'O' and M < N, LDU >= M.
   *
   *  VT      (output) DOUBLE PRECISION array, dimension (LDVT,N)
   *          If JOBZ = 'A' or JOBZ = 'O' and M >= N, VT contains the
   *          N-by-N orthogonal matrix V**T;
   *          if JOBZ = 'S', VT contains the first min(M,N) rows of
   *          V**T (the right singular vectors, stored rowwise);
   *          if JOBZ = 'O' and M < N, or JOBZ = 'N', VT is not referenced.
   *
   *  LDVT    (input) INTEGER
   *          The leading dimension of the array VT.  LDVT >= 1; if
   *          JOBZ = 'A' or JOBZ = 'O' and M >= N, LDVT >= N;
   *          if JOBZ = 'S', LDVT >= min(M,N).
   *
   *  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK))
   *          On exit, if INFO = 0, WORK(1) returns the optimal LWORK;
   *
   *  LWORK   (input) INTEGER
   *          The dimension of the array WORK. LWORK >= 1.
   *          If JOBZ = 'N',
   *            LWORK >= 3*min(M,N) + max(max(M,N),7*min(M,N)).
   *          If JOBZ = 'O',
   *            LWORK >= 3*min(M,N)*min(M,N) + 
   *                     max(max(M,N),5*min(M,N)*min(M,N)+4*min(M,N)).
   *          If JOBZ = 'S' or 'A'
   *            LWORK >= 3*min(M,N)*min(M,N) +
   *                     max(max(M,N),4*min(M,N)*min(M,N)+4*min(M,N)).
   *          For good performance, LWORK should generally be larger.
   *          If LWORK = -1 but other input arguments are legal, WORK(1)
   *          returns the optimal LWORK.
   *
   *  IWORK   (workspace) INTEGER array, dimension (8*min(M,N))
   *
   *  INFO   (output) INTEGER
   *          = 0:  successful exit.
   *          < 0:  if INFO = -i, the i-th argument had an illegal value.
   *          > 0:  DBDSDC did not converge, updating process failed.
   */

  extern "C" { 
    void   dgesdd_(char* jobz, int* m, int* n, double* A, int* lda,
		   double* S, double* U, int* ldu, double* vt,
		   int* ldvt, double* work, int* lwork, int* iwork,
		   int* info);
  }

  void svd(Matrix* A, Matrix* S, Matrix* U, Matrix* VT) {
    int M = A->m, N = A->n;
    int ld = A->ld;
    int lwork = 3*min(M,N)*min(M,N) +
      max(max(M,N),4*min(M,N)*min(M,N)+4*min(M,N)) + 1000;
#ifdef DEBUG
    std::cerr << "lwork: " << lwork << std::endl;
    std::cerr << "Allocating work array" << std::endl;
#endif
    double* Work = new double[lwork];
#ifdef DEBUG
    std::cerr << "Successfully allocated work array" << std::endl;
#endif
    int Iwork[8*N];
    int info;
    S->resize(1, min(M,N));
    U->resize(M, M);
    VT->resize(N, N);
    char jobz = 'A';
#ifdef DEBUG
    std::cerr << "Starting SVD" << std::endl;
#endif
    dgesdd_(&jobz, &M, &N, A->data, &ld,
	    S->data, U->data, (int*) &(U->ld),
	    VT->data, (int*) &(VT->ld),
	    Work, &lwork,
	    Iwork, &info);
#ifdef DEBUG
    std::cerr << "Finished SVD" << std::endl;
#endif
    Matrix _S(M,N);
    for(int i = 0; i < min(M,N); ++i) {
      _S(i,i) = S->data[i];
    }
    S->resize(M,N);
    swap(_S.data, S->data);
    delete [] Work;
  }
}
