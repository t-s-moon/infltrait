/**
 * @file   trie.h
 * @author Taesun Moon <tsmoon@wireless-128-62-123-101.public.utexas.edu>
 * @date   Fri Feb  6 22:16:23 2009
 * 
 * @brief  TRIE data structure implementation for storage and
 * retrieval of string objects
 * 
 */

#ifndef __TRIE_H__
#define __TRIE_H__

#include <algorithm>
#include <fstream>
#include <set>
#include <string>
#include <vector>

#include "ds/PPMV.h"
#include "ds/SuffixCollector.h"

#include "io/StringFilter.h"

namespace infl {

  using std::set;
  using std::string;
  using std::vector;

  typedef std::set<string> WORDBAG;

  template <typename Rl = BinaryRule,
	    typename HashFunc = InflHash<Rl>,
	    typename StatisticalTest = ChiSquare<double,STAT_TEST_DEFAULT>,
	    EXP_PARAM EP = EXP_PARAM_DEFAULT>
  struct Trie {
  public:
    typedef PMAP<Rl> RULE_PMAP;
    typedef set<PPMV<Rl>*> PPMV_PSET;
    typedef vector<PPMV<Rl>*> PPMV_PVEC;
    
  public:
    char key;
    size_t count; /// Keeps track of how many times a certain string
    /// token occurred up to current trie node.
    size_t type_count; /// Keeps track of the number of types that share
    /// particular trie node
    Trie* child;	/// Pointer to child
    Trie* sibling;	/// Pointer to sibling which keeps track of
			/// bifurcations or non-matches in string
			/// types
    Trie* parent;	/// Pointer to parent
    Trie* terminal;	/// False at instantiation. Indicates whether
			/// node in trie is a terminal node for some
			/// string.

    const static char EOS = '\0'; /// String termination character

    StringFilter* sf;

  public:
    static RULE_PMAP* rule_pmap;
    static PPMV_PVEC* ppmv_pvec;

    static SuffixCollector<Rl,EP,StatisticalTest>* sc;
    static SuffixCollector<Rl,EP,StatisticalTest>* pc;	/**< Prefix
							   collector */

  public:
    Trie() {init();}
    Trie(StringFilter* _sf, SuffixCollector<Rl,EP,StatisticalTest>* _sc)
      : sf(_sf) {init(); sc = _sc;}

    virtual ~Trie(){}

    void init() {
      key = EOS;
      count = 0;
      type_count = 0;
      child = NULL;
      sibling = NULL;
      parent = NULL;
      terminal = NULL;
    }

    size_t get_count(const string& word) {
      size_t i = 0;
      Trie* current = this;
      Trie* last_parent;
  
      while(i < word.size()) {
	if(current->key == word[i]) {
	  last_parent = current;
	  current = current->child;
	  i++;
	} else {
	  if(current->sibling != NULL)
	    current = current->sibling;
	  else
	    return 0;
	}
      }
  
      return last_parent->count;
    }
    
    size_t get_type_count(const string& word) {
      size_t i = 0;
      Trie* current = this;
      Trie* last_parent;
  
      while(i < word.size()) {
	if(current->key == word[i]) {
	  last_parent = current;
	  current = current->child;
	  i++;
	} else {
	  if(current->sibling != NULL)
	    current = current->sibling;
	  else
	    return 0;
	}
      }
  
      return last_parent->type_count;
    }
    
    void set_first(const string& word, size_t num = 1) {
      unsigned int i = 0;
      Trie* current = this;
      Trie* prev;
      while(i < word.size()) {
	prev = current;
	current->key = word[i++];
	current->count += num;
	current->type_count++;
	current->child = new Trie;
	current = current->child;
	current->parent = prev;
      }
      prev->terminal = new Trie;
      current = prev->terminal;
      current->count += num;
      current->type_count++;
    }
    
    void insert(const string& word, size_t num = 1) {
      size_t out = 0, i = 0;
      char letter = word[i];
      Trie* current = this;            //we start with the first node
      Trie* prev = NULL;
      while(i < word.size() && out == 0) {  
	if (letter == current->key) {
	  prev = current;
	  letter = word[++i];
	  if(letter == EOS) {
	    if(current->terminal == NULL)
	      current->terminal = new Trie;
	    current = current->terminal;
	    current->count += num;
	    current->type_count++;
	    return;
	  }
	  current->count += num;
	  current->type_count++;
	  current = current->child;           //follow the child
	} else {
	  if (current->sibling != NULL) {
	    current = current->sibling;
	  } else {
	    out=1;                         //no sibling, we add new node
	  }
	}
      }


      //       if (out == 0) {
      // 	if(current->key != EOS) {
      // 	  if(current->terminal == NULL)
      // 	    current->terminal = new Trie;
      // 	  current = current->terminal;
      // 	  current->count += num;
      // 	  current->type_count++;
      // 	}
      // 	return;   //if the word was already there, exit
      //       }
      // otherwise add a new node with the current letter
	
      //CREATE new NODE (letter);
      current->sibling = new Trie;
      current = current->sibling;
      current->parent = prev;
      // and move on to append the rest of the letters

      while(i < word.size())
	{
	  prev = current;
	  current->key = word[i++];
	  current->count += num;
	  current->type_count++;
	  current->child = new Trie;
	  current = current->child;
	  current->parent = prev;
	}
      if(prev->terminal == NULL)
	prev->terminal = new Trie;
      current = prev->terminal;
      current->count += num;
      current->type_count++;
    }
    
