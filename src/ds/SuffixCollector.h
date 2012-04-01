/**
 * @file   SuffixCollector.h
 * @author Taesun Moon <tsmoon@w-central-232-178.public.utexas.edu>
 * @date   Sat Apr 11 13:09:19 2009
 * 
 * @brief  
 * 
 * 
 */


#ifndef __SUFFIX_COLLECTOR_H__
#define __SUFFIX_COLLECTOR_H__

#include <algorithm>
#include <map>
#include <iostream>
#include <set>
#include <string>

#include "ds/PPMV.h"

#include "ml/ChiSquareTest.h"

namespace infl {
  
  using std::endl;
  using std::map;
  using std::ostream;
  using std::set;
  using std::string;

  template <typename T>
  void print_set(const set<T>* s) {
    for(typename set<T>::const_iterator it = s->begin(); it != s->end(); ++it) {
      std::cerr << *it << ",";
    }
    std::cerr << endl;
  }

  /** 
   * Compares two sets. If s2 is a proper subset of s1, then return
   * 1. If s1 is a proper subset of s2, return 2. If neither, return
   * 0. If they are the same, return 3.
   * 
   * @param s1 
   * @param s2 
   * 
   * @return 
   */
  template<typename T>
  size_t is_proper_subset(const set<T>&s1,const set<T>&s2) {
    if(s1 == s2) {
      return 3;
    } else {
      if(s1.size() < s2.size()) {
	for(typename set<T>::const_iterator it = s1.begin();
	    it != s1.end(); ++it) {
	  if(s2.find(*it) == s2.end())
	    return 0;
	}
	return 2;
      } else {
	for(typename set<T>::const_iterator it = s2.begin();
	    it != s2.end(); ++it) {
	  if(s1.find(*it) == s1.end())
	    return 0;
	}
	return 1;
      }
    }
  }

  template <typename Rl = BareBinaryRule, EXP_PARAM EP = EXP_PARAM_DEFAULT,
	    typename StatisticalTest = ChiSquare<double,STAT_TEST_DEFAULT> >
  struct SuffixCollector {
  public:
    class RuleCounter : public PMAP<Rl,size_t> {};
    class RuleChiSquare : public PMAP<Rl,double> {};
    class RuleStems : public PMAP<Rl,set<string> > {};

  public:
    size_t N;
    size_t min_cell;

    map<string,size_t> suffix_counter;
    map<string,set<string> > suffix_to_suffix, suffix_to_stem;

    set<set<string> > suffix_cluster_bin, cluster_bin;

    RuleCounter rc;
    RuleStems rs;
    RuleChiSquare valid_rules, all_rules;

    set<string>* vocabulary;

  public:
    SuffixCollector() {init();}
    SuffixCollector(set<string>*_voc, size_t _mc=5)
      : N(0), min_cell(_mc), vocabulary(_voc) {}

    ~SuffixCollector() {}

  public:
    void init() {N=0;min_cell=5;}

  public:
    void add(const string&, const string&);
    void add(const string&, const string&, const string&);
    void add(const set<string>& ss) {throw 0;}

    void add_base(const string& s, const string& stem) {
      suffix_to_stem[s].insert(stem);
      suffix_counter[s]++;
      N += 1;
    }
    
    void add_base(const string&s1, const string&s2, const string&stem) {
      Rl rl(s1,s2);
      rc[rl]++;
      rs[rl].insert(stem);
    }
    void add_base(const set<string>&);

  public:
    void calculate_significance(StatisticalTest&);
    void calculate_significance_all(StatisticalTest&);

    void calculate_significance_alt(StatisticalTest& cs) {
      for(typename RuleCounter::iterator it = rc.begin(); it != rc.end(); ++it) {
	size_t o11 = it->second;
	size_t o12 = suffix_counter.find(it->first.s1)->second - o11;
	size_t o21 = suffix_counter.find(it->first.s2)->second - o11;
	size_t o22 = N - o11 - o12 - o21;
	double val;
	if(cs.is_significant(o11,o12,o21,o22,&val) && o11 > min_cell
	   && o12 > min_cell && o21 > min_cell) {
	  valid_rules[it->first] = val;
	  string s1 = it->first.s1, s2 = it->first.s2;
	  suffix_to_suffix[s1].insert(s2);
	  suffix_to_suffix[s2].insert(s1);
	}
      }
    }

