/**
 * @file   named_matrix.cpp
 * @author Taesun Moon <tsmoon@wireless-128-62-123-101.public.utexas.edu>
 * @date   Fri Feb  6 22:14:27 2009
 * 
 * @brief  
 * 
 * 
 */

#include "ds/NamedMatrix.h"

#include "io/StringAlgo.h"

#include "la/MiniGSL.h"
#include "la/Op.h"

#ifdef DEBUG
#define DVERBOSE	verbose = true
#else
#define DVERBOSE
#endif

namespace infl {

  /** 
   * 
   * 
   * @param _is 
   * @param _n 
   * @param _v 
   * @param _l 
   */
  NamedMatrix::NamedMatrix(istream& _is, int _dim, int _rmax,
		   int _rand_sample, int _rule_max,
		   bool _v, bool _l)
    : is(&_is), dim(_dim), rmax(_rmax), rand_sample(_rand_sample),
      rule_max(_rule_max), verbose(_v), lowercase(_l) {
    init();
  }

  /** 
   * 
   * 
   * @param is 
   * @param _dim 
   * @param _v 
   * @param _l 
   */
  NamedMatrix::NamedMatrix(istream* _is, int _dim, int _rmax,
		   int _rand_sample, int _rule_max,
		   bool _v, bool _l)
    : is(_is), dim(_dim), rmax(_rmax), rand_sample(_rand_sample),
      rule_max(_rule_max), verbose(_v), lowercase(_l) {
    init();
  }

  /** 
   * 
   * 
   * @param _c 
   * @param _dim 
   * @param _v 
   * @param _l 
   */
  NamedMatrix::NamedMatrix(const char* _c, int _dim, int _rmax,
		   int _rand_sample, int _rule_max,
		   bool _v, bool _l)
    : dim(_dim), rmax(_rmax), rand_sample(_rand_sample),
      rule_max(_rule_max), verbose(_v), lowercase(_l) {
    init();
    filename.assign(_c);
    if(endswith(filename, ".gz") || endswith(filename, ".Z")) {
      is = new igzstream(filename.c_str());
      ftype = GZIP;
    } else {
      is = new ifstream(filename.c_str());
      ftype = TXT;
    }
  }

  /** 
   * 
   * 
   */
  NamedMatrix::~NamedMatrix() {
    delete idx_map;
    delete full_idx_map;
    delete frequency_dict;
    delete ppmv_set;
    for(hsvmap::iterator it = word_vector->begin(); it != word_vector->end(); ++it)
      delete it->second;
    delete word_vector;
    for(hsvmap::iterator it = full_word_vector->begin();
	it != full_word_vector->end(); ++it)
      delete it->second;
    delete full_word_vector;
    for(hsvmap::iterator it = full_word_vector_projection->begin();
	it != full_word_vector_projection->end(); ++it)
      delete it->second;
    delete full_word_vector_projection;

    tr->clearall();
    delete tr;
  }

  /** 
   * 
   * 
   */
  void NamedMatrix::init() {
    DVERBOSE;
    full_idx_map = new hsimap;
    idx_map = new hsimap;
    frequency_dict = new hsimap;
    ppmv_set = new hsimap;

    word_vector = new hsvmap;
    full_word_vector = new hsvmap;
    full_word_vector_projection = new hsvmap;
    tr = new Trie<BinaryRule>;
    randomize = false;
  }

  /** 
   * 
   * 
   */
  void NamedMatrix::readstream() {
    readstream(is, ftype);
  }
  
  /** 
   * 
   * 
   * @param is 
   * @param _ft 
   * @param _dim 
   */
  void NamedMatrix::readstream(istream& is, FTYPE _ft) {
    readstream(&is, _ft);
  }

  /** 
   * 
   * 
   * @param is 
   */
  void NamedMatrix::seed_trie(istream* is) {
    string current_word, prev_word;
    print("# Reading file");
    int counter = 0;
    *is >> current_word;
    tr->set_first(current_word);
    counter++;
    while(*is >> current_word) {
      if(is_alpha(current_word)) {
	if(lowercase)
	  makelower(current_word);
	(*frequency_dict)[current_word]++;
	tr->insert(current_word);
      }
      counter++;
      if(counter % 100000 == 0) {
	char c[100];
	sprintf(c, "# Processed %d words", counter);
	print(c);
      }
    }
    tr->find_stems(0,ppmv_stem_min);
    tr->partition_ppmv_by_rule(rule_max);
    rule_pmap = tr->rule_pmap;
    ppmv_pvec = tr->ppmv_pvec;
    build_ppmv_set();
  }

