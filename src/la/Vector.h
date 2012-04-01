/**
 * @file   vector.h
 * @author Taesun Moon <tsmoon@wireless-128-62-123-101.public.utexas.edu>
 * @date   Fri Feb  6 22:16:18 2009
 * 
 * @brief  
 * 
 * 
 */

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <algorithm>
#include <iterator>
#include <vector>

#include <ctime>

#ifdef __cplusplus
extern "C" {
#endif
#include <cblas.h>
#ifdef  __cplusplus
}
#endif

namespace infl {

  template <class T>
  struct TemplVector {
    size_t size;
    size_t stride;
    CBLAS_ORDER order; // {CblasRowMajor=101, CblasColMajor=102};
    T* data;

    TemplVector() {init();}
    TemplVector(size_t s) {init(); resize(s);}
    TemplVector(size_t s, const T* d) {
      init(); resize(s);
      for(size_t i = 0; i < s; ++i)
	data[i] = d[i];
    }
    TemplVector(const TemplVector& v) {init(); *this = v;}

    template<typename ForwardIterator>
    explicit TemplVector(const ForwardIterator first, 
			 const ForwardIterator last) {
      init(); resize(last - first);
      std::copy(first, last, data);
    }
    ~TemplVector() {delete [] data;}
    
    TemplVector& operator=(const TemplVector& v) {
      if(this != &v) {
	resize(v.size);
	for(size_t i = 0; i < size; ++i)
	  data[i] = v.data[i];
      }
      return *this;
    }

    TemplVector& operator+=(const TemplVector& v) {
      if(*this == v) {
	for(size_t i = 0; i < size; ++i) 
	  data[i] += v.data[i];
	return *this;
      } else {
	throw 0;
      }
    }

    TemplVector& operator-=(const TemplVector& v) {
      if(*this == v) {
	for(size_t i = 0; i < size; ++i) 
	  data[i] -= v.data[i];
	return *this;
      } else {
	throw 0;
      }
    }

    inline bool operator==(const TemplVector& v) {
      return size == v.size && order == v.order;
    }

    T& operator[] (size_t i) {
      return data[i];
    }

    inline void init(){
      size = stride = 1;
      data = new T[1];
#ifndef DEBUG
      srand(time(NULL));
#endif
    }

    TemplVector& randomize() {
      for(size_t i = 0; i < size; ++i)
	data[i] = 2 * rand() / (T) RAND_MAX;
      return *this;
    }

    TemplVector& resize(size_t s, CBLAS_ORDER o = CblasColMajor) {
      delete [] data;
      order = o;
      size = s;
      data = new T[size];
      for(size_t i = 0; i < size; ++i)
	data[i] = 0;
      return *this;
    }

    TemplVector transpose() {
      TemplVector v = *this;
      v.order = (order == CblasRowMajor ? CblasColMajor : CblasRowMajor);
      return v;
    }
  };

  struct Vector : public TemplVector<double> {
    Vector() : TemplVector<double>() {}
    Vector(size_t s) : TemplVector<double>(s) {}
    Vector(size_t s, const double* d) : TemplVector<double>(s,d) {}
    Vector(const Vector& v) : TemplVector<double>(v) {}
  };
}

#endif