    void clearall() {
      for(typename RULE_PMAP::iterator it = rule_pmap->begin();
	  it != rule_pmap->end(); ++it) {
	for(typename PPMV_PSET::iterator itp = (it->second->ppmv_pset)->begin();
	    itp != (it->second->ppmv_pset)->end(); ++itp) {
	  delete *itp;
	}
	delete it->second;
      }
      clear();
    }
    
    void clear() {
      if(sibling != NULL) {
	sibling->clear();
	delete sibling;
	sibling = NULL;
      }

      if(child != NULL) {
	if(child->key == EOS) {
	  delete child;
	  child = NULL;
	  key = EOS;
	} else {
	  child->clear();
	}
      }
    
      if(terminal != NULL) {
	delete terminal;
	terminal = NULL;
      }
    }

    void find_suffixes(Trie*,WORDBAG&);
    void find_suffixes(Trie* branch_point,WORDBAG& wb,
		       size_t stem_length,size_t current_depth=0) {
      if (current_depth >= stem_length) {
	throw 0;
      }
      string s;
      //      if(key == EOS || terminal != NULL) {
      if(terminal != NULL) {
	get_string_from_node(s, branch_point);
	std::reverse(s.begin(), s.end());
	wb.insert(s);
      } 
      if(sibling != NULL) {
	sibling->find_suffixes(branch_point, wb,stem_length,current_depth);
	// 	sibling->find_suffixes(branch_point, wb,stem_length,++current_depth);
      }
      if(child != NULL) {
	child->find_suffixes(branch_point, wb,stem_length,current_depth + 1);
	// 	child->find_suffixes(branch_point, wb,stem_length,++current_depth);
      }
    }
    
    void get_string_from_node(string& s,Trie* branch_point = NULL) {
      //       if(parent != branch_point) {
      // 	s += parent->key;
      // 	parent->get_string_from_node(s, branch_point);
      //       } 
      if(this != branch_point) {
	if(key != EOS)
	  s += key;
	parent->get_string_from_node(s, branch_point);
      } 
    }

    void partition_ppmv(size_t _n = PPMV_MIN_COUNT) {
      typename PPMV_PVEC::iterator it =
	std::partition(ppmv_pvec->begin(), ppmv_pvec->end(),
		       std::bind2nd(is_less_than<>(),_n));
      ppmv_pvec->erase(it, ppmv_pvec->end());
#ifdef MDEBUG
      for(unsigned int i = 0; i < ppmv_pvec->size(); ++i) {
	cout << *(ppmv_pvec->at(i)) << endl;
      }
#endif
      std::sort(ppmv_pvec->begin(), ppmv_pvec->end(), ppmv_cmp<>());
#ifdef MDEBUG
      cout << endl << endl << endl << "Printing sorted results:" << endl;
      for(unsigned int i = 0; i < ppmv_pvec->size(); ++i) {
	cout << *(ppmv_pvec->at(i)) << endl;
      }
#endif
    }

    void partition_ppmv_by_rule(size_t _n = RULE_MAX_COUNT) {throw;}
    
    void seed_from_stream
    (std::istream* is,size_t min_stem_len,
     size_t max_ppmvs, set<string>* vocabulary = NULL) {
      string current_word, prev_word;
      int counter = 0;
      while(*is >> current_word) {
	if(sf->filter(current_word) != "") {
	  set_first(current_word);
	  if(vocabulary != NULL)
	    vocabulary->insert(current_word);
	  break;
	}
      }
      counter++;
      while(*is >> current_word) {
	if(sf->filter(current_word) != "") {
	  insert(current_word);
	  if(vocabulary != NULL)
	    vocabulary->insert(current_word);
	}
      }
      find_stems(0,min_stem_len);
    }