    void calculate_significance_all_alt(StatisticalTest& cs) {
      for(typename RuleCounter::iterator it = rc.begin(); it != rc.end(); ++it) {
	size_t o11 = it->second;
	size_t o12 = suffix_counter.find(it->first.s1)->second - o11;
	size_t o21 = suffix_counter.find(it->first.s2)->second - o11;
	size_t o22 = N - o11 - o12 - o21;
	double val;
	cs.is_significant(o11,o12,o21,o22,&val);
	all_rules[it->first] = val;
      }
    }

    void expand_clusters(const string& suffix, set<string> suffixes) {
      bool none_inserted = true;
      for(set<string>::iterator it = suffix_to_suffix[suffix].begin();
	  it != suffix_to_suffix[suffix].end(); ++it) {
	bool should_insert = true;
	for(set<string>::iterator its = suffixes.begin();
	    its != suffixes.end(); ++its) {
	  if(suffix_to_suffix[*it].find(*its)
	     == suffix_to_suffix[*it].end()) {
	    should_insert = false;
	    break;
	  }
	}
	if(should_insert) {
	  none_inserted = false;
	  suffixes.insert(*it);
	  expand_clusters(*it, suffixes);
	}
      }
      if(none_inserted)
	suffix_cluster_bin.insert(suffixes);
    }

    void expand_clusters();
    
    template <typename _Rl, EXP_PARAM _EP,
	      typename _StatisticalTest>
    void expand_clusters(SuffixCollector<_Rl, _EP, _StatisticalTest>& sc) {
      cluster_bin.clear();
      set<set<string>*> erase_bin;
      map<string, set<set<string>*> > temp_bin;
      for(set<set<string> >::iterator it = suffix_cluster_bin.begin();
	  it != suffix_cluster_bin.end(); ++it) {
	for(set<string>::iterator it_suffix1 = it->begin();
	    it_suffix1 != it->end(); ++it_suffix1) {
	  for(set<string>::iterator it_stem = suffix_to_stem[*it_suffix1].begin();
	      it_stem != suffix_to_stem[*it_suffix1].end(); ++it_stem) {
	    set<string>* variants = new set<string>;
	    set<string>::iterator it_suffix2 = it_suffix1; ++it_suffix2;
	    for(;it_suffix2 != it->end(); ++it_suffix2) {
	      const set<string>& tss =
		sc.rs[BareBinaryRule(*it_suffix1,*it_suffix2)];
	      if(tss.find(*it_stem) != tss.end()) {
		variants->insert(*it_stem + *it_suffix1);
		variants->insert(*it_stem + *it_suffix2);
	      }
	    }
	    if(variants->size() > 1) {
	      bool has_not_been_inserted = true;
	      for(set<string>::iterator it_word = variants->begin();
		  it_word != variants->end(); ++it_word) {
		if(!temp_bin[*it_word].size()) {
		  has_not_been_inserted = false;
		  temp_bin[*it_word].insert(variants);
		} else {
		  bool should_insert = false;
		  set<set<string>*> t_erase_bin;
		  for(set<set<string>*>::iterator itss = temp_bin[*it_word].begin();
		      itss != temp_bin[*it_word].end(); ++itss) {
		    bool should_break = false;
		    switch(is_proper_subset(*variants,**itss)) {
		    case 0: should_insert = true; break;
		    case 1: //temp_bin[*it_word].insert(variants);
		      t_erase_bin.insert(*itss);
		      erase_bin.insert(*itss);
		      has_not_been_inserted = false;
		      should_insert = true;
		      break;
		    case 2:
		    case 3: 
		      should_break = true;
		      should_insert = false;
		      break;
		    }
		    if(should_break)
		      break;
		  }
		  for(set<set<string>*>::iterator itss = t_erase_bin.begin(); itss != t_erase_bin.end(); ++itss)
		    temp_bin[*it_word].erase(*itss);
		  if(should_insert) {
		    has_not_been_inserted = false;
		    temp_bin[*it_word].insert(variants);
		  }
		}
	      }
	      if(has_not_been_inserted)
		delete variants;
	    } else {
	      delete variants;
	    }
	  }
	}
      }

      for(set<set<string>*>::iterator it = erase_bin.begin(); it != erase_bin.end(); ++it)
	delete *it;

      set<string> t_vocabulary = *vocabulary;
      erase_bin.clear();
      for(map<string,set<set<string>*> >::iterator it = temp_bin.begin();
	  it != temp_bin.end(); ++it) {
	for(set<set<string>*>::iterator its = it->second.begin();
	    its != it->second.end(); ++its) {
	  cluster_bin.insert(**its);
	  erase_bin.insert(*its);
	  for(set<string>::iterator itss = (**its).begin();
	      itss != (**its).end(); ++itss)
	    t_vocabulary.erase(*itss);
	}
      }

      for(set<string>::iterator it = t_vocabulary.begin();
	  it != t_vocabulary.end(); ++it) {
	set<string> singleton; singleton.insert(*it);
	cluster_bin.insert(singleton);
      }

      for(set<set<string>*>::iterator it = erase_bin.begin(); it != erase_bin.end(); ++it)
	delete *it;
    }

