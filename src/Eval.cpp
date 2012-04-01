#include "Eval.h"

namespace infl {

  EvalCelex::~EvalCelex() {
    for(set<WORDIDBAG*>::iterator it = bag_register.begin();
	it != bag_register.end(); ++it)
      delete *it;
  }

  void EvalCelex::set_celex_standard(istream& is, const Str2IdMap<WORDID>& s2im) {
    string prev_idx = "", line;
    WORDIDBAG* wb_full = NULL;
  
    s2i = &s2im;
    while(getline(is, line)) {
      vector<string> celex_line = split(line);
      string entry = celex_line[0];
      string this_idx = celex_line[1];
      TRAITNAMES tn = string_to_traitnames(celex_line[2]);
      
      WORDID wid;
      if(s2i->isMember(entry)) {
	wid = s2i->getCId(entry);
	if(prev_idx != this_idx) {
	  wb_full = new WORDIDBAG;
	  bag_register.insert(wb_full);
	}
	wb_full->insert(wid);
	if(celex_table.find(wid) == celex_table.end())
	  celex_table[wid] = make_pair(tn, wb_full);
      }
      prev_idx = this_idx;
    }
  }

  void EvalCelex::set_celex_map(istream& is) {
    string line;
    while(getline(is, line)) {
      vector<string> vs = split(line, "\t");
      if(vs.size() > 1) {
	string celex_tag = vs[0];
	string standard_tag = vs[1];
	TRAITNAMES t = string_to_traitnames(standard_tag);
	celex_tag_map[celex_tag] = t;
      }
    }
  }


  void EvalCelex::eval_cluster(const Infltrait& infltrait) {
    const TRAITBIN& tb = infltrait.get_traits();
    for(TRAITBIN::const_iterator it = tb.begin();
	it != tb.end(); ++it) {
      Counter counter;
      TC[it->first] = counter;
    }

    const set<WORDIDBAG*>& ics = infltrait.get_cluster_set();

    for(set<WORDIDBAG*>::const_iterator swibit = ics.begin();
	swibit != ics.end(); ++swibit) {
      for(WORDIDBAG::const_iterator wibit = (*swibit)->begin();
	  wibit != (*swibit)->end(); ++wibit) {
	if(celex_table.find(*wibit) != celex_table.end()) {
	  UINT sits = size_of_its(*swibit, celex_table[*wibit].second);
	  double c = correct(*swibit, celex_table[*wibit].second, sits);
	  double d = deleted(*swibit, celex_table[*wibit].second, sits);
	  double i = inserted(*swibit, celex_table[*wibit].second, sits);
	  total_correct += c;
	  total_deleted += d;
	  total_inserted += i;
	}
      }
    }


    for(TRAITBIN::const_iterator it = tb.begin();
	it != tb.end(); ++it) {
      const set<WORDIDBAG*>& cs = (it->second)->get_cluster_set();
      for(set<WORDIDBAG*>::const_iterator swibit = cs.begin();
	  swibit != cs.end(); ++swibit) {
	for(WORDIDBAG::const_iterator wibit = (*swibit)->begin();
	    wibit != (*swibit)->end(); ++wibit) {
	  if(celex_table.find(*wibit) != celex_table.end()) {
	    UINT sits = size_of_its(*swibit, celex_table[*wibit].second);
	    double c = correct(*swibit, celex_table[*wibit].second, sits);
	    double d = deleted(*swibit, celex_table[*wibit].second, sits);
	    double i = inserted(*swibit, celex_table[*wibit].second, sits);
	    if(it->first == celex_table[*wibit].first)
	      {
		TC[it->first].correct += c;
	      }
	    TC[it->first].deleted += d;
	    TC[it->first].inserted += i;
	  }
	}
      }
    }
  }

