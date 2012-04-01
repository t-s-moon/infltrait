/**
 * @file   ppmv.h
 * @author Taesun Moon <tsmoon@wireless-128-62-123-101.public.utexas.edu>
 * @date   Sat Feb  7 18:59:13 2009
 * 
 * @brief  
 * 
 * 
 */

#ifndef __PPMV_H__
#define __PPMV_H__

#include <algorithm>
#include <cstdio>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "Defs.h"

#include "ds/HashDef.h"

namespace infl {

  using std::ostream;
  using std::set;
  using std::string;
  using std::vector;

  const unsigned int PPMV_MIN_LEN = 4;
  const unsigned int PPMV_MIN_COUNT = 2;
  const unsigned int RULE_MAX_COUNT = 200;

  inline string isnull(const string& s) {
    return s == "" ? "NULL" : s;
  }

  template<typename Rl>
  struct PPMV {
    string stem;
    Rl* r;
    double ncs;
    PPMV(const string& _s, Rl* _r) : stem(_s), r(_r), ncs(0) {}
    PPMV(const PPMV& ppmv) {(*this) = ppmv;}
    PPMV& operator=(const PPMV& ppmv) {
      if(this != &ppmv) {stem=ppmv.stem;r=ppmv.r;ncs=ppmv.ncs;}
      return *this;
    }
    bool operator==(const PPMV& p) const {
      return stem == p.stem && (*r) == (*p.r);}
  };

  struct RuleBase {
    unsigned int count;
    virtual ~RuleBase() {}
    RuleBase& operator++() {++count; return *this;}
    virtual void init() {count = 1;}
    virtual ostream& output_to_stream(ostream&os) const {throw;}
    virtual string get_suffix_string() const {throw;}
    virtual RuleBase& operator=(const RuleBase& r) {throw;}
    virtual bool operator==(const RuleBase& r) const {throw;}
  };
  
  struct BinaryRule : public RuleBase {
    string s1, s2;	/// Suffixes one and two
    set<PPMV<BinaryRule>*>* ppmv_pset;
    BinaryRule(const BinaryRule& r) {(*this) = r;}
    BinaryRule() {init();}
    BinaryRule(const string& _s1,  const string& _s2)
      : s1(_s1), s2(_s2) {
      if(s2 < s1) std::swap(s1, s2);
      init();
    }
    ~BinaryRule() {if(ppmv_pset != NULL) delete ppmv_pset;}
    void init(){RuleBase::init();ppmv_pset = new std::set<PPMV<BinaryRule>*>;}
    ostream& output_to_stream(ostream& os) const {
      char c[1024]; string s = isnull(s1);
      sprintf(c, "suf1:%s\tsuf2:%s\tcount:%d\tp_count:%d",
	      s.c_str(), s2.c_str(), count,
	      static_cast<unsigned int>((ppmv_pset)->size()));
      os << c; return os;
    }
    string get_suffix_string() const {
      return s1 < s2 ? s1 + " " + s2 : s2 + " " + s1;}
    BinaryRule& operator=(const BinaryRule& r) {
      if(this != &r) {s1=r.s1;s2=r.s2;ppmv_pset=NULL;} return *this;
    }
    bool operator==(const BinaryRule& r) const {
      return s1 == r.s1 && s2 == r.s2;
    }
  };

  struct BaselineRule : public RuleBase {
  };

  struct BareBinaryRule : public RuleBase {
    string s1, s2;
    BareBinaryRule() {}
    BareBinaryRule(const string& _s1,  const string& _s2)
      : s1(_s1), s2(_s2) {
      if(s2 < s1) std::swap(s1, s2);
    }
    ostream& output_to_stream(ostream& os) const {
      string s = isnull(s1);
      os << "suf1:" << s << "\tsuf2:" << s2; return os;
    }
    string get_suffix_string() const {
      return s1 < s2 ? s1 + " " + s2 : s2 + " " + s1;}
    BareBinaryRule& operator=(const BareBinaryRule& r) {
      if(this != &r) {s1=r.s1;s2=r.s2;} return *this;
    }
    bool operator==(const BareBinaryRule& r) const {
      return s1 == r.s1 && s2 == r.s2;
    }
  };