  void NamedMatrix::build_ppmv_set() {
    for(PPMV_PVEC::iterator it = ppmv_pvec->begin();
	it != ppmv_pvec->end(); ++it) {
      string stem = (*it)->stem;
      string s1 = ((*it)->r)->s1;
      string s2 = ((*it)->r)->s2;
      (*ppmv_set)[stem+s1] = (*frequency_dict)[stem+s1];
      (*ppmv_set)[stem+s2] = (*frequency_dict)[stem+s2];
    }
  }
  
  /** 
   * 
   * 
   * @param is 
   * @param _ft 
   * @param _dim 
   */
  void NamedMatrix::readstream(istream* is, FTYPE _ft) {
    m.resize(dim+1, 2*(dim+1));
    seed_trie(is);
    
    print("# Copying lexicon to linear array");
    copy(ppmv_set->begin(), ppmv_set->end(), std::back_inserter(ppmv_order));
    print("# Sorting linear array");
    sort(ppmv_order.begin(), ppmv_order.end(), cmp<WORDCOUNT>());
    //    delete frequency_dict; frequency_dict = new hsimap();
    size_t iN[dim];
    for(size_t i = 0; i < static_cast<size_t>(dim); ++i) {
      iN[i] = i;
    }
    if(randomize)
      std::random_shuffle(iN, iN + dim);
    for(size_t i = 0; i < ppmv_order.size(); ++i){
      char c[100];
      sprintf(c, "%d:\t%s\t%d", static_cast<int>(i), ppmv_order[i].first.c_str(),
	      ppmv_order[i].second);
      //      print(c);
      //      (*frequency_dict)[ppmv_order[i].first] = ppmv_order[i].second;
      if(i < static_cast<size_t>(dim))
	(*idx_map)[ppmv_order[i].first] = iN[i];
      (*full_word_vector)[ppmv_order[i].first] = new Vector(2*(dim+1));
    }

    if(ftype == GZIP) {
      delete is;
      is = new igzstream(filename.c_str());
    } else {
      is->clear();
      is->seekg(0, std::ios_base::beg);
    }
    
    print("# Resetting stream pointer");
    string idx_word, current_word;
    list<string> pre, post;
    print("# Building matrix"); int counter = 0;
    while(*is >> current_word) {
      if(is_alpha(current_word)) {
	if(lowercase)
	  makelower(current_word);
	post.push_back(current_word);
	if(post.size() > 50) {
	  idx_word = post.front();
	  post.pop_front();
	  if(pre.size() > 0) {
	    if(idx_map->find(idx_word) != idx_map->end()) {
	      for(list<string>::iterator it = pre.begin(); it != pre.end(); ++it) {
		if(idx_map->find(*it) != idx_map->end()) 
		  update(idx_word, 2*(*idx_map)[*it], true);
		else if(full_word_vector->find(*it)
			!= full_word_vector->end())
		  update(idx_word, 2*dim, true);
	      }
	      for(list<string>::iterator it = post.begin(); it != post.end(); ++it) {
		if(idx_map->find(*it) != idx_map->end())
		  update(idx_word, 2 * (*idx_map)[*it] + 1, true);
		else if(full_word_vector->find(*it)
			!= full_word_vector->end())
		  update(idx_word, 2 * dim + 1, true);
	      }
	    } else if(full_word_vector->find(idx_word)
		      != full_word_vector->end()) {
	      for(list<string>::iterator it = pre.begin(); it != pre.end(); ++it) {
		if(idx_map->find(*it) != idx_map->end()) 
		  update(idx_word, 2 * (*idx_map)[*it]);
		else if(full_word_vector->find(*it)
			!= full_word_vector->end())
		  update(idx_word, 2 * dim);
	      }
	      for(list<string>::iterator it = post.begin(); it != post.end(); ++it) {
		if(idx_map->find(*it) != idx_map->end()) 
		  update(idx_word, 2 * (*idx_map)[*it] + 1);
		else if(full_word_vector->find(*it)
			!= full_word_vector->end()) 
		  update(idx_word, 2 * dim + 1);
	      }
	    }
	  }
	  pre.push_back(idx_word);
	}
	if(pre.size() > 50)
	  pre.pop_front();
      }
      counter++;
      if(counter % 100000 == 0) {
	char c[512];
	sprintf(c, "# Processed %d words. Current word is %s", counter, idx_word.c_str());
	print(c);
      }
    }
    while(!pre.empty()) {
      idx_word = pre.back();
      pre.pop_back();
      if(idx_map->find(idx_word) != idx_map->end()) {
	for(list<string>::iterator it = pre.begin(); it != pre.end(); ++it) {
	  if(idx_map->find(*it) != idx_map->end()) 
	    update(idx_word, 2 * (*idx_map)[*it]);
	  else if(full_word_vector->find(*it)
		  != full_word_vector->end()) 
	    update(idx_word, 2 * dim);
	}
      } else if(full_word_vector->find(idx_word)
		!= full_word_vector->end()) {
	for(list<string>::iterator it = pre.begin(); it != pre.end(); ++it) {
	  if(idx_map->find(*it) != idx_map->end()) 
	    update(idx_word, 2 * (*idx_map)[*it]);
	  else if(full_word_vector->find(*it)
		  != full_word_vector->end()) 
	    update(idx_word, 2 * dim);
	}
      }
    }
  }

