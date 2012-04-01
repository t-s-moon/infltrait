/**
 * @file   matrix.h
 * @author Taesun Moon <tsmoon@wireless-128-62-123-101.public.utexas.edu>
 * @date   Fri Feb  6 22:16:42 2009
 * 
 * @brief  
 * 
 * 
 */

#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <iostream>
#include <algorithm>
#include <iterator>
#include <ctime>
#include <cmath>

#include "la/Vector.h"

namespace infl {
  
  using std::min;
  using std::max;
  using std::swap;
  
  struct Range {
    int rstart, rend;
    int cstart, cend;
    Range() {rstart=cstart=0;rend=cend=-1;}
    Range(int _rstart, int _rend, int _cstart, int _cend) :
      rstart(_rstart), rend(_rend), cstart(_cstart), cend(_cend) {}
    inline void set(int _rstart, int _rend, int _cstart, int _cend) {
      rstart=_rstart; rend=_rend; cstart=_cstart; cend=_cend;
    }
  };

  template<typename T>
  struct TemplMatrix {
    size_t m;	//row size
    size_t n;	//column size
    size_t ld;	//leading dimension
    size_t td;	//trailing dimension
    T * data;

    TemplMatrix(){init();}
    TemplMatrix(size_t _m, size_t _n){init(); resize(_m, _n);}
    TemplMatrix(size_t _m, size_t _n, const T* _data) {
      init(); resize(_m, _n);
      for(size_t i = 0; i < m*n; ++i)
	data[i] = _data[i];
    }
    TemplMatrix(const TemplMatrix& mat) {init(); *this = mat;}
    virtual ~TemplMatrix() {delete [] data;}

    T& operator()(size_t i, size_t j){
      if(i < m && j < n)
	return data[i + j*ld];
      else
	throw 0;
    }

    TemplMatrix& operator=(const TemplMatrix& mat) {
      if(this != &mat){
	resize(mat.m, mat.n);
	for(size_t i = 0; i < m*n; ++i)
	  data[i] = mat.data[i];
      }
      return *this;
    }

    inline TemplMatrix& operator+=(const TemplMatrix& mat) {
      if(m == mat.m && n == mat.n) {
	for(size_t i = 0; i < m*n; ++i)
	  data[i] += mat.data[i];
	return *this;
      } else {
	throw 0;
      }
    }

    inline TemplMatrix& operator-=(const TemplMatrix& mat) {
      if(m == mat.m && n == mat.n) {
	for(size_t i = 0; i < m*n; ++i)
	  data[i] -= mat.data[i];
	return *this;
      } else {
	throw 0;
      }
    }

    inline TemplMatrix& operator*=(T d) {
      for(size_t i = 0; i < m*n; ++i)
	data[i] *= d;
      return *this;
    }

    double frobenius() {
      double sum;
      for (size_t i = 0; i < m*n; ++i)
	sum += pow(data[i], 2);
      return pow(sum, 0.5);
    }

    double taxicab() {
      double sum;
      for(size_t i = 0; i < m*n; ++i)
	sum += abs(data[i]);
      return sum;
    }

    virtual TemplMatrix get(Range& r) {
      TemplMatrix mat;
      size_t _m, _n;
      if(r.rend == -1)
	_m = m - r.rstart;
      else
	_m = r.rend - r.rstart;
      if(r.cend == -1)
	_n = n - r.cstart;
      else
	_n = r.cend - r.cstart;
      mat.resize(_m,_n);
      for(size_t i=r.rstart,_i=0;i<(r.rend==-1?m:r.rend);++i,++_i)
	for(size_t j=r.cstart,_j=0;j<(r.cend==-1?n:r.cend);++j,++_j)
	  mat(_i,_j) = operator()(i,j);
      return mat;
    }

    virtual void get(TemplMatrix& mat, Range& r) {
      size_t _m, _n;
      if(r.rend == -1)
	_m = m - r.rstart;
      else
	_m = r.rend - r.rstart;
      if(r.cend == -1)
	_n = n - r.cstart;
      else
	_n = r.cend - r.cstart;
      mat.resize(_m,_n);
      for(size_t i=r.rstart,_i=0;i<(r.rend==-1?m:r.rend);++i,++_i)
	for(size_t j=r.cstart,_j=0;j<(r.cend==-1?n:r.cend);++j,++_j)
	  mat(_i,_j) = operator()(i,j);
    }

    virtual Vector get(size_t i, CBLAS_ORDER o) {
      Vector v;
      get(i, v, o);
      return v;
    }

    virtual void get(size_t i, Vector& v, CBLAS_ORDER o) {
      if(o == CblasRowMajor) {
	v.resize(n, o);
	for(size_t j = 0; j < n; ++j)
	  v[j] = operator()(i,j);
      } else {
	v.resize(m, o);
	for(size_t j = 0; j < m; ++j)
	  v[j] = operator()(j,i);
      }
    }

    inline void init(){
      data = new T[1];
#ifndef DEBUG
      srand(time(NULL));
#endif
    }

    TemplMatrix& resize(size_t _m, size_t _n) {
      delete [] data;
      m = _m; n = _n; ld = _m; td = _n;
      data = new T[m * n];
      for(size_t i = 0; i < ld; ++i)
	for(size_t j = 0; j < td; ++j)
	  data[i*td+j] = 0;
      return *this;
    }

    TemplMatrix&  randomize() {
      for(size_t i = 0; i < ld; ++i)
	for(size_t j = 0; j < td; ++j)
	  data[i*td+j] = 2 * rand() / (T) RAND_MAX;
      return *this;
    }

    TemplMatrix& transpose() {
      T* tdata = new T[m*n];
      for(size_t i = 0; i < n; ++i)
	for(size_t j = 0; j < m; ++j)
	  tdata[j*n+i] = data[i*m+j];
      swap(m,n); swap(ld,td);
      delete [] data;
      data = tdata;
      return *this;
    }

  };

  struct Matrix : public TemplMatrix<double> {
    Matrix() : TemplMatrix<double>() {}
    Matrix(size_t _m, size_t _n) : TemplMatrix<double>(_m,_n) {}
    Matrix(size_t _m, size_t _n, const double* _data)
      : TemplMatrix<double>(_m,_n,_data) {}
    Matrix(const Matrix& mat) : TemplMatrix<double>(mat) {}
  };
}

#endif