    void expand_clusters_alt(){
      for(map<string,set<string> >::iterator it = suffix_to_suffix.begin();
	  it != suffix_to_suffix.end(); ++it) {
	for(set<string>::iterator its = it->second.begin();
	    its != it->second.end(); ++its) {
	  set<string> suffixes;
	  suffixes.insert(it->first); suffixes.insert(*its);
	  expand_clusters(*its, suffixes);
	}
      }

      set<set<string>*> erase_bin;
      map<string, set<set<string>*> > temp_bin;
      for(set<set<string> >::iterator it = suffix_cluster_bin.begin();
	  it != suffix_cluster_bin.end(); ++it) {
	for(set<string>::iterator it_suffix1 = it->begin();
	    it_suffix1 != it->end(); ++it_suffix1) {
	  for(set<string>::iterator it_stem = suffix_to_stem[*it_suffix1].begin();
	      it_stem != suffix_to_stem[*it_suffix1].end(); ++it_stem) {
	    set<string>* variants = new set<string>;
	    for(set<string>::iterator it_suffix2 = it->begin();
		it_suffix2 != it->end(); ++it_suffix2) {
	      string word = *it_stem + *it_suffix2;
	      if(vocabulary->find(word) != vocabulary->end()) {
		variants->insert(word);
	      }
	    }
	    if(variants->size() > 1) {
	      bool has_not_been_inserted = true;
	      for(set<string>::iterator it_word = variants->begin();
		  it_word != variants->end(); ++it_word) {
		if(!temp_bin[*it_word].size()) {
		  has_not_been_inserted = false;
		  temp_bin[*it_word].insert(variants);
		} else {
		  bool should_insert = false;
		  set<set<string>*> t_erase_bin;
		  for(set<set<string>*>::iterator itss = temp_bin[*it_word].begin();
		      itss != temp_bin[*it_word].end(); ++itss) {
		    bool should_break = false;
		    switch(is_proper_subset(*variants,**itss)) {
		    case 0: should_insert = true; break;
		    case 1: //temp_bin[*it_word].insert(variants);
		      t_erase_bin.insert(*itss);
		      erase_bin.insert(*itss);
		      has_not_been_inserted = false;
		      should_insert = true;
		      break;
		    case 2:
		    case 3: 
		      should_break = true;
		      should_insert = false;
		      break;
		    }
		    if(should_break)
		      break;
		  }
		  for(set<set<string>*>::iterator itss = t_erase_bin.begin(); itss != t_erase_bin.end(); ++itss)
		    temp_bin[*it_word].erase(*itss);
		  if(should_insert) {
		    has_not_been_inserted = false;
		    temp_bin[*it_word].insert(variants);
		  }
		}
	      }
	      if(has_not_been_inserted)
		delete variants;
	    } else {
	      delete variants;
	    }
	  }
	}
      }

      for(set<set<string>*>::iterator it = erase_bin.begin(); it != erase_bin.end(); ++it)
	delete *it;

      set<string> t_vocabulary = *vocabulary;
      erase_bin.clear();
      for(map<string,set<set<string>*> >::iterator it = temp_bin.begin();
	  it != temp_bin.end(); ++it) {
	for(set<set<string>*>::iterator its = it->second.begin();
	    its != it->second.end(); ++its) {
	  cluster_bin.insert(**its);
	  erase_bin.insert(*its);
	  for(set<string>::iterator itss = (**its).begin();
	      itss != (**its).end(); ++itss)
	    t_vocabulary.erase(*itss);
	}
      }

      for(set<string>::iterator it = t_vocabulary.begin();
	  it != t_vocabulary.end(); ++it) {
	set<string> singleton; singleton.insert(*it);
	cluster_bin.insert(singleton);
      }

      for(set<set<string>*>::iterator it = erase_bin.begin(); it != erase_bin.end(); ++it)
	delete *it;
    }
      