  /** 
   * 
   * 
   */
  void NamedMatrix::calculate_semantics() {
    wordsimilarity();
    normalize_cosine_score();
  }

  /** 
   * 
   * 
   * @param c 
   */
  void NamedMatrix::print(const char* c) {
    if(verbose)
      cout << c << endl;
  }


  void NamedMatrix::normalize_cosine_score() {
    for(PPMV_PVEC::iterator it = ppmv_pvec->begin();
	it != ppmv_pvec->end(); ++it) {
      PPMV<BinaryRule>* p = *it;
      string s1 = p->stem + p->r->s1,
	s2 = p->stem + p->r->s2;
      Vector* v1 = (*full_word_vector_projection)[s1],
	*v2 = (*full_word_vector_projection)[s2];
      double cosine = gsl_cos_nofuss(*v1, *v2);
      int vN_size = ppmv_order.size() - 2;
      int vN[vN_size];
      for(unsigned int i = 0, j = 0; i < ppmv_order.size(); ++i) {
	if(ppmv_order[i].first != s1 && ppmv_order[i].first != s2)
	  vN[++j] = i;
      }
      std::random_shuffle(vN, vN + vN_size);
      std::vector<double> cosines;
      for(int i = 0; i < rand_sample; ++i) {
	string rs = ppmv_order[vN[i]].first;
	cosines.push_back
	  (gsl_cos_nofuss(*v1, *(*full_word_vector_projection)[rs]));
      }
      double mu1 = mean(cosines);
      double sigma1 = sd(cosines);
    
      std::random_shuffle(vN, vN + vN_size);
      cosines.clear();
      for(int i = 0; i < rand_sample; ++i) {
	string rs = ppmv_order[vN[i]].first;
	cosines.push_back
	  (gsl_cos_nofuss(*v2, *(*full_word_vector_projection)[rs]));
      }
      double mu2 = mean(cosines);
      double sigma2 = sd(cosines);
      p->ncs = std::min( (cosine - mu1) / sigma1,
			 (cosine - mu2) / sigma2);
    }
  }

  /** 
   * 
   * 
   */
  void NamedMatrix::wordsimilarity() {
    Matrix* U = new Matrix(),
      *S = new Matrix(),
      *VT = new Matrix();

#ifdef MDEBUG
    cout << "m=" << endl;
    cout << m << ";" << endl << endl;
#endif
    
    svd(&m, U, S, VT);
    Range r(0,rmax,0,-1);
    Matrix subV;
    VT->get(subV, r);

#ifdef MDEBUG
    cout << "SUBV=";
    cout << subV << ";" << endl;
#endif
    
    for(hsvmap::iterator it = full_word_vector->begin();
	it != full_word_vector->end(); ++it) {
      Vector* vresult = new Vector(subV.m);
      it->second->order = CblasColMajor; /// transposing vector
      gsl_blas_dgemv(CblasNoTrans, 1, subV, *(it->second), 0, *vresult);
#ifdef DEBUG
      string s = it->first;
#endif
      (*full_word_vector_projection)[it->first] = vresult;
    }
  }

  void NamedMatrix::test_print_data() {
    hsvmap::iterator itf = full_word_vector->begin();
    for(hsvmap::iterator it = full_word_vector_projection->begin();
	it != full_word_vector_projection->end(); ++it, ++itf) {
      cout << itf->first << "= " << *(itf->second) << ";" << endl;
      cout << it->first << "_p= " << *(it->second)<< ";" << endl;
    }
  }
  
  /** 
   * 
   * 
   */
  void NamedMatrix::test_print() {
    for(PPMV_PVEC::iterator it = ppmv_pvec->begin();
	it != ppmv_pvec->end(); ++it) {
      string s1 = (*it)->stem + (*it)->r->s1,
	s2 = (*it)->stem + (*it)->r->s2;
      double ncs = (*it)->ncs;
      cout << "# " << s1 << "\t" << s2 << "\t" << ncs << endl;
    }
  }
}
