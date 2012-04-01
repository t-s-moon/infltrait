/**
 * @file   Triple.h
 * @author Taesun Moon <tsmoon@w-central-232-178.public.utexas.edu>
 * @date   Sun Apr 12 18:11:51 2009
 * 
 * @brief  
 * 
 * 
 */

#ifndef __TRIPLE_H__
#define __TRIPLE_H__

#include <map>
#include <string>
#include <vector>

namespace infl {
  template <typename Container = std::vector<double>,
	    typename StringT = std::string, typename T = size_t>
  struct Triple {
    Container* const vp;
    StringT label;
    T cluster_idx;

    Triple(Container* _vp, StringT& _l, T _cl) :
      vp(_vp), label(_l), cluster_idx(_cl) {}

    typename Container::iterator begin() const {return vp->begin();}
    typename Container::iterator end() const {return vp->end();}
    typename Container::size_type size() const {return vp->size();}

    bool operator<(const Triple& tr) const {
      return vp < tr.vp;
    }
  };

  typedef Triple<std::vector<double> > TRIPLEVD;
  typedef Triple<std::vector<float> > TRIPLEVF;

  typedef std::vector<TRIPLEVD> VTRIPLEVD;
  typedef std::vector<TRIPLEVF> VTRIPLEVF;

  typedef std::map<TRIPLEVD, size_t> MTRIPLEVD;
  typedef std::map<TRIPLEVF, size_t> MTRIPLEVF;
}

#endif
