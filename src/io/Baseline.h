/**
 * @file   Baseline.h
 * @author Taesun Moon <tsmoon@wireless-128-62-86-124.public.utexas.edu>
 * @date   Mon Apr 20 21:39:08 2009
 * 
 * @brief  
 * 
 * 
 */

#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>
#include <string>

#include "Defs.h"

#include "ds/Trie.h"

#include "io/IO.h"
#include "io/StringFilter.h"

namespace infl {

  using std::set;
  using std::string;

  template <EXP_PARAM EP>
  struct Baseline {};

  struct Processor {
    Dispenser* disp;
    set<string>* wb;
    StringFilter* sf;
    Processor(Dispenser* _d, set<string>* _w, StringFilter* _s)
      : disp(_d), wb(_w), sf(_s) {}

    void process() {
      for(size_t i = 0; i < disp->size(); ++i) {
	std::istream* is = &((*disp)[i]);
	string current_word;
	while(*is >> current_word) {
	  if(sf->filter(current_word) != "") {
	    wb->insert(current_word);
	  }
	}
      }
    }
  };

  template <>
  struct Baseline<BASELINE_ALL_SINGLETON> {
    Dispenser disp;		/**<  */
    StringFilter* sf;
    set<string> wb;

    Baseline(StringFilter* _sf, const char _c[])
      : sf(_sf) {
      disp.load_dir(_c);
    }

    void process() {
      Processor proc(&disp, &wb, sf);
      proc.process();
    }

    ostream& write_stream(ostream& os) {
      std::copy(wb.begin(), wb.end(),
		std::ostream_iterator<string>(os, "\n"));
      return os;
    }
  };

  template <>
  struct Baseline<BASELINE_ALL_IN_ONE> {
    Dispenser disp;		/**<  */
    StringFilter* sf;
    set<string> wb;

    Baseline(StringFilter* _sf, const char _c[])
      : sf(_sf) {
      disp.load_dir(_c);
    }

    void process() {
      Processor proc(&disp, &wb, sf);
      proc.process();
    }

    ostream& write_stream(ostream& os) {
      std::copy(wb.begin(), wb.end(),
		std::ostream_iterator<string>(os, " "));
      return os;
    }
  };

  template <>
  struct Baseline<BASELINE_FIRST_K> {
    typedef Trie<
      BaselineRule,InflHash<BaselineRule>,
      ChiSquare<double,STAT_TEST_DEFAULT>,
      BASELINE_FIRST_K>	RlTrie;

    Dispenser disp;		/**<  */
    StringFilter* sf;
    RlTrie trie;
    set<string> wb;
    size_t min_len;
    set<set<string> > clusters;

    Baseline(StringFilter* _sf, const char _c[])
      : sf(_sf) {
      disp.load_dir(_c);
    }

    void process() {
      trie.sf = sf;
      trie.reset_containers();
      trie.set_first("a");
      string current_word;
      for(size_t i = 0; i < disp.size(); ++i) {
	std::istream* is = &(disp[i]);
	while(*is >> current_word) {
	  if(sf->filter(current_word) != "")
	    trie.insert(current_word);
	}
      }
      trie.find_stems(0,min_len, clusters, wb);
      trie.clear();
      trie.delete_containers();
    }

    void flip() {
      set<set<string> > tc;
      std::swap(tc,clusters);
      for(set<set<string> >::iterator it = tc.begin();
	  it != tc.end(); ++it) {
	set<string> dump;
	for(set<string>::iterator its = it->begin();
	    its != it->end(); ++its) {
	  string st = *its;
	  std::reverse(st.begin(), st.end());
	  dump.insert(st);
	}
	clusters.insert(dump);
      }      

      set<string> _wb;
      std::swap(_wb,wb);
      for(set<string>::iterator it = _wb.begin();
	  it != _wb.end(); ++it) {
	string st = *it;
	std::reverse(st.begin(), st.end());
	wb.insert(st);
      }
    }
    
    ostream& write_stream(ostream& os) {
      for(set<set<string> >::iterator it = clusters.begin();
	  it != clusters.end(); ++it) {
	std::copy(it->begin(), it->end(),
		  std::ostream_iterator<string>(os, " "));
	os << endl;
      }
      return os;
    }
  };
}
