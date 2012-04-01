/**
 * @file   op.h
 * @author Taesun Moon <tsmoon@wireless-128-62-123-101.public.utexas.edu>
 * @date   Fri Feb  6 22:16:31 2009
 * 
 * @brief  
 * 
 * 
 */

#ifndef __OP_H__
#define __OP_H__

namespace infl {
  class Range;
  class Matrix;

  std::ostream& operator<<(std::ostream& os, const Vector& m);
  Vector operator*(const Matrix&, const Vector&);
  double operator*(const Vector&, const Vector&);

  std::ostream& operator<<(std::ostream& os, const Matrix& m);
  Matrix operator+(const Matrix&, const Matrix&);
  Matrix operator-(const Matrix&, const Matrix&);
  Matrix operator*(const Matrix&, const Matrix&);
}

#endif