  struct UnrestrictedRule : public RuleBase {
    std::set<string>* suffixes;
    std::set<PPMV<UnrestrictedRule>*>* ppmv_pset;
    bool is_copy;
    UnrestrictedRule(const UnrestrictedRule& r) {(*this) = r;}
    UnrestrictedRule(set<string>&ss) {init();
      std::copy(ss.begin(), ss.end(), std::inserter(*suffixes,suffixes->begin()));}
    UnrestrictedRule(const string& _s1,  const string& _s2) {init();
      suffixes->insert(_s1); suffixes->insert(_s2);}
    UnrestrictedRule() {init();}
    ~UnrestrictedRule() {if(!is_copy) {delete ppmv_pset;delete suffixes;}}
    void init() {RuleBase::init();suffixes=new std::set<string>;
      ppmv_pset = new std::set<PPMV<UnrestrictedRule>*>;is_copy=false;}
    ostream& output_to_stream(ostream& os) const {
      os << "suffixes(count:" << count
	 << " p_count:" << ppmv_pset->size() << ")\t" ;
      for(set<string>::const_iterator it = suffixes->begin();
	  it != suffixes->end(); ++it)
	os << *it << " "; return os;}
    string get_suffix_string() const {
      string sstring;
      for(set<string>::const_iterator it = suffixes->begin();
	  it != suffixes->end(); ++it)
	sstring += *it + " ";
      return sstring;}
    UnrestrictedRule& operator++() {++count; return *this;}
    UnrestrictedRule& operator=(const UnrestrictedRule& r) {
      if(this != &r) {count=r.count;is_copy=true;
	suffixes=r.suffixes;ppmv_pset=r.ppmv_pset;}
      return *this;}
    bool operator==(const UnrestrictedRule& r) const {
      return (*suffixes) == (*r.suffixes);}
  };

  template<typename T1 = PPMV<BinaryRule>*,
	   typename T2 = unsigned int, typename B = bool>
  struct is_less_than : public std::binary_function<T1,T2,B> {
    B operator()(const T1 p, const T2 theta) const {
      return (p->r)->count >= theta;
    }
  };

  template<typename T = PPMV<BinaryRule>*, typename B = bool>
  struct ppmv_cmp : public std::binary_function<T,T,B> {
    B operator() (const T p1, const T p2) {
      return (p1->r)->count > (p2->r)->count;
    }
  };

  template<typename Rl>
  struct RuleCmp {
    typedef Rl first_argument_type; typedef Rl second_argument_type; typedef bool result_type;
    result_type operator() (const first_argument_type* r1, const second_argument_type* r2) {
      return r1->count > r2->count;
    }
  };
  
//   template <typename Rl>
//   struct InflHash {};
  template <typename Rl>
  struct InflHash {
    size_t operator() (const Rl& r) const {
      return chash()(r.get_suffix_string().c_str());
    }
  };

//   template <>
//   struct InflHash<BinaryRule> {
//     size_t operator() (const BinaryRule& r) const {
//       return chash()(r.get_suffix_string().c_str());
//     }
//   };

//   template <>
//   struct InflHash<UnrestrictedRule> {
//     size_t operator() (const UnrestrictedRule& r) const {
//       return chash()(r.get_suffix_string().c_str());
//     }
//   };

  template<typename Rl>
  struct InflHash<PPMV<Rl> > {
    size_t operator() (const PPMV<Rl>& p) const {
      return chash()( (p.stem + " " + p.r->get_suffix_string()).c_str() );
    }
  };

//   struct BinaryRuleHash : public InflHash<BinaryRule> {};
//   struct UnrestrictedRuleHash : public InflHash<UnrestrictedRule> {};
//   struct PPMV_Hash : public InflHash<PPMV<RuleBase> > {};

  template <typename C>
  class PVEC : public vector<C*> {
    PVEC(size_t n, C* c) : vector<C*>(n,c) {}
  };
  
  template <typename C>
  class PSET : public set<C*> {};

#ifdef SGIHASH
  template <typename C, typename Cp = C*, typename HashFunc = InflHash<C> >
  class PMAP : public __gnu_cxx::hash_map<C, Cp, HashFunc> {};
#else
  template <typename C, typename Cp = C*, typename HashFunc = InflHash<C> >
  class PMAP : public std::tr1::unordered_map<C, Cp, HashFunc> {};
#endif

  std::ostream& operator<<(std::ostream&, const PPMV<RuleBase>&);

  template <typename Rl, typename HashFunc>
  std::ostream& operator<<(std::ostream& os,
			   const PMAP<Rl>& rp) {
    for(typename PMAP<Rl>::const_iterator it = rp.begin();
	it != rp.end(); ++it) {
      os << *(it->second) << std::endl;
    }
    return os;
  }
}

#endif
