#include <algorithm>
#include <functional>

#include "ds/RuleByDoc.h"

namespace infl {

  template <> void RuleByDoc
  <BareBinaryRule, InflHash<BareBinaryRule>,
   ChiSquare<double,STAT_TEST_DEFAULT> >
  ::initialize_statistical_test() {
    st = new ChiSquare<double,STAT_TEST_DEFAULT>(ep.confidence);
  }

  template <> void RuleByDoc
  <BareBinaryRule, InflHash<BareBinaryRule>,
   ChiSquare<double,STAT_TEST_DEFAULT>,EXP_DS_DC>
  ::initialize_statistical_test() {
    st = new ChiSquare<double,STAT_TEST_DEFAULT>(ep.confidence);
  }

  template <> void RuleByDoc
  <BareBinaryRule, InflHash<BareBinaryRule>,
   ChiSquare<double,STAT_TEST_DEFAULT>,EXP_DS_GC>
  ::initialize_statistical_test() {
    st = new ChiSquare<double,STAT_TEST_DEFAULT>(ep.confidence);
  }

  template <> void RuleByDoc
  <BareBinaryRule, InflHash<BareBinaryRule>,
   ChiSquare<double,STAT_TEST_DEFAULT>, EXP_PARAM_DEFAULT>
  ::process_rule_by_doc() {
    process_rule_by_doc_base();
  }

  template <> void RuleByDoc
  <BareBinaryRule, InflHash<BareBinaryRule>,
   ChiSquare<double,STAT_TEST_DEFAULT>,EXP_DS_DC>
  ::process_rule_by_doc() {
    process_rule_by_doc_base();
  }

  template <> void RuleByDoc
  <BareBinaryRule, InflHash<BareBinaryRule>,
   ChiSquare<double,STAT_TEST_DEFAULT>,EXP_GS_DC>
  ::process_rule_by_doc() {
    for(size_t i = 0; i < disp.size(); ++i) {
      std::istream* is = &(disp[i]);
      RlTrie* trie = new RlTrie(sf,sc);
      trie->reset_containers();
      trie->seed_from_stream(is, vocabulary);
      trie->clear();
      trie->delete_containers();
    }
  }

  template <> void RuleByDoc
  <BareBinaryRule, InflHash<BareBinaryRule>,
   ChiSquare<double,STAT_TEST_DEFAULT>,EXP_DS_GC>
  ::process_rule_by_doc() {
    for(size_t i = 0; i < disp.size(); ++i) {
      std::istream* is = &(disp[i]);
      RlTrie* trie = new RlTrie(sf,sc);
      trie->reset_containers();
      trie->seed_from_stream(is, vocabulary);
      insert_rule_pmap(trie->rule_pmap);
      insert_ppmv_pmap(trie->ppmv_pvec);
      trie->clear();
      trie->delete_containers();
    }

    try {
      initialize_statistical_test();
      sc->calculate_significance(*st); 
      sc->expand_clusters();
    } catch (int) {
      std::cerr << "Significance testing has not been implemented"
	" for this type yet" << endl;
    }
  }

  template <>
  void RuleByDoc<BinaryRule>::process_rule_by_doc() {
    process_rule_by_doc_simple();
  }

  template<>
  Infltrait& RuleByDoc<BinaryRule>::get_infltrait() {
    Str2IdMap<WORDID>& s2i = infltrait.get_str2id_map_nc();
    for(PPMV_PVECPMAP::iterator it = global_pp.begin();
	it != global_pp.end(); ++it) {
      const PPMV<BinaryRule>& ppmv = it->first;
      string s1 = ppmv.stem + ppmv.r->s1;
      string s2 = ppmv.stem + ppmv.r->s2;
      s2i.getId(s1);
      s2i.getId(s2);
    }
    return infltrait;
  }

  template<>
  ostream& RuleByDoc<BinaryRule>::write_stream(ostream& os) {
    set<string> _vocabulary(*vocabulary);
    for(PPMV_PVECPMAP::iterator it = global_pp.begin();
	it != global_pp.end(); ++it) {
      const PPMV<BinaryRule>& ppmv = it->first;
      string s1 = ppmv.stem + ppmv.r->s1;
      string s2 = ppmv.stem + ppmv.r->s2;
      os << s1 << "\t" << s2 << endl;
      set<string>::iterator vit = _vocabulary.find(s1);
      if(vit != _vocabulary.end())
	_vocabulary.erase(vit);
      vit = _vocabulary.find(s2);
      if(vit != _vocabulary.end())
	_vocabulary.erase(vit);
    }
    for(set<string>::iterator it = _vocabulary.begin();
	it != _vocabulary.end(); ++it) {
      os << *it << endl;      
    }
    return os;
  }

  template<>
  ostream& RuleByDoc<UnrestrictedRule>::write_stream(ostream& os) {
    set<string> _vocabulary(*vocabulary);
    for(PPMV_PVECPMAP::iterator it = global_pp.begin();
	it != global_pp.end(); ++it) {
      const PPMV<UnrestrictedRule>& ppmv = it->first;
      for(set<string>::iterator it = ppmv.r->suffixes->begin();
	  it != ppmv.r->suffixes->end(); ++it) {
	string s = ppmv.stem + *it;
	os << s << "\t";
	set<string>::iterator vit = _vocabulary.find(s);
	if(vit != _vocabulary.end())
	  _vocabulary.erase(vit);
      }
      os << endl;
    }
    for(set<string>::iterator it = _vocabulary.begin();
	it != _vocabulary.end(); ++it) {
      os << *it << endl;      
    }
    return os;
  }
}
