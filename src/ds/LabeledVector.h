/**
 * @file   LabeledVector.h
 * @author Taesun Moon <tsmoon@wireless-128-62-237-94.public.utexas.edu>
 * @date   Tue Mar  3 23:59:42 2009
 * 
 * @brief  
 * 
 * 
 */

#ifndef __LABELED_VECTOR_H__
#define __LABELED_VECTOR_H__

#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace infl {
  
  template <typename T>
  struct LabelVector : public std::vector<T> {
    std::string label;

    LabelVector(std::string& _l) :
      label(_l) {}

    LabelVector(typename LabelVector<T>::size_type _size,
		T _init) :
      std::vector<T>(_size, _init) {}
  };

  template <typename T>
  struct LabelIdxVector : public LabelVector<T> {
    std::string label;
    size_t cluster_idx;

    LabelIdxVector(typename LabelVector<T>::size_type _size,
		   T _init, std::string& _l, size_t _cl=0) :
      LabelVector<T>(_size, _init), label(_l), cluster_idx(_cl) {}

    LabelIdxVector(LabelVector<T>& lv) {
      label = lv.label;
      std::copy(lv.begin(), lv.end(), this->begin());
    }

    LabelIdxVector(LabelIdxVector<T>& liv) {
      if(this != &liv) {
	label = liv.label;
	cluster_idx = liv.cluster_idx;
	std::copy(liv.begin(), liv.end(), this->begin());
      }
    }
    
    LabelIdxVector(std::string& _l, size_t _cl=0) :
      label(_l), cluster_idx(_cl) {}
  };

  struct LabelIdxMap : public std::map<size_t, size_t> {
    size_t cluster_idx;
    LabelIdxMap() : cluster_idx(0) {}
  };
}

#endif