    void seed_from_stream
    (std::istream* is, set<string>* vocabulary = NULL) {
      string current_word, prev_word;
      int counter = 0;
      while(*is >> current_word) {
	if(sf->filter(current_word) != "") {
	  set_first(current_word);
	  if(vocabulary != NULL)
	    vocabulary->insert(current_word);
	  break;
	}
      }
      counter++;
      while(*is >> current_word) {
	if(sf->filter(current_word) != "") {
	  insert(current_word);
	  if(vocabulary != NULL)
	    vocabulary->insert(current_word);
	}
      }
      find_stems(0);
    }

    void find_stems(size_t,size_t);
    void find_stems(size_t,size_t,set<set<string> >&,set<string>&);
    void find_stems(size_t current_depth=0) {
      WORDBAG suffixes;
      string stem;
      parent->get_string_from_node(stem);
      std::reverse(stem.begin(), stem.end());
      try {
	if(current_depth > 1) {
	  if(parent->terminal != NULL || sibling != NULL) {
	    if(parent->terminal != NULL)
	      suffixes.insert("");
	    find_suffixes(parent, suffixes,stem.size(),1);
	    // 	    if(child != NULL)
	    // 	      child->find_suffixes(parent, suffixes,stem.size(),1);
	    // 	    if(sibling != NULL)
	    // 	      sibling->find_suffixes(parent, suffixes,stem.size(),1);
	    if(suffixes.size() > 1) {
	      for(WORDBAG::const_iterator it1 = suffixes.begin();
		  it1 != suffixes.end(); ++it1) {
		sc->add(*it1,stem);
		WORDBAG::const_iterator it2 = it1; ++it2;
		for(;it2 != suffixes.end(); ++it2) {
		  if(sc != NULL)
		    sc->add(*it1, *it2, stem);
		}
	      }
	    }
	  }
	}
      } catch (int) {}
      if(child != NULL)
	child->find_stems(current_depth + 1);
      if(sibling != NULL)
	sibling->find_stems(current_depth);
    }
    
    void delete_containers() {
      if(Trie::rule_pmap != NULL)
	delete Trie::rule_pmap;
      Trie::rule_pmap = NULL;
      if(Trie::ppmv_pvec != NULL)
	delete Trie::ppmv_pvec;
      Trie::ppmv_pvec = NULL;
    }
    
    void reset_containers() {
      Trie::rule_pmap = new RULE_PMAP;
      Trie::ppmv_pvec = new PPMV_PVEC;
    }

    ostream& graphviz_print(ostream& os, size_t uid=0) {
      os << "digraph " << uid << " {" << endl;
      os << "0 [label=\"ROOT\"];" << endl;
      graphviz_print(os, (Trie*) 0);
      os << "}" << endl;
      return os;
    }

    ostream& graphviz_print(ostream& os, Trie* tr) {
      os << "\t" << (size_t) this
	 << "[label=\"" << ((key == EOS) ? '$' : key) << "\"];" << endl;
      if(terminal != NULL && key != EOS && child->key != EOS) {
	os << "\t" << (size_t) terminal
	   << "[label=\"$\"];" << endl;
	os << "\t" << (size_t) this << " -> "
	   << (size_t) terminal << ";" << endl;
      }
      if(child != NULL) {
	//	if(child->key != EOS || (child->key == EOS && child->sibling != NULL))
	if(child->key != EOS || child->sibling != NULL)
	  child->graphviz_print(os,this);
      }
      if(sibling != NULL)
	sibling->graphviz_print(os,parent);
      os << "\t" << (size_t) tr << " -> "
	 << (size_t) this << ";" << endl;
      return os;
    }
  };

  template <typename Rl, typename HashFunc, typename StatisticalTest, EXP_PARAM EP>
  typename Trie<Rl,HashFunc,StatisticalTest,EP>::RULE_PMAP*
  Trie<Rl,HashFunc,StatisticalTest,EP>::rule_pmap = NULL;

  template <typename Rl, typename HashFunc,typename StatisticalTest, EXP_PARAM EP>
  typename Trie<Rl,HashFunc,StatisticalTest,EP>::PPMV_PVEC*
  Trie<Rl,HashFunc,StatisticalTest,EP>::ppmv_pvec = NULL;
  
  template <typename Rl, typename HashFunc,typename StatisticalTest, EXP_PARAM EP>
  SuffixCollector<Rl,EP,StatisticalTest>*
  Trie<Rl,HashFunc,StatisticalTest,EP>::sc = NULL;

  template <typename Rl, typename HashFunc,typename StatisticalTest, EXP_PARAM EP>
  SuffixCollector<Rl,EP,StatisticalTest>*
  Trie<Rl,HashFunc,StatisticalTest,EP>::pc = NULL;
}

#endif