  void EvalCelex::print_eval(ostream& os, bool trait_eval) {
    print_sep(os);
    os << "recall:\t" << recall() << endl;
    os << "precision:\t" << precision() << endl;
    os << "f-score:\t" << f_score() << endl;
    print_sep(os);

    if(trait_eval) {
      for(map<TRAITNAMES, Counter>::const_iterator it = TC.begin();
	  it != TC.end(); ++it) {
	os << "POS Category: " << traitnames_to_string(it->first) << endl;
	os << "\trecall:\t"
	   << recall((it->second).correct,
		     (it->second).deleted)
	   << endl;
	os << "\tprecision:\t"
	   << precision((it->second).correct,
			(it->second).inserted)
	   << endl;
	os << "\tf-score:\t"
	   << f_score((it->second).correct,
		      (it->second).inserted,
		      (it->second).deleted)
	   << endl;
	print_sep(os);
      }
    }
  }

  void EvalCelex::print_eval_latex(ostream& os) {
    double eval_matrix[3][TC.size()];

    eval_matrix[0][0] = f_score();
    eval_matrix[1][0] = recall();
    eval_matrix[2][0] = precision();

    UINT cols = 1;
    for(map<TRAITNAMES, Counter>::const_iterator it = TC.begin();
	it != TC.end(); ++it, ++cols) {
      eval_matrix[0][cols] = f_score((it->second).correct,
				     (it->second).inserted,
				     (it->second).deleted);
      eval_matrix[1][cols] = recall((it->second).correct,
				    (it->second).deleted);
      eval_matrix[2][cols] = precision((it->second).correct,
				       (it->second).inserted);
    }

    for(UINT i = 0; i < 3; ++i) {
      UINT j = 0;
      for(; j < cols; ++j) {
	os << eval_matrix[i][j] << " & ";
      }
      os << eval_matrix[i][j] << endl;
    }
  }

  void do_cluster_recall(ostream& os, EVALBIN& standard, EVALBIN& test) {
    set<set<string>* > record_bin;
    for(EVALBIN::iterator it  = test.begin(); it != test.end(); it++) {
      record_bin.insert(it->second);
    }
    UINT N = record_bin.size();
    double sum = 0;

    for(EVALBIN::iterator it  = test.begin(); it != test.end(); it++) {
      if(record_bin.find(it->second) != record_bin.end()) {
	double num;
	if(standard.find(it->first) == standard.end()) {
	  num = 0;
	} else {
	  num = standard[it->first]->size();
	  double denum = size_of_its(standard[it->first], it->second);
	  denum += num;
	  sum += num/denum;
	}
	record_bin.erase(it->second);
      }
    }
    os << sum/N << endl;
  }

  void do_cluster_precision(ostream& os, EVALBIN& standard, EVALBIN& test) {
    set<set<string>* > record_bin;
    UINT N = test.size();
    double sum = 0;

    for(EVALBIN::iterator it  = test.begin(); it != test.end(); it++) {
      if(standard.find(it->first) != standard.end()) {
	double num = size_of_its(standard[it->first], it->second);
	double denum = standard[it->first]->size() + 1;
	sum += num/denum;
      }
    }
    os << sum/N << endl;
  }

  void delete_evalbin(EVALBIN& eb) {
    set<set<string>* > record_bin;
    for(EVALBIN::iterator it  = eb.begin(); it != eb.end(); it++) {
      record_bin.insert(it->second);
    }
    for(set<set<string>* >::iterator sit = record_bin.begin(); sit != record_bin.end(); sit++) {
      delete *sit;
    }
  }

  EvalCelexStream::EvalCelexStream() : verbose(true), ecos(&cout) {
    ospec_types = new OutputSpecFull;
    ospec_eval_latex = new OutputSpecEvalLatex;
    ospec_eval_log = new OutputSpecEvalLog;
  }

  EvalCelexStream::~EvalCelexStream() {
    for(CELEXTABLE::iterator it = celex_table.begin();
	it != celex_table.end(); ++it)
      delete it->second;
    for(IDTABLE::iterator it = string_to_group_idx.begin();
	it != string_to_group_idx.end(); ++it)
      delete it->second;
    delete ospec_types;
    delete ospec_eval_latex;
    delete ospec_eval_log;
  }

