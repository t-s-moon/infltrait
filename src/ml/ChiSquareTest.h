/**
 * @file   ChiSquareTest.h
 * @author Taesun Moon <tsmoon@w-central-232-178.public.utexas.edu>
 * @date   Sat Apr 11 14:05:13 2009
 * 
 * @brief  
 * 
 * 
 */

#ifndef __CHI_SQUARE_TEST_H__
#define __CHI_SQUARE_TEST_H__

#include <cmath>

#include "Defs.h"

// #include "la/Matrix.h"

namespace infl {

  template <typename T, STAT_TEST_PARAM P>
  struct ChiSquare {
  };

//   template <typename T>
//   struct ChiSquare<T, STAT_TEST_FULL> {
//     const static double df1[];
//     const size_t confidence;
//     ChiSquare(size_t c=0) : confidence(c) {}
//     T evaluate(TemplMatrix<T>& m) {
//       T* rowsums = new T[m.m];
//       T* colsums = new T[m.n];
//       TemplMatrix<T> e(m.m,m.n);
//       T N = 0;
//       for(size_t i = 0; i < m.m; ++i) {
// 	rowsums[i] += m.get(i,CblasRowMajor).taxicab();
// 	N += rowsums[i];
//       }
//       for(size_t i = 0; i < m.n; ++i) {
// 	colsums[i] += m.get(i,CblasColMajor).taxicab();
//       }
//       for(size_t i = 0; i < m.m; ++i) {
// 	for(size_t j = 0; j < m.n; ++j) {
// 	  e(i,j) = rowsums[i] * colsums[j] / N;
// 	}
//       }
//       delete [] rowsums;
//       delete [] colsums;
//       T r = 0;
//       for(size_t i = 0; i < m.m; ++i) {
// 	for(size_t j = 0; j < m.n; ++j) {
// 	  r += pow(m(i,j)-e(i,j), 2) / e(i,j);
// 	}
//       }
//       return r;
//     }
//     bool is_significant(TemplMatrix<T>& m, T* _val=NULL) {
//       T val = evaluate(m);
//       if(_val != NULL)
// 	*_val = val;
//       return val  > df1[confidence];
//     }
//   };

  template <typename T>
  struct ChiSquare<T, STAT_TEST_DEFAULT> {
    const static double df1[];
    const size_t confidence;
    ChiSquare(size_t c=0) : confidence(c) {}
    T evaluate(size_t o11, size_t o12, size_t o21, size_t o22) {
      T N = o11 + o12 + o21 + o22;
      return N * pow(static_cast<double>(o11 * o22) - o12 * o21, 2) /
	// need casting to double in first term so there is no integer
	// overflow
	(static_cast<double>(o11+o12)*(o11+o21)*(o12+o22)*(o21+o22));
    }

    T evaluate_alt(size_t c1, size_t c2, size_t j, size_t N) {
      size_t o11=j, o12=c1-j, o21=c2-j,o22=N-c1-c2+j;
      return evaluate(o11,o12,o21,o22);
    }

    bool is_significant(size_t o11, size_t o12, size_t o21, size_t o22,
			double* _val=NULL) {
      double val = evaluate(o11, o12, o21, o22);
      if(_val != NULL)
	*_val = val;
      return val  > df1[confidence];
    }
  };

  template <typename T>
  const double ChiSquare<T,STAT_TEST_DEFAULT>::df1[] = {3.84, 6.63, 7.88};
}

#endif
