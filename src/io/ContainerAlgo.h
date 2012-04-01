/**
 * @file   ContainerAlgo.h
 * @author Taesun Moon <tsmoon@wireless-128-62-86-190.public.utexas.edu>
 * @date   Mon Mar 23 17:10:23 2009
 * 
 * @brief  
 * 
 * 
 */


#ifndef __CONTAINER_ALGO_H__
#define __CONTAINER_ALGO_H__

#include <set>

namespace infl {

  using std::set;
  
  /** 
   * 
   * @brief Examines whether c1 is a contiguous subsequence in c2. c1
   * must always be shorter than or the same length as c2.
   * @param c1 stl container
   * @param c2 stl container
   * 
   * @return True if so, false if not.
   */
  template <class Container>
  bool is_in(const Container& c1, const Container& c2) {
    if(c1.size() > c2.size())
      return false;
  
    typename Container::const_iterator it, prev_find, this_find, temp_find;
    bool not_found = false;
    prev_find = c2.begin();
    while(prev_find != c2.end()) {
      it = c1.begin();
      prev_find = find(prev_find, c2.end(), *it++);
      temp_find = prev_find;
      for(; it != c1.end(); it++) {
	this_find = find(++prev_find, c2.end(), *it);
	if(prev_find != this_find) {
	  not_found = true;
	  break;
	} else {
	  not_found = false;
	}
      }
      if(!not_found)
	break;

      if(temp_find != c2.end())
	prev_find = ++temp_find;
      else
	prev_find = temp_find;
    }
    return !not_found;
  }


  /** 
   * @brief Creates a union of two sets by inserting the first set into
   * the second. 
   * 
   * @param s1 
   * @param s2 This set will change value
   */
  //
  template <class T>
  inline void set_union(const set<T>& s1, set<T>& s2) {
    for(typename set<T>::iterator it = s1.begin(); it != s1.end(); it++) {
      s2.insert(*it);
    }
  }


  /** 
   * @brief Calculates the cardinality of the union between two sets
   * 
   * @param s1 stl set
   * @param s2 stl set
   * 
   * @return unsigned integer
   */
  template <class T>
  inline UINT size_of_union(const set<T>* s1, const set<T>* s2) {
    UINT result = 0;
    for(typename set<T>::iterator it = s1->begin(); it != s1->end(); it++)
      if(s2->find(*it) != s2->end())
	result++;
    return s1->size() + s2->size() - result;
  
  }


  /** 
   * @brief Calculates the cardinality of the intersection between two
   * sets 
   * 
   * @param s1 stl set
   * @param s2 stl set
   * 
   * @return unsigned integer
   */
  template <class T>
  inline UINT size_of_its(const set<T>* const s1, const set<T>* const s2) {
    UINT result = 0;
    for(typename set<T>::const_iterator it = s1->begin(); it != s1->end(); it++)
      if(s2->find(*it) != s2->end())
	result++;
    return result;
  }


  /** 
   * @brief Returns the intersection of two sets. Returns a new instance
   * 
   * @param s1 stl set
   * @param s2 stl set
   * 
   * @return stl set
   */
  template <class T>
  inline set<T> intersection(const set<T>& s1, const set<T>& s2) {
    set<T> result;
    for(typename set<T>::iterator it = s1.begin(); it != s1.end(); it++)
      if(s2.find(*it) != s2.end())
	result.insert(*it);
    return result;
  }
}

#endif