  void EvalCelexStream::set_standard(istream& standard, istream& model) {
    string line, entry, sidx;
    vocabulary = new WORDBAG;
    while(getline(model,line)) {
      istringstream iss(line);
      string word;
      while (iss >> word) {
	vocabulary->insert(word);
      }
    }

    while(getline(standard, line)) {
      vector<string> vs = split(line, "\\");
      entry = vs[1];
      if(vocabulary->find(entry) != vocabulary->end()) {
	sidx = vs[3];
	WORDID idx = atoi(sidx.c_str());
	if(celex_table.find(idx) == celex_table.end())
	  celex_table[idx] = new WORDBAG;
	celex_table[idx]->insert(entry);
	if(string_to_group_idx.find(entry) == string_to_group_idx.end())
	  string_to_group_idx[entry] = new WORDIDBAG;
	string_to_group_idx[entry]->insert(idx);
      }
    }

    model.clear();
    model.seekg(0,std::ios::beg);
  }

  void EvalCelexStream::set_standard(istream& is, WORDBAG& wb) {
    vocabulary = &wb;
    string line, entry, sidx;
    while(getline(is, line)) {
      vector<string> vs = split(line, "\\");
      entry = vs[1];
      if(wb.find(entry) != wb.end()) {
	sidx = vs[3];
	WORDID idx = atoi(sidx.c_str());
	if(celex_table.find(idx) == celex_table.end())
	  celex_table[idx] = new WORDBAG;
	celex_table[idx]->insert(entry);
	if(string_to_group_idx.find(entry) == string_to_group_idx.end())
	  string_to_group_idx[entry] = new WORDIDBAG;
	string_to_group_idx[entry]->insert(idx);
      }
    }
  }

  void EvalCelexStream::eval_cluster(istream& is) {
    string line;
    size_t singletons = 0, nonsingletons = 0;
    WORDBAG real_vocabulary;
    while(getline(is, line)) {
      istringstream iss(line);
      string word;
      WORDBAG wb;
      while(iss >> word) {
	if(string_to_group_idx.find(word) != string_to_group_idx.end()) {
	  wb.insert(word);
	  real_vocabulary.insert(word);
	  if(ospec_types != NULL)
	    ospec_types->write(*ecos,word,true);
	} else {
	  if(ospec_types != NULL)
	    ospec_types->write(*ecos,word,false);
	}
      }
      *ecos << endl;

      if(wb.size() > 1)
	nonsingletons++;
      else if (wb.size() == 1)
	singletons++;
      for(WORDBAG::iterator itw = wb.begin();
	  itw != wb.end(); ++itw) {
	WORDIDBAG* wib = string_to_group_idx[*itw];
	for(WORDIDBAG::iterator it = wib->begin();
	    it != wib->end(); ++it) {
	  UINT sits = size_of_its(&wb, celex_table[*it]);
	  double c = correct(&wb, celex_table[*it], sits);
	  double d = deleted(&wb, celex_table[*it], sits);
	  double i = inserted(&wb, celex_table[*it], sits);
	  total_correct += c;
	  total_deleted += d;
	  total_inserted += i;
	}
      }
    }

    Counter c;
    c.recall = recall();
    c.precision = precision();
    c.f_score = f_score();
    c.nonsingletons = nonsingletons;
    c.singletons = singletons;
    c.vocab_size = real_vocabulary.size();

    if(ospec_eval_latex != NULL)
      ospec_eval_latex->write(cout, c);
    if(ospec_eval_log != NULL)
      ospec_eval_log->write(*ecos, c);
  }

  void EvalCelexStream::eval_all(istream& is) {
    eval_cluster(is);
    delete ospec_types;
    ospec_types = NULL;
    is.clear();
    is.seekg(0,std::ios::beg);
    eval_muc(is);
    is.clear();
    is.seekg(0,std::ios::beg);
    eval_b3(is);
  };

