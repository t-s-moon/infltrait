#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
//#include <typeinfo>

#include <cfloat>
#include <cmath>

#include "ds/TermByDoc.h"

namespace infl {

  TermByDoc::TermByDoc(StringFilter* _sf, char c[], bool _logify)
    : TBD<double>(_sf,c), logify(_logify) {
    //    disp.load_dir(c);
  }

  TermByDoc::~TermByDoc() {
    for(size_t i = 0; i < doc_by_term.size(); ++i) {
      if(doc_by_term.at(i) != NULL)
	delete doc_by_term.at(i);
    }
    for(size_t i = 0; i < term_by_doc.size(); ++i) {
      delete term_by_doc.at(i);
    }
  }

  void TermByDoc::build_matrix() {
    for(size_t i = 0; i < disp.size(); ++i) {
      TERM_VECTOR* tv = new TERM_VECTOR;
      if(doc_by_term.size() != 0)
	tv->assign((*(doc_by_term.rbegin()))->size(),0);
      doc_by_term.push_back(tv);

      std::istream& is = disp[i];
      string s;
      while(is >> s) {
#ifdef MDEBUG
	string ss = s;
	std::cout << ss << std::endl;
#endif
	sf->filter(s);
	if(!s.empty()) {
	  size_t id = s2i.getId(s);
	  try {
	    (tv->at(id))++;
	  } catch(std::out_of_range& oor) {
	    if(id == tv->size()) {
#ifdef MDEBUG
	      std::cout << "NEW: " << ss << "\t" << s << std::endl;
#endif
	      tv->push_back(0);
	    } else {
	      std::ostringstream oss;
	      oss << "Uh-oh! There's a discrepancy of "
		  << id - tv->size() << "between the size"
		  << " of this vector and the last entry in"
		  << " the Str2Id map" << std::endl;
	      throw std::runtime_error(oss.str());
	    }
	  }
	}
      }
    }

#ifdef DEBUG
    std::cerr << "Finished reading all files" << std::endl;
#endif

    size_t final_size = (*doc_by_term.rbegin())->size();
    for(size_t i = 0; i < doc_by_term.size(); ++i) {
      size_t this_size = doc_by_term.at(i)->size();
      while(final_size - (this_size++)) {
	doc_by_term.at(i)->push_back(0);
      }
    }

#ifdef MDEBUG
    for(size_t i = 0; i < doc_by_term.size(); ++i) {
      std::cerr << doc_by_term.at(i)->size() << std::endl;
    }
#endif
#ifdef DEBUG
    std::cerr << "Finished adjusting all doc matrices" << std::endl;
#endif
    
    if(logify)
      make_log();

#ifdef DEBUG
    std::cerr << "Finished log conversion on all doc matrices" << std::endl;
#endif
  }

  void TermByDoc::make_log() {
    for(size_t i = 0; i < doc_by_term.size(); ++i) {
      for(size_t j = 0; j < doc_by_term.at(i)->size(); ++j) {
	double val = doc_by_term.at(i)->at(j);
	if(val < DBL_EPSILON)
	  doc_by_term.at(i)->at(j) = 0;
	else 
	  doc_by_term.at(i)->at(j) = log(val);
      }
    }
  }

  void TermByDoc::transpose() {
    size_t row_size = s2i.size();
    size_t column_size = doc_by_term.size();
    for(size_t i = 0; i < row_size; ++i) {
      std::string s = s2i.getStr(i);
      LIVECTOR* lv = new LIVECTOR(column_size, 0, s);
      term_by_doc.push_back(lv);
    }
    for(size_t i = 0; i < row_size; ++i) {
      for(size_t j = 0; j < column_size; ++j) {
	term_by_doc.at(i)->at(j) = doc_by_term.at(j)->at(i);
      }
    }
    converted_to_tbd = true;
  }

  TermByDocAbs::TermByDocAbs(StringFilter* _sf, char c[])
    : TBD<size_t>(_sf,c) {
    build_matrix();
  }

  void TermByDocAbs::build_matrix() {
    for(size_t i = 0; i < disp.size(); ++i) {
      std::istream& is = disp[i];
      string s;
      while(is >> s) {
	sf->filter(s);
	if(!s.empty())
	  insert(std::make_pair(s,0));
      }
    };

    for(size_t i = 0; i < disp.size(); ++i) {
      std::istream& is = disp[i];
      std::set<string> ss;
      string s;
      while(is >> s) {
	sf->filter(s);
	if(!s.empty()) {
	  if(ss.find(s) == ss.end()) {
	    ss.insert(s);
	    (*this)[s]++;
	  }
	}
      }      
    }
  }

  TermByDocAbs::LIMATRIX& TermByDocAbs::get_matrix() {
    for(iterator it = begin(); it != end(); ++it) {
      string s = it->first;
      LabelIdxVector<size_t>* vs = new LabelIdxVector<size_t>(2,0,s);
      limatrix.push_back(vs);
      vs->at(0) = (it->first).size();
      vs->at(1) = it->second;
    }
    
    return limatrix;
  }

  void TermByDocCount::build_matrix() {
    for(size_t i = 0; i < disp.size(); ++i) {
      std::istream& is = disp[i];
      string s;
      while(is >> s) {
	sf->filter(s);
	if(!s.empty())
	  (*this)[s]++;
      }
    };
  }

  TermByDocAbs::LIMATRIX& TermCount::get_matrix() {
    for(iterator it = begin(); it != end(); ++it) {
      string s = it->first;
      LabelIdxVector<size_t>* vs = new LabelIdxVector<size_t>(1,0,s);
      limatrix.push_back(vs);
      vs->at(0) = (it->first).size();
    }
    
    return limatrix;
  }

  void TermByDocSparse::build_matrix() {
    for(size_t i = 0; i < disp.size(); ++i) {
      std::istream& is = disp[i];
      string s;
      while(is >> s) {
	sf->filter(s);
	if(!s.empty()) {
	  LabelIdxMap* lim;
	  if(find(s) != end()) {
	    lim = (*this)[s];
	  } else {
	    lim = new LabelIdxMap;
	    (*this)[s] = lim;
	  }
	  (*lim)[i]++;
	}
      }
    }

#ifdef DEBUG
    std::cerr << "Finished reading all files" << std::endl;
#endif

    if(logify)
      make_log();

#ifdef DEBUG
    std::cerr << "Finished log conversion on all doc matrices" << std::endl;
#endif
  }

  void TermByDocSparse::make_log() {
    for(iterator it = begin(); it != end(); ++it) {
      for(LabelIdxMap::iterator its = it->second->begin();
	  its != it->second->end(); ++its) {
	its->second = log(its->second);
      }
    }
  }
}
