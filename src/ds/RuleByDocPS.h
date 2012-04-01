/**
 * @file   RuleByDocPS.h
 * @author Taesun Moon <tsmoon@wireless-128-62-86-235.public.utexas.edu>
 * @date   Wed Mar  4 19:05:20 2009
 * 
 * @brief  RuleByDoc structure for languages that both prefix AND suffix
 * 
 * 
 */

#ifndef __RULE_BY_DOC_PS_H__
#define __RULE_BY_DOC_PS_H__

#include <fstream>
#include <iostream>
#include <set>
#include <streambuf>
#include <string>
#include <vector>
#include <utility>

#include "ds/Infltrait.h"
#include "ds/PPMV.h"
#include "ds/SuffixCollector.h"
#include "ds/Trie.h"
#include "ds/RuleByDoc.h"

#include "io/IO.h"
#include "io/StringFilter.h"

namespace infl {

  using std::cout;
  using std::endl;
  using std::ostream;
  using std::pair;
  using std::set;
  using std::streambuf;
  using std::string;
  using std::vector;

  template <typename Rl = BareBinaryRule,
	    typename HashFunc = InflHash<Rl>,
	    typename StatisticalTest = ChiSquare<double,STAT_TEST_DEFAULT>,
	    EXP_PARAM EP = EXP_USP_DS_DC>
  class RuleByDocPS {
  public:
    typedef Trie<Rl, HashFunc, StatisticalTest, EP> RlTrie;
    typedef SuffixCollector<Rl,EP,StatisticalTest> RlSuffixCollector;
  private:
    Dispenser disp;		/**<  */
    StringFilter* suffix_filter;		/**<  */
    StringFilter* prefix_filter;		/**<  */

    string cwd;			/**<  */
    size_t min_stem_len;	/**< Lower limit on length of stem */

  public:
    ExperimentParameters<Rl,StatisticalTest> ep;

  public:
    std::ifstream bigfile;

  public:
    set<set<string> > clusters;
    set<string>* vocabulary, *prefix_vocabulary;
    RlSuffixCollector* prefix_collector;
    RlSuffixCollector* suffix_collector;
    StatisticalTest* st;

  public:
    RuleByDocPS(const char _cwd[], size_t _msl = 0)
      : min_stem_len(_msl) {
      init(_cwd);
    }

    RuleByDocPS(const char _cwd[],
		const char _big_file[], size_t _msl = 0)
      : min_stem_len(_msl) {
      init(_cwd);
      bigfile.open(_big_file);
    }

    RuleByDocPS(){}

    ~RuleByDocPS() { 
      delete suffix_collector;
      delete prefix_collector;
      delete suffix_filter;
      delete prefix_filter;
      delete vocabulary;
      delete prefix_vocabulary;
    }

    void init(const char _cwd[]) {
      cwd = disp.load_dir(_cwd);
      suffix_filter = new UspFilter;
      prefix_filter = new ReverseUspFilter;
      vocabulary = new set<string>;
      prefix_vocabulary = new set<string>;
      suffix_collector =
	new RlSuffixCollector(vocabulary,ep.min_cell_size);
      prefix_collector =
	new RlSuffixCollector(prefix_vocabulary,ep.min_cell_size);
    }

    void process_rule_by_doc();

    void build_big_trie() {
      RlTrie* trie = new RlTrie(suffix_filter,suffix_collector);
      trie->reset_containers();
      trie->seed_from_stream(&bigfile, vocabulary);
      trie->clear();
      trie->delete_containers();
      delete trie;
      bigfile.clear();
      bigfile.seekg(0,std::ios::beg);
      trie = new RlTrie(prefix_filter,prefix_collector);
      trie->reset_containers();
      trie->seed_from_stream(&bigfile, prefix_vocabulary);
      trie->clear();
      trie->delete_containers();
      delete trie;
    }

    void build_tries(RlSuffixCollector* _sc,
		     RlSuffixCollector* _pc) {
      for(size_t i = 0; i < disp.size(); ++i) {
	std::istream* is = &(disp[i]);
	RlTrie* trie = new RlTrie(suffix_filter,_sc);
	trie->reset_containers();
	trie->seed_from_stream(is, vocabulary);
	trie->clear();
	trie->delete_containers();
	delete trie;
	is->clear();
	is->seekg(0,std::ios::beg);
	trie = new RlTrie(prefix_filter,_pc);
	trie->reset_containers();
	trie->seed_from_stream(is, prefix_vocabulary);
	trie->clear();
	trie->delete_containers();
	delete trie;
      }
    }

    void print_clust(ostream&os, map<string,set<string>*> t) {
      set<set<string>*> done;
      for(map<string,set<string>*>::iterator it = t.begin();
	  it != t.end(); ++it) {
	if(done.find(it->second) == done.end()) {
	  done.insert(it->second);
	  for(set<string>::iterator meat = it->second->begin();
	      meat != it->second->end(); ++meat) {
	    os << *meat << " ";
	  }
	  os << endl;
	}
      }
    }

