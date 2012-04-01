/**
 * @file   eval.hpp
 * @author Taesun Moon <tsmoon@taesun-moons-macbook-air.local>
 * @date   Sat Mar 22 16:35:51 2008
 * 
 * @brief  Evaluate based on CELEX files
 * 
 */

#ifndef __EVAL_H__
#define __EVAL_H__

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>

#include "Defs.h"

#include "ds/Infltrait.h"

#include "io/ContainerAlgo.h"

namespace infl {

  using std::istream;
  using std::istringstream;
  using std::map;
  using std::ostream;
  using std::set;
  using std::string;

  typedef map<string, set<string>*> EVALBIN;

  struct ClusterEval;

  template <typename T = WORDID, typename BagContent = string>
  class Eval
  {
  public:
    ClusterEval* cluster_eval;

  protected:
    /// need "static" keyword in following to be able to call from
    /// nested structure "OutputSpec"!!
    static void print_sep(ostream& os, string delim = "=")
    {
      int i = 0;
      while(i++ < 80)
	os << delim;
      os << endl;
    }

  public:
    struct Counter {
      double correct;
      double deleted;
      double inserted;

      double precision;
      double recall;
      double f_score;

      size_t singletons, nonsingletons, vocab_size;
      
      Counter() {
	correct = deleted = inserted = precision = recall = f_score = 0;
	singletons = nonsingletons = vocab_size = 0;
      }
    };


  public:
    struct OutputSpec {
      virtual ~OutputSpec()=0;
      virtual ostream& write(ostream&os,string&s,bool b) {throw 0;}
      virtual ostream& write(ostream&os,Counter&c) {throw 0;}
    };

    struct OutputSpecFull : public OutputSpec {
      ~OutputSpecFull(){}
      ostream& write(ostream&os,string&s,bool b=true){
	b ? os << s : os << "#" << s << "#"; os << " ";
	return os;
      }
    };

    struct OutputSpecRaw : public OutputSpec {
      ~OutputSpecRaw(){}
       ostream& write(ostream&os,string&s,bool b=true){
	os << s << " "; return os;
      }
    };

    struct OutputSpecEvalLatex : public OutputSpec {
      ~OutputSpecEvalLatex(){}
       ostream& write(ostream&os,Counter& c) {
	const size_t buffsz = 1024;
	char buf[buffsz];
	sprintf(buf, "%.2f & %.2f & %.2f & ",
		c.precision * 100, c.recall * 100 , c.f_score * 100);
	os << buf << endl;
	return os;
      }
    };

    struct OutputSpecEvalLog : public OutputSpec {
      ~OutputSpecEvalLog(){}
       ostream& write(ostream&os,Counter& c) {
	print_sep(os, "#");
	os << "# recall:\t" << c.recall << endl;
	os << "# precision:\t" << c.precision << endl;
	os << "# f-score:\t" << c.f_score << endl;
	os << "# Total clusters:\t" << c.nonsingletons + c.singletons << endl;
	os << "# Nonsingleton clusters:\t" << c.nonsingletons << endl;
	os << "# Singleton clusters:\t" << c.singletons << endl;
	os << "# Vocab size:\t" << c.vocab_size << endl;
	print_sep(os, "#");
	return os;
      }
    };

  public:
    typedef set<BagContent> WORDBAG;
    typedef set<T> WORDIDBAG;
    typedef std::pair<TRAITNAMES, WORDIDBAG*> pTW;
    typedef Infltrait::TRAITBIN TRAITBIN;

  protected:
    double total_correct, total_deleted, total_inserted;

  public:
    OutputSpec* ospec_types;
    OutputSpec* ospec_eval_latex;
    OutputSpec* ospec_eval_log;

  public:
    Eval() {
      total_correct = 0;
      total_deleted = 0;
      total_inserted = 0;
    }

  public:
    static double correct
    (const set<BagContent>* wib1, const set<BagContent>* wib2, UINT sits) {
      return sits / (double) wib2->size();
    }

    static double deleted
    (const set<BagContent>* wib1, const set<BagContent>* wib2, UINT sits) {
      return (wib2->size() - sits) / (double) wib2->size();
    }