  void EvalCelexStream::eval_muc(istream& is) {

    double recall_denominator = 0, precision_denominator = 0;
    double numerator = 0;
    for(CELEXTABLE::iterator it = celex_table.begin();
	it != celex_table.end(); ++it) {
      recall_denominator += it->second->size() - 1;
    }

    string line;
    size_t singletons = 0, nonsingletons = 0;
    WORDBAG real_vocabulary;
    while(getline(is, line)) {
      istringstream iss(line);
      string word;
      WORDBAG wb;
      while(iss >> word) {
	if(string_to_group_idx.find(word) != string_to_group_idx.end()) {
	  wb.insert(word);
	  real_vocabulary.insert(word);
	  if(ospec_types != NULL)
	    ospec_types->write(*ecos,word,true);
	} else {
	  if(ospec_types != NULL)
	    ospec_types->write(*ecos,word,false);
	}
      }
      *ecos << endl;

      if(wb.size() > 1) {
	nonsingletons++;
	precision_denominator += wb.size() - 1;
      } else if (wb.size() == 1) {
	singletons++;
      }
      map<set<string>*,size_t> overlap_counter;
      for(WORDBAG::iterator itw = wb.begin();
	  itw != wb.end(); ++itw) {
	WORDIDBAG* wib = string_to_group_idx[*itw];
	for(WORDIDBAG::iterator it = wib->begin();
	    it != wib->end(); ++it) {
	  set<string>* gold_cluster = celex_table[*it];
	  size_t sits = size_of_its(&wb, celex_table[*it]);
	  if(overlap_counter.find(gold_cluster) == overlap_counter.end())
	    overlap_counter[gold_cluster] = sits;
	}
      }
      for(map<set<string>*,size_t>::iterator it = overlap_counter.begin();
	  it != overlap_counter.end(); ++it)
	numerator += it->second - 1;
    }

    Counter c;
    c.recall = numerator / recall_denominator;
    c.precision = numerator / precision_denominator;
    c.f_score = 2 * c.recall * c.precision / (c.recall + c.precision);
    c.nonsingletons = nonsingletons;
    c.singletons = singletons;
    c.vocab_size = real_vocabulary.size();

    if(ospec_eval_latex != NULL) {
      cout << "MUC:\t";
      ospec_eval_latex->write(cout, c);
    }
    if(ospec_eval_log != NULL)
      ospec_eval_log->write(*ecos, c);
  }

  void EvalCelexStream::eval_b3(istream& is) {
    double recall_denominator = 0, precision_denominator = 0;
    double numerator = 0;
    for(CELEXTABLE::iterator it = celex_table.begin();
	it != celex_table.end(); ++it) {
      recall_denominator += it->second->size();
    }

    string line;
    size_t singletons = 0, nonsingletons = 0;
    WORDBAG real_vocabulary;
    while(getline(is, line)) {
      istringstream iss(line);
      string word;
      WORDBAG wb;
      while(iss >> word) {
	if(string_to_group_idx.find(word) != string_to_group_idx.end()) {
	  wb.insert(word);
	  real_vocabulary.insert(word);
	  if(ospec_types != NULL)
	    ospec_types->write(*ecos,word,true);
	} else {
	  if(ospec_types != NULL)
	    ospec_types->write(*ecos,word,false);
	}
      }
      *ecos << endl;

      if(wb.size() > 1) {
	nonsingletons++;
	precision_denominator += wb.size();
      } else if (wb.size() == 1) {
	singletons++;
	precision_denominator++;
      }
      map<set<string>*,size_t> overlap_counter;
      for(WORDBAG::iterator itw = wb.begin();
	  itw != wb.end(); ++itw) {
	WORDIDBAG* wib = string_to_group_idx[*itw];
	for(WORDIDBAG::iterator it = wib->begin();
	    it != wib->end(); ++it) {
	  set<string>* gold_cluster = celex_table[*it];
	  size_t sits = size_of_its(&wb, celex_table[*it]);
	  if(overlap_counter.find(gold_cluster) == overlap_counter.end())
	    overlap_counter[gold_cluster] = sits;
	}
      }
      for(map<set<string>*,size_t>::iterator it = overlap_counter.begin();
	  it != overlap_counter.end(); ++it)
	numerator += it->second;
    }

    Counter c;
    c.recall = numerator / recall_denominator;
    c.precision = numerator / precision_denominator;
    c.f_score = 2 * c.recall * c.precision / (c.recall + c.precision);
    c.nonsingletons = nonsingletons;
    c.singletons = singletons;
    c.vocab_size = real_vocabulary.size();

    if(ospec_eval_latex != NULL) {
      cout << "B3:\t";
      ospec_eval_latex->write(cout, c);
    }
    if(ospec_eval_log != NULL)
      ospec_eval_log->write(*ecos, c);
  }

