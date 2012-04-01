/**
 * @file   op.cpp
 * @author Taesun Moon <tsmoon@wireless-128-62-123-101.public.utexas.edu>
 * @date   Fri Feb  6 22:15:54 2009
 * 
 * @brief  
 * 
 * 
 */

#include "la/MiniGSL.h"
#include "la/Vector.h"
#include "la/Matrix.h"

namespace infl {
  std::ostream& operator<<(std::ostream& os, const Vector& v) {
    os << "[";
    size_t i = 0;
    while(i < v.size - 1) {
      os << v.data[i] << ", ";
      ++i;
    }
    os << v.data[i] << "]";
    return os;
  }

  Vector operator*(const Matrix& m, const Vector& v) {
    Vector v2(m.m);
    if(v.order == CblasColMajor) {
      gsl_blas_dgemv(CblasNoTrans, 1, m, v, 0, v2);
      return v2;
    } else {
      throw 0;
    }
  }

  double operator*(const Vector& v1, const Vector& v2) {
    return gsl_blas_ddot(v1, v2);
  }

  std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    os << "[";
    for(size_t i = 0; i < m.m; ++i) {
      size_t j = 0;
      while(j < m.n - 1){
	os << m.data[j*m.m+i] << ", ";
	j++;
      } 
      if(i != m.m - 1)
	os << m.data[j*m.m+i] << ";" << std::endl;
      else
	os << m.data[j*m.m+i] << "]";
    }
    return os;
  }

  Matrix operator+(const Matrix& m1, const Matrix& m2) {
    Matrix mat = m1;
    mat += m2;
    return mat;
  }

  Matrix operator-(const Matrix& m1, const Matrix& m2) {
    Matrix mat = m1;
    mat -= m2;
    return mat;
  }

  Matrix operator*(const Matrix& m1, const Matrix& m2) {
    if(m1.n == m2.m) {
      Matrix mat(m1.m, m2.n); 
      gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,m1,m2,0,mat);
      return mat;
    } else {
      throw 0;
    }
  }
}