    static double inserted
    (const set<BagContent>* wib1, const set<BagContent>* wib2, UINT sits) {
      return (wib1->size() - sits) / (double) wib2->size();
    }

    static double recall(double correct, double deleted) {
      return correct / (correct + deleted);
    }

    static double precision(double correct, double inserted) {
      return correct / (correct + inserted);
    }

    static double f_score
    (double correct, double inserted, double deleted) {
      double r = recall(correct, deleted);
      double p = precision(correct, inserted);
      return r * p * 2 / (r + p);
    }

  public:
    double recall() const {
      return total_correct / (total_correct + total_deleted);
    }

    double precision() const {
      return total_correct / (total_correct + total_inserted);
    }

    double f_score() const {
      double r = recall();
      double p = precision();
      return r * p * 2 / (r + p);
    }
  };

  /**
   * 
   * 
   */
  class EvalCelex : public Eval<WORDID, WORDID>
  {
  public:
    typedef map<WORDID, pTW> CELEXTABLE;

  private:
    map<TRAITNAMES, Counter> TC;
    
  private:
    CELEXTABLE celex_table;
    map<string, TRAITNAMES> celex_tag_map;
    const Str2IdMap<WORDID>* s2i;
    set<WORDIDBAG*> bag_register;

  public:
    EvalCelex() {}
    ~EvalCelex();

  public:
    void set_celex_map(istream&);
    void set_celex_standard(istream&, const Str2IdMap<WORDID>&);
    //   void evalCluster(const TRAITBIN&);
    void eval_cluster(const Infltrait&);
  
    void print_eval(ostream& os, bool = false);
    void print_eval_latex(ostream& os);
  };

  
  /**
   * 
   * 
   */
  class EvalCelexStream : public Eval<> {
  public:
    typedef map<WORDID, WORDBAG*> CELEXTABLE;
    typedef map<string, WORDIDBAG*> IDTABLE;

  public:
    bool verbose;
    ostream* ecos;

    WORDBAG* vocabulary;

  public:
    IDTABLE string_to_group_idx;
    CELEXTABLE celex_table;
    
  public:
    EvalCelexStream();
    virtual ~EvalCelexStream();

  public:
    virtual void set_standard(istream& is) {throw;}
    virtual void set_standard(istream&, istream&);
    virtual void set_standard(istream&, WORDBAG&);
    virtual void eval_all(istream&);
    void eval_cluster(istream&);
    void eval_muc(istream&);
    void eval_b3(istream&);
    void eval_ceaf(istream&);
  };

  void do_cluster_recall(ostream& os, EVALBIN& standard, EVALBIN& test);
  void do_cluster_precision(ostream& os, EVALBIN& standard, EVALBIN& test);
  void delete_evalbin(EVALBIN& eb);

  //definition of pure virtual destructor just so linker doesn't get
  //thrown off
  template <typename T, typename BagContent>
  Eval<T, BagContent>::OutputSpec::~OutputSpec(){};

  
  struct ClusterEval : public EvalCelexStream {
    
    virtual ~ClusterEval(){}
    virtual void eval(istream&is)=0;
    
  };

  struct ClusterEvalMUC : public ClusterEval {
    const string name;
    ClusterEvalMUC() : name("MUC") {}
    void eval(istream&) {}
  };

  struct ClusterEvalB3 : public ClusterEval {
    const string name;
    ClusterEvalB3() : name("B3") {}
    void eval(istream&is) {}
  };

  struct ClusterEvalCEAF : public ClusterEval {
    map<string,set<set<string>*> > model_map;
    const string name;
    ClusterEvalCEAF() : name("CEAF") {}
    ~ClusterEvalCEAF();
    void eval(istream&is) {}
  };

  struct EvalClusterPerLine : public EvalCelexStream {
    EvalClusterPerLine() {verbose = false;}
    ~EvalClusterPerLine();
    void eval_all(istream&);
    void set_standard(istream&);
    void set_standard(istream&,istream&);
    void set_standard(istream&,WORDBAG&);
  };

}
#endif
