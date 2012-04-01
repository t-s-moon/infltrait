/**
 * @file   Trie.cpp
 * @author Taesun Moon <tsmoon@wireless-128-62-123-101.public.utexas.edu>
 * @date   Fri Feb  6 22:15:41 2009
 * 
 * @brief  
 * 
 * 
 */

#ifdef DEBUG
#include <iostream>
#endif

#include <algorithm>

#include "ds/Trie.h"

namespace infl {

  template <>
  void Trie<BaselineRule,InflHash<BaselineRule>,
	    ChiSquare<double,STAT_TEST_DEFAULT>,
	    BASELINE_FIRST_K>
  ::find_suffixes(Trie* branch_point,WORDBAG& wb) {
    string s;
    if(terminal != NULL) {
      get_string_from_node(s, branch_point);
      std::reverse(s.begin(), s.end());
      wb.insert(s);
    } 
    if(sibling != NULL)
      sibling->find_suffixes(branch_point, wb);
    if(child != NULL)
      child->find_suffixes(branch_point, wb);
  }
    
  /**
   * 
   *   MUST FIX!!!!
   */
  template <>
  void Trie<BaselineRule,InflHash<BaselineRule>,
	    ChiSquare<double,STAT_TEST_DEFAULT>,
	    BASELINE_FIRST_K>
  ::find_stems(size_t current_depth, size_t min_len,
	       set<set<string> >& clusters, set<string>& vocab) {
    WORDBAG suffixes;
    string stem;
    parent->get_string_from_node(stem);
    std::reverse(stem.begin(), stem.end());
    if(current_depth > min_len) {
      if(parent->terminal != NULL || sibling != NULL) {
	find_suffixes(parent, suffixes);
	set<string> cluster;
	for(set<string>::iterator it = suffixes.begin();
	    it != suffixes.end(); ++it) {
	// 	if(vocab.find(stem + *it) == vocab.end()) {
	  cluster.insert(stem + *it);
	  vocab.insert(stem + *it);
	// 	}
	}
      //       if(cluster.size())
	clusters.insert(cluster);
      }
    } else {
      if(parent != NULL && parent->terminal != NULL) {
	// 	if(vocab.find(stem) == vocab.end()) {
	vocab.insert(stem);
	set<string> cluster; cluster.insert(stem);
	clusters.insert(cluster);
	// 	}
      }
      if(child != NULL)
	child->find_stems(current_depth + 1, min_len, clusters, vocab);
      if(sibling != NULL)
	sibling->find_stems(current_depth, min_len, clusters, vocab);
    }
  }

  /**
   * 
   * MUST FIX!!!
   */
  template <>
  void Trie<BinaryRule>::find_stems
  (size_t current_depth, size_t min_len) {
    WORDBAG suffixes;
    string stem;
    get_string_from_node(stem);
    std::reverse(stem.begin(), stem.end());
    if(current_depth > min_len) {
      if(terminal != NULL || sibling != NULL) {
	if(terminal != NULL || key == EOS)
	  suffixes.insert("");
	if(child != NULL)
	  child->find_suffixes(parent, suffixes,stem.size(),1);
	if(sibling != NULL)
	  sibling->find_suffixes(parent, suffixes,stem.size(),1);
	for(WORDBAG::const_iterator it1 = suffixes.begin();
	    it1 != suffixes.end(); ++it1) {
	  WORDBAG::const_iterator it2 = it1; ++it2;
	  for(;it2 != suffixes.end(); ++it2) {
	    BinaryRule* r = new BinaryRule(*it1, *it2);
	    if(rule_pmap->find(*r) != rule_pmap->end())
	      ++(*(*rule_pmap)[*r]);
	    else
	      (*rule_pmap)[*r] = r;
	    PPMV<BinaryRule>* p = new PPMV<BinaryRule>(stem, (*rule_pmap)[*r]);
	    ppmv_pvec->push_back(p);
	    ((*rule_pmap)[*r])->ppmv_pset->insert(p);
	  }
	}
      }
    }
    if(child != NULL)
      child->find_stems(current_depth + 1, min_len);
    if(sibling != NULL)
      sibling->find_stems(current_depth, min_len);
  }