  ClusterEvalCEAF::~ClusterEvalCEAF() {
    for(map<string,set<set<string>*> >::iterator it = model_map.begin();
	it != model_map.end(); ++it) {
      for(set<set<string>*>::iterator its = it->second.begin();
	  its != it->second.end(); ++its) {
	delete *its;
      }
    }
  }

  EvalClusterPerLine::~EvalClusterPerLine() {
//     for(CELEXTABLE::iterator it = celex_table.begin();
// 	it != celex_table.end(); ++it)
//       delete it->second;
//     for(IDTABLE::iterator it = string_to_group_idx.begin();
// 	it != string_to_group_idx.end(); ++it)
//       delete it->second;
//     delete ospec_types;
//     delete ospec_eval_latex;
//     delete ospec_eval_log;
  }

  void EvalClusterPerLine::set_standard(istream& standard) {
    string line, entry;
    size_t idx = 0;
    while(getline(standard, line)) {
      std::istringstream iss(line);
      WORDBAG* wb = new WORDBAG;
      celex_table[idx] = wb;
      while(iss >> entry) {
	wb->insert(entry);
	if(string_to_group_idx.find(entry) == string_to_group_idx.end())
	  string_to_group_idx[entry] = new WORDIDBAG;
	string_to_group_idx[entry]->insert(idx);
      }
      ++idx;
    }
  }

  void EvalClusterPerLine::set_standard(istream& standard, WORDBAG& _vocabulary) {
    string line, entry;
    size_t idx = 0;
    vocabulary = &_vocabulary;
    while(getline(standard, line)) {
      std::istringstream iss(line);
      WORDBAG* wb = new WORDBAG;
      celex_table[idx] = wb;
      while(iss >> entry) {
	if(vocabulary->find(entry) != vocabulary->end()) {
	  wb->insert(entry);
	  if(string_to_group_idx.find(entry) == string_to_group_idx.end())
	    string_to_group_idx[entry] = new WORDIDBAG;
	  string_to_group_idx[entry]->insert(idx);
	}
      }
      ++idx;
    }
  }

  void EvalClusterPerLine::set_standard(istream& standard, istream& model) {
    string line, entry;
    size_t idx = 0;
    vocabulary = new WORDBAG;
    while(getline(model,line)) {
      istringstream iss(line);
      string word;
      while (iss >> word) {
	vocabulary->insert(word);
      }
    }

    while(getline(standard, line)) {
      std::istringstream iss(line);
      WORDBAG* wb = new WORDBAG;
      celex_table[idx] = wb;
      while(iss >> entry) {
	if(vocabulary->find(entry) != vocabulary->end()) {
	  wb->insert(entry);
	  if(string_to_group_idx.find(entry) == string_to_group_idx.end())
	    string_to_group_idx[entry] = new WORDIDBAG;
	  string_to_group_idx[entry]->insert(idx);
	}
      }
      ++idx;
    }
    model.clear();
    model.seekg(0,std::ios::beg);
  }

  void EvalClusterPerLine::eval_all(istream& is) {
    eval_cluster(is);
  };
}