    ostream& write_clusters(ostream& os) {
      for(size_t i = 0; i < 80; ++i)
	os << "=";
      os << endl;
      os << "List of maximal clusters:" << endl;
      for(set<set<string> >::iterator it = suffix_cluster_bin.begin();
	  it != suffix_cluster_bin.end(); ++it) {
	for(set<string>::iterator its = it->begin();
	    its != it->end(); ++its) {
	  os << isnull(*its) << "\t";
	}
	os << endl;
      }
      return os;
    }

    //Broke const-ness of function to accomodate tr1::unordered_map
    ostream& write_stream(ostream& os)  {
      for(set<set<string> >::iterator it = cluster_bin.begin();
	  it != cluster_bin.end(); ++it) {
	for(set<string>::iterator its = it->begin();
	    its != it->end(); ++its) {
	  os << *its << " ";
	  vocabulary->erase(*its);
	}
	os << endl;
      }

      for(set<string>::iterator it = vocabulary->begin();
	  it != vocabulary->end(); ++it) {
	os << *it << endl;
      }
      return os;
    }

    ostream& write_stream_all(ostream& os)  {
      for(size_t i = 0; i < 80; ++i)
	os << "=";
      os << endl;
      os << "Printing valid chi-square scores and pairs" << endl;
      os << "Total N count: " << N << endl;
      os << "Total number of suffixes: " << suffix_counter.size() << endl;
      for(typename RuleChiSquare::iterator it = valid_rules.begin();
	  it != valid_rules.end(); ++it) {
	it->first.output_to_stream(os);
	os << "\ts1_count:" << suffix_counter.find(it->first.s1)->second
	   << ",s2_count:" << suffix_counter.find(it->first.s2)->second
	   << ",joint:" << rc.find(it->first)->second
	   << ",x2:" << it->second;
	set<string>& stems = rs[it->first];
	os << ",stems:";
	for(set<string>::iterator it = stems.begin();
	    it != stems.end(); ++it) {
	  os << *it << ",";
	}
	os << endl;
      }

      for(size_t i = 0; i < 80; ++i)
	os << "=";
      os << endl;
      os << "Printing all chi-square scores" << endl;;
      for(typename RuleChiSquare::iterator it = all_rules.begin();
	  it != all_rules.end(); ++it) {
	it->first.output_to_stream(os);
	os << "\ts1_count:" << suffix_counter.find(it->first.s1)->second
	   << ",s2_count:" << suffix_counter.find(it->first.s2)->second
	   << ",joint:" << rc.find(it->first)->second
	   << ",x2:" << it->second;
	set<string>& stems = rs[it->first];
	os << ",stems:";
	for(set<string>::iterator it = stems.begin();
	    it != stems.end(); ++it) {
	  os << *it << ",";
	}
	os << endl;
      }
      return os;
    }

    set<set<string> >& flip() {
      set<set<string> > tc;
      std::swap(tc,cluster_bin);
      for(set<set<string> >::iterator it = tc.begin();
	  it != tc.end(); ++it) {
	set<string> dump;
	for(set<string>::iterator itc = it->begin(); itc != it->end(); ++itc) {
	  string st = *itc;
	  reverse(st.begin(), st.end());
	  dump.insert(st);
	}
	cluster_bin.insert(dump);
      }

      set<string> tv;
      std::swap(*vocabulary,tv);
      for(set<string>::iterator it = tv.begin(); it != tv.end(); ++it) {
	string st = *it;
	reverse(st.begin(), st.end());
	vocabulary->insert(st);
      }
      return cluster_bin;
    }
  };
}

#endif
