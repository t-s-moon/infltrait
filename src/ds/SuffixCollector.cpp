#include "ds/SuffixCollector.h"

namespace infl {
  template<>
  void SuffixCollector<BareBinaryRule>::add
  (const string& s1, const string& s2, const string& stem) {
    BareBinaryRule bbr(s1,s2);
    rc[bbr]++;
    rs[bbr].insert(stem);
    suffix_to_stem[s1].insert(stem);
    suffix_to_stem[s2].insert(stem);
    suffix_counter[s1]++; suffix_counter[s2]++;
    N += 2;
  }

  template<>
  void SuffixCollector<BareBinaryRule>::add
  (const string& s, const string& stem) {
    add_base(s,stem);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_DS_DC>::add
  (const string& s, const string& stem) {
    add_base(s,stem);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_GS_DC>::add
  (const string& s, const string& stem) {
    add_base(s,stem);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_DS_GC>::add
  (const string& s, const string& stem) {
    add_base(s,stem);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_DS_DC>::add
  (const string& s, const string& stem) {
    add_base(s,stem);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_DS_GC>::add
  (const string& s, const string& stem) {
    add_base(s,stem);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_GS_DC>::add
  (const string& s, const string& stem) {
    add_base(s,stem);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_DS_DC>::add
  (const string& s1, const string& s2, const string& stem) {
    add_base(s1,s2,stem);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_GS_DC>::add
  (const string& s1, const string& s2, const string& stem) {
    add_base(s1,s2,stem);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_DS_GC>::add
  (const string& s1, const string& s2, const string& stem) {
    add_base(s1,s2,stem);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_DS_DC>::add
  (const string& s1, const string& s2, const string& stem) {
    add_base(s1,s2,stem);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_DS_GC>::add
  (const string& s1, const string& s2, const string& stem) {
    add_base(s1,s2,stem);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_GS_DC>::add
  (const string& s1, const string& s2, const string& stem) {
    add_base(s1,s2,stem);
  }

  template<>
  void SuffixCollector<BareBinaryRule>::calculate_significance
  (ChiSquare<double,STAT_TEST_DEFAULT>& cs) {
    calculate_significance_alt(cs);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_DS_DC>::calculate_significance
  (ChiSquare<double,STAT_TEST_DEFAULT>& cs) {
    calculate_significance_alt(cs);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_DS_GC>::calculate_significance
  (ChiSquare<double,STAT_TEST_DEFAULT>& cs) {
    calculate_significance_alt(cs);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_DS_DC>::calculate_significance
  (ChiSquare<double,STAT_TEST_DEFAULT>& cs) {
    calculate_significance_alt(cs);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_DS_GC>::calculate_significance
  (ChiSquare<double,STAT_TEST_DEFAULT>& cs) {
    calculate_significance_alt(cs);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_GS_DC>::calculate_significance
  (ChiSquare<double,STAT_TEST_DEFAULT>& cs) {
    calculate_significance_alt(cs);
  }

  template<>
  void SuffixCollector<BareBinaryRule>::calculate_significance_all
  (ChiSquare<double,STAT_TEST_DEFAULT>& cs) {
    calculate_significance_all_alt(cs);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_DS_DC>::calculate_significance_all
  (ChiSquare<double,STAT_TEST_DEFAULT>& cs) {
    calculate_significance_all_alt(cs);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_DS_GC>::calculate_significance_all
  (ChiSquare<double,STAT_TEST_DEFAULT>& cs) {
    calculate_significance_all_alt(cs);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_DS_DC>::calculate_significance_all
  (ChiSquare<double,STAT_TEST_DEFAULT>& cs) {
    calculate_significance_all_alt(cs);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_DS_GC>::calculate_significance_all
  (ChiSquare<double,STAT_TEST_DEFAULT>& cs) {
    calculate_significance_all_alt(cs);
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_GS_DC>::calculate_significance_all
  (ChiSquare<double,STAT_TEST_DEFAULT>& cs) {
    calculate_significance_all_alt(cs);
  }

  template<>
  void SuffixCollector<BinaryRule>::expand_clusters() {
    std::cerr << "\"void SuffixCollector<BinaryRule>::expand_clusters()\""
	      << " has not been implemented yet!" << std::endl;
    throw 0;
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_PARAM_DEFAULT>::expand_clusters() {
    expand_clusters_alt();
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_DS_GC>::expand_clusters() {
    expand_clusters_alt();
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_DS_DC>::expand_clusters() {
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
	  set<string>::iterator it_suffix2 = it_suffix1; ++it_suffix2;
	  for(;it_suffix2 != it->end(); ++it_suffix2) {
	    const set<string>& tss =
	      rs[BareBinaryRule(*it_suffix1,*it_suffix2)];
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

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_DS_DC>::expand_clusters() {
    expand_clusters_alt();
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_DS_GC>::expand_clusters() {
    expand_clusters_alt();
  }

  template<>
  void SuffixCollector<BareBinaryRule,EXP_USP_GS_DC>::expand_clusters() {
    expand_clusters_alt();
  }
}