  /**
   * 
   * MUST FIX!!
   */
  template <>
  void Trie<UnrestrictedRule>::find_stems
  (size_t current_depth, size_t min_len) {
    WORDBAG suffixes;
    string stem;
    get_string_from_node(stem);
    std::reverse(stem.begin(), stem.end());
    try {
      if(current_depth > min_len) {
	if(terminal != NULL || sibling != NULL) {
	  if(terminal != NULL || key == EOS)
	    suffixes.insert("");
	  if(child != NULL)
	    child->find_suffixes(parent, suffixes, stem.size(),1);
	  if(sibling != NULL)
	    sibling->find_suffixes(parent, suffixes, stem.size(),1);
	  if(sc != NULL) sc->add(suffixes);
	  UnrestrictedRule* ur = new UnrestrictedRule(suffixes);
	  if(rule_pmap->find(*ur) != rule_pmap->end())
	    ++(*(*rule_pmap)[*ur]);
	  else
	    (*rule_pmap)[*ur] = ur;
	  PPMV<UnrestrictedRule>* p =
	    new PPMV<UnrestrictedRule>(stem, (*rule_pmap)[*ur]);
	  ppmv_pvec->push_back(p);
	  ((*rule_pmap)[*ur])->ppmv_pset->insert(p);
	}
      }
    } catch (int) {}
    if(child != NULL)
      child->find_stems(current_depth + 1, min_len);
    if(sibling != NULL)
      sibling->find_stems(current_depth, min_len);
  }

  template<>
  void Trie<BareBinaryRule>::partition_ppmv_by_rule(size_t _n) {;}

  template<>
  void Trie<BareBinaryRule,InflHash<BareBinaryRule>,
	    ChiSquare<double,STAT_TEST_DEFAULT>,EXP_DS_DC>
    ::partition_ppmv_by_rule(size_t _n) {;}

  template<>
  void Trie<BinaryRule>::partition_ppmv_by_rule(size_t _n) {
    std::vector<BinaryRule*>* vr = new std::vector<BinaryRule*>;
    for(RULE_PMAP::iterator it = rule_pmap->begin();
	it != rule_pmap->end(); ++it) {
      vr->push_back(it->second);
    }
    std::sort(vr->begin(), vr->end(), RuleCmp<BinaryRule>());

#ifdef MDEBUG
    for(size_t i = 0; i < _n; ++i) {
      std::cout << *(vr->at(i))  << std::endl;
    }
    size_t s;
#endif
    
    PPMV_PVEC* pt = new PPMV_PVEC;
    for(size_t i = 0; i < vr->size(); ++i) {
      if(i < _n) {
	for(std::set<PPMV<BinaryRule>*>::iterator it = (vr->at(i))->ppmv_pset->begin();
	    it != (vr->at(i))->ppmv_pset->end(); ++it) {
	  pt->push_back(*it);
	}
      } else {
	for(std::set<PPMV<BinaryRule>*>::iterator it = (vr->at(i))->ppmv_pset->begin();
	    it != (vr->at(i))->ppmv_pset->end(); ++it) {
	  delete *it;
	}
	rule_pmap->erase(*(vr->at(i)));
#ifdef MDEBUG
	s = rule_pmap->size();
#endif
	delete vr->at(i);
      }
    }
    swap(pt, ppmv_pvec);
#ifdef MDEBUG
    /// Doing sanity check. Check whether all the rule pointers in
    /// rule_pmap correspond to the ones in ppmv_pvec
    for(typename RULE_PMAP::iterator it = rule_pmap->begin();
	it != rule_pmap->end(); ++it) {
      std::cout << it->first << std::endl;
      std::cout << *(it->second) << std::endl;
    }
    
    for(size_t i = 0; i < ppmv_pvec->size(); ++i) {
      PPMV p = *(ppmv_pvec->at(i));
      if(rule_pmap->find(*p.r) == rule_pmap->end()) {
	std::cout << p << std::endl;
      }
    }
#endif 
    delete pt; delete vr;
  }

  template <>
  void Trie<BinaryRule>::seed_from_stream
  (std::istream* is,size_t min_stem_len,
   size_t max_ppmvs, set<string>* vocabulary) {
    string current_word, prev_word;
    int counter = 0;
    while(*is >> current_word) {
      if(sf->filter(current_word) != "") {
	set_first(current_word);
	vocabulary->insert(current_word);
	break;
      }
    }
    counter++;
    while(*is >> current_word) {
      if(sf->filter(current_word) != "") {
	insert(current_word);
	vocabulary->insert(current_word);
      }
    }
    find_stems(0,min_stem_len);
    partition_ppmv_by_rule(max_ppmvs);
  }
}
