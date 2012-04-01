#include "ds/RuleByDocPS.h"

namespace infl {
  template <> void RuleByDocPS
  <BareBinaryRule, InflHash<BareBinaryRule>,
   ChiSquare<double,STAT_TEST_DEFAULT> >
  ::initialize_statistical_test() {
    st = new ChiSquare<double,STAT_TEST_DEFAULT>(ep.confidence);
  }

  template <> void RuleByDocPS
  <BareBinaryRule, InflHash<BareBinaryRule>,
   ChiSquare<double,STAT_TEST_DEFAULT>, EXP_USP_GS_DC>
  ::initialize_statistical_test() {
    st = new ChiSquare<double,STAT_TEST_DEFAULT>(ep.confidence);
  }

  template <> void RuleByDocPS
  <BareBinaryRule, InflHash<BareBinaryRule>,
   ChiSquare<double,STAT_TEST_DEFAULT>, EXP_USP_DS_GC >
  ::initialize_statistical_test() {
    st = new ChiSquare<double,STAT_TEST_DEFAULT>(ep.confidence);
  }

  template <> void RuleByDocPS
  <BareBinaryRule, InflHash<BareBinaryRule>,
   ChiSquare<double,STAT_TEST_DEFAULT>, EXP_USP_DS_DC>
  ::process_rule_by_doc() {
    process_rule_by_doc_base();
  }

  template <> void RuleByDocPS
  <BareBinaryRule, InflHash<BareBinaryRule>,
   ChiSquare<double,STAT_TEST_DEFAULT>, EXP_USP_GS_DC>
  ::process_rule_by_doc() {
    build_big_trie();
    RlSuffixCollector* d_suffix_collector = new RlSuffixCollector(vocabulary,ep.min_cell_size);
    RlSuffixCollector* d_prefix_collector = new RlSuffixCollector(vocabulary,ep.min_cell_size);
    build_tries(d_suffix_collector,d_prefix_collector);
    try {
      initialize_statistical_test();
      suffix_collector->calculate_significance(*st); 
      suffix_collector->expand_clusters();
      prefix_collector->calculate_significance(*st); 
      prefix_collector->expand_clusters();
      delete st; st = NULL;

      set<set<string> >& t_prefix_cluster = prefix_collector->cluster_bin;
      set<set<string> > prefix_cluster;
      set<set<string> >& suffix_cluster = suffix_collector->cluster_bin;
      // 	map<string, set<string>* > s_to_c, p_to_c;
      // 	map<string, vector<const set<string>*> > s_to_c, p_to_c;
      map<string, vector<set<set<string> >::iterator> > s_to_c, p_to_c;

      for(set<set<string> >::iterator itc = t_prefix_cluster.begin();
	  itc != t_prefix_cluster.end(); ++itc) {
	set<string> dump;
	for(set<string>::iterator its = itc->begin();
	    its != itc->end(); ++its) {
	  string word = *its;
	  reverse(word.begin(), word.end());
	  dump.insert(word);
	}
	prefix_cluster.insert(dump);
      }

      for(set<set<string> >::iterator itc = prefix_cluster.begin();
	  itc != prefix_cluster.end(); ++itc) {
	for(set<string>::iterator its = itc->begin();
	    its != itc->end(); ++its) {
	  if(p_to_c.find(*its) == p_to_c.end())
	    p_to_c[*its].push_back(itc);
	}
      }
      for(set<set<string> >::iterator itc = suffix_cluster.begin();
	  itc != suffix_cluster.end(); ++itc) {
	for(set<string>::iterator its = itc->begin();
	    its != itc->end(); ++its) {
	  s_to_c[*its].push_back(itc);
	}
      }

      for(set<string>::iterator it = vocabulary->begin();
	  it != vocabulary->end(); ++it) {
	vector<set<set<string> >::iterator>& s_to_c_vec = s_to_c[*it];
	vector<set<set<string> >::iterator>& p_to_c_vec = p_to_c[*it];
	for(size_t i = 0; i < s_to_c_vec.size(); ++i) {
	  for(size_t j = 0; j < p_to_c_vec.size(); ++j) {
	    set<string> dump;
	    set_union(s_to_c_vec[i]->begin(), s_to_c_vec[i]->end(),
		      p_to_c_vec[j]->begin(), p_to_c_vec[j]->end(),
		      inserter(dump, dump.begin()));
	    clusters.insert(dump);
	  }
	}	  
      }
    } catch (int) {
      std::cerr << "Significance testing has not been implemented"
	" for this type yet" << endl;
    }

  }

  template <> void RuleByDocPS
  <BareBinaryRule, InflHash<BareBinaryRule>,
   ChiSquare<double,STAT_TEST_DEFAULT>, EXP_USP_DS_GC>
  ::process_rule_by_doc() {
    process_rule_by_doc_base();
  }

}