    void process_rule_by_doc_base() {
      build_tries(suffix_collector,prefix_collector);
      try {
	initialize_statistical_test();
	suffix_collector->calculate_significance(*st); 
	suffix_collector->expand_clusters();
	prefix_collector->calculate_significance(*st); 
	prefix_collector->expand_clusters();
	delete st; st = NULL;

	map<string,set<string>*> ss1, ss2;
// 	set<set<string> >& t_prefix_cluster = prefix_collector->cluster_bin;
	prefix_collector->flip();
	set<set<string> >& prefix_cluster = prefix_collector->cluster_bin;
	disjoint_clusters(prefix_cluster,ss1);
	set<set<string> >& suffix_cluster = suffix_collector->cluster_bin;
	disjoint_clusters(prefix_cluster,ss2);
// 	print_clust(ofs1,ss1);
// 	print_clust(ofs2,ss2);
// 	map<string, set<string>* > s_to_c, p_to_c;
// 	map<string, vector<const set<string>*> > s_to_c, p_to_c;
  	map<string, vector<set<set<string> >::iterator> > s_to_c, p_to_c;

// 	for(set<set<string> >::iterator itc = t_prefix_cluster.begin();
// 	    itc != t_prefix_cluster.end(); ++itc) {
// 	  set<string> dump;
// 	  for(set<string>::iterator its = itc->begin();
// 	      its != itc->end(); ++its) {
// 	    string word = *its;
// 	    reverse(word.begin(), word.end());
// 	    dump.insert(word);
// 	  }
// 	  prefix_cluster.insert(dump);
// 	}

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

  private:

    void initialize_statistical_test();

    template <typename Test>
    void set_test(int i) {
      delete st;
      ep.confidence = i;
      st = new Test(ep.confidence);
    }

  public:
//     void expand_chain(set<string*>& ss, map<string,set<string*> >& chain,
// 		      map<string,string*>&sid) {
//       for(set<string*>::iterator it = ss.begin(); it != ss.end(); ++it) {
// 	expand_chain(ss,chain,sid);
//       }
//     }

    void disjoint_clusters(set<set<string> >& sss,map<string,set<string>*>&tc) {
      map<string,string*> sid;
      for(set<set<string> >::iterator it = sss.begin(); it != sss.end(); ++it) {
	for(set<string>::iterator its = it->begin(); its != it->end(); ++its) {
	  if(tc.find(*its) == tc.end())
	    tc[*its] = new set<string>;
	  tc[*its]->insert(*its);
	  set<string>::iterator itsi = it->begin(); ++itsi;
	  for(; itsi != it->end(); ++itsi) {
	    if(tc.find(*itsi) == tc.end()) {
	      tc[*itsi] = tc[*its];
	    } else if (tc[*itsi] != tc[*its]) {
	      set<string>* no_touch = tc[*its];
	      set<string>* touch = tc[*itsi];
	      for(set<string>::iterator other_it = touch->begin();
		  other_it != touch->end(); ++other_it) {
		no_touch->insert(*other_it);
		if(tc[*other_it] != no_touch) {
		  //		  delete tc[*other_it];
		  tc[*other_it] = no_touch;
		}
	      }
	      delete touch;
	    }
	  }	  
// 	  if(sid.find(*its) == sid.end())
// 	    sid[*its] = new string(*its);
	}
      }
//       map<string,set<string*> > s_chain;
//       for(set<set<string> >::iterator it = sss.begin(); it != sss.end(); ++it) {
// 	for(set<string>::iterator its = it->begin(); its != it->end(); ++its) {
// 	  for(set<string>::iterator itsi = it->begin(); itsi != it->end(); ++itsi) {
// 	    s_chain[*its].insert(sid[*itsi]);
// 	  }
// 	}
//       }
//       map<string,set<string*> > f_cluster;
    }
    
  public:

    ostream& write_stream(ostream&os) {
      for(set<set<string> >::iterator it = clusters.begin();
	  it != clusters.end(); ++it) {
	for(set<string>::iterator its = it->begin();
	    its != it->end(); ++its) {
	  os << *its << " ";
	}
	os << endl;
      }
//       suffix_collector->write_stream(os);
      return os;
    }

    ostream& write_debug_stream(ostream&os) {
//       suffix_collector->write_clusters(os);
//       suffix_collector->calculate_significance_all(*st);
//       suffix_collector->write_stream_all(os);
      return os;
    }

  };

  typedef RuleByDocPS<BareBinaryRule, InflHash<BareBinaryRule>,
    ChiSquare<double,STAT_TEST_DEFAULT>,
    EXP_USP_DS_DC> RBD_USP_DS_DC; 

  typedef RuleByDocPS<BareBinaryRule, InflHash<BareBinaryRule>,
    ChiSquare<double,STAT_TEST_DEFAULT>,
    EXP_USP_GS_DC> RBD_USP_GS_DC; 

  typedef RuleByDocPS<BareBinaryRule, InflHash<BareBinaryRule>,
    ChiSquare<double,STAT_TEST_DEFAULT>,
    EXP_USP_DS_GC> RBD_USP_DS_GC; 
}
#endif
