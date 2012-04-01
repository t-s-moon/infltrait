/**
 * @file   RuleByDoc.h
 * @author Taesun Moon <tsmoon@wireless-128-62-86-235.public.utexas.edu>
 * @date   Wed Mar  4 19:05:20 2009
 * 
 * @brief  
 * 
 * 
 */

#ifndef __RULE_BY_DOC_H__
#define __RULE_BY_DOC_H__

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


  template <typename Tp, typename T = size_t,
	    typename Arg = std::pair<Tp, T> >
  struct ascending : std::binary_function<Arg, Arg, bool> {
    bool operator() (const Arg& p1, const Arg& p2) {
      return p1.second < p2.second;
    }
  };

  template <typename Tp, typename T = size_t,
	    typename Arg = pair<Tp, T> >
  struct descending : std::binary_function<Arg, Arg, bool> {
    bool operator() (const Arg& p1, const Arg& p2) {
      return p1.second > p2.second;
    }
  };

  template <typename Tp, typename T = size_t,
	    typename Comp = descending<Tp, T>, typename Arg = vector<pair<Tp,T> > >
  struct pair_sorter {
    void operator() (Arg& v) {
      std::sort(v.begin(), v.end(), Comp());
    }
  };

  template <typename Rl, typename StatisticalTest>
  struct ExperimentParameters {};

  template <>
  struct ExperimentParameters<BareBinaryRule,ChiSquare<double,STAT_TEST_DEFAULT> >
  {
    size_t confidence;
    size_t min_cell_size;
    ExperimentParameters() : confidence(0),min_cell_size(5) {}
  };

  template <typename Rl = BinaryRule,
	    typename HashFunc = InflHash<Rl>,
	    typename StatisticalTest = ChiSquare<double,STAT_TEST_DEFAULT>,
	    EXP_PARAM EP = EXP_PARAM_DEFAULT>
  class RuleByDoc
    : vector< pair< typename Trie<Rl>::RULE_PMAP*,
		    typename Trie<Rl>::PPMV_PVEC*> > {
    
  public:
    typedef typename Trie<Rl>::RULE_PMAP RULE_PMAP;
    typedef typename Trie<Rl>::PPMV_PVEC PPMV_PVEC;
    typedef typename Trie<Rl>::PPMV_PSET PPMV_PSET;

    typedef vector<Rl*> RULE_PVEC;
    
    typedef pair<RULE_PMAP*, PPMV_PVEC*> RULE_PAIR;
    
    typedef PMAP<Rl, RULE_PVEC*> RULE_PVECPMAP;
    typedef PMAP<PPMV<Rl>, PPMV_PVEC*> PPMV_PVECPMAP;

    typedef Trie<Rl, HashFunc, StatisticalTest, EP> RlTrie;

  public:
    enum RTYPE {RESTRICTED, UNRESTRICTED};

  public:
    typedef vector< pair<Rl*,size_t> > RPSZ_PAIRVEC;

  public:
    RTYPE rbd_type;

  private:
    Dispenser disp;		/**<  */
    StringFilter* sf;		/**<  */

    string cwd;			/**<  */

    size_t min_stem_len;	/**< Lower limit on length of stem */
    size_t max_ppmvs;		/**<  */

    Infltrait infltrait;

  public:
    ExperimentParameters<Rl,StatisticalTest> ep;

  public:
    set<string>* vocabulary;

    RULE_PVECPMAP global_rp;	/**<  */
    PPMV_PVECPMAP global_pp;	/**<  */

    SuffixCollector<Rl,EP,StatisticalTest>* sc;

    StatisticalTest* st;

  public:
    RuleByDoc(StringFilter* _sf, const char _cwd[],
	      size_t _msl = 0, size_t _mp = size_t(-1))
      : sf(_sf), min_stem_len(_msl), max_ppmvs(_mp) {
      cwd = disp.load_dir(_cwd);
      rbd_type = RESTRICTED;

      //might want to pull this out into a separate module
      vocabulary = new set<string>;
      sc = new SuffixCollector<Rl,EP,StatisticalTest>(vocabulary,ep.min_cell_size);
    }

  RuleByDoc(){}

    ~RuleByDoc() {clear(); 
      //if pulling out allocation above, pull delete as well
      delete sc;
      delete vocabulary;
    }

    void process_rule_by_doc();

    void process_rule_by_doc_base() {
      for(size_t i = 0; i < disp.size(); ++i) {
	std::istream* is = &(disp[i]);
	RlTrie* trie = new RlTrie(sf,sc);
	trie->reset_containers();
	trie->seed_from_stream(is, vocabulary);
	insert_rule_pmap(trie->rule_pmap);
	insert_ppmv_pmap(trie->ppmv_pvec);
	trie->clear();
	trie->delete_containers();
	delete trie;
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

    void process_rule_by_doc_alt() {
      for(size_t i = 0; i < disp.size(); ++i) {
	std::istream* is = &(disp[i]);
	RlTrie* trie = new RlTrie(sf,sc);
	trie->reset_containers();
	//The following function is different for this alternative function
	trie->seed_from_stream(is, min_stem_len, max_ppmvs, vocabulary);
	insert_rule_pmap(trie->rule_pmap);
	insert_ppmv_pmap(trie->ppmv_pvec);
	trie->clear();
	trie->delete_containers();
	delete trie;
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

    void process_rule_by_doc_simple() {
      for(size_t i = 0; i < disp.size(); ++i) {
	std::istream* is = &(disp[i]);
	RlTrie* trie = new RlTrie(sf,sc);
	trie->reset_containers();
	//The following function is different for this alternative function
	trie->seed_from_stream(is, min_stem_len, max_ppmvs, vocabulary);
	insert_rule_pmap(trie->rule_pmap);
	insert_ppmv_pmap(trie->ppmv_pvec);
	trie->clear();
	trie->delete_containers();
	delete trie;
      }
      sc->expand_clusters();
    }

  private:

    void initialize_statistical_test();// {throw;}

    void insert_rule_pmap(RULE_PMAP* rp) {
      for(typename RULE_PMAP::iterator it = rp->begin();
	  it != rp->end(); ++it)
	if(global_rp.find(it->first) == global_rp.end()) {
	  global_rp[it->first] = new RULE_PVEC(1, it->second);
	} else {
	  global_rp[it->first]->push_back(it->second);
	}
    }

    void insert_ppmv_pmap(PPMV_PVEC* pp) {
      for(size_t i = 0; i < pp->size(); ++i) {
	if(global_pp.find(*(pp->at(i))) == global_pp.end()) {
	  global_pp[*(pp->at(i))] = new PPMV_PVEC(1, pp->at(i));
	} else {
	  global_pp[*(pp->at(i))]->push_back(pp->at(i));
	}
      }
    }

  public:
    enum THRESH_TYPE {COUNT, PERCENT};

  public:
    void clear() {
      for(typename RULE_PVECPMAP::iterator it = global_rp.begin();
	  it != global_rp.end(); ++it) {
	for(typename RULE_PVEC::iterator rit = it->second->begin();
	    rit != it->second->end(); ++rit)
	  delete *rit;
	delete it->second;
      }
    
      for(typename PPMV_PVECPMAP::iterator it = global_pp.begin();
	  it != global_pp.end(); ++it) {
	for(typename PPMV_PVEC::iterator pit = it->second->begin();
	    pit != it->second->end(); ++pit)
	  delete *pit;
	delete it->second;
      }
    
      //      erase(this->begin(), this->end());
    }

    void extract_top_n(RPSZ_PAIRVEC* rp, size_t n, THRESH_TYPE th) {
      double thresh;
      switch(th) {
      case COUNT:
	//thresh = n + 0.5;
	break;
      case PERCENT:
	if(n > 100) thresh = 1;
	else thresh = n / 100.0; break;	
      }

      std::vector<size_t> size_idx;
      for(typename RULE_PVECPMAP::iterator it = global_rp.begin();
	  it != global_rp.end(); ++it) {
	size_idx.push_back(it->second->size());
      }
      std::sort(size_idx.begin(), size_idx.end(), std::greater<size_t>());

      switch(th) {
      case COUNT:
	thresh = size_idx[n-1] - 0.5; break;
      case PERCENT:
	thresh = size_idx.size() * thresh;
      }

      std::vector<std::pair<Rl*,size_t> > vrp;
      for(typename RULE_PVECPMAP::iterator it = global_rp.begin();
	  it != global_rp.end(); ++it) {
	if(it->second->size() > thresh)
	  rp->push_back
	    (std::make_pair
	     (*(it->second->begin()), it->second->size()));
      }

      pair_sorter<Rl*>()(*rp);
    }
    
    void process_and_print(size_t sz = 300, THRESH_TYPE tt = COUNT) {
      process_rule_by_doc();
      RPSZ_PAIRVEC rpsz;
      extract_top_n(&rpsz, sz, tt);
  
      for(size_t i = 0; i < rpsz.size(); ++i) {
	Rl* r = rpsz[i].first;
	cout << *r << endl;
	for(typename RULE_PVEC::iterator it = global_rp[*r]->begin();
	    it != global_rp[*r]->end(); ++it) {
	  for(typename PPMV_PSET::iterator pit = (*it)->ppmv_pset->begin();
	      pit != (*it)->ppmv_pset->end(); ++pit) {
	    cout << **pit << "\t";
	  }
	  cout << endl;
	}
	cout << "\tcount: " << rpsz[i].second << endl;
      }
    }

    template <typename Test>
    void set_test(int i) {
      delete st;
      ep.confidence = i;
      st = new Test(ep.confidence);
    }
    
  public:
    inline RULE_PVECPMAP& get_rp() {return global_rp;}
    inline PPMV_PVECPMAP& get_pp() {return global_pp;}

    set<string>& get_vocabulary() {return *vocabulary;}

    ostream& write_stream(ostream&os) {
      //      set<string> _vocabulary(*vocabulary);
      sc->write_stream(os);
      return os;
    }

    ostream& write_debug_stream(ostream&os) {
      sc->write_clusters(os);
      sc->calculate_significance_all(*st);
      sc->write_stream_all(os);
      return os;
    }

    Infltrait& get_infltrait() {throw;}
    
  public:
    //    friend ostream& operator<<(ostream&,const RuleByDoc&);
  };

  typedef RuleByDoc<BareBinaryRule, InflHash<BareBinaryRule>,
    ChiSquare<double,STAT_TEST_DEFAULT>,
    EXP_DS_DC> RBD_DS_DC; 

  typedef RuleByDoc<BareBinaryRule, InflHash<BareBinaryRule>,
    ChiSquare<double,STAT_TEST_DEFAULT>,
    EXP_GS_DC> RBD_GS_DC; 

  typedef RuleByDoc<BareBinaryRule, InflHash<BareBinaryRule>,
    ChiSquare<double,STAT_TEST_DEFAULT>,
    EXP_DS_GC> RBD_DS_GC; 
}
#endif
