/**
 * @file   named_matrix.h
 * @author Taesun Moon <tsmoon@wireless-128-62-169-147.public.utexas.edu>
 * @date   Wed Feb  4 00:56:00 2009
 * 
 * @brief  
 * 
 * 
 */

#ifndef __NAMED_MATRIX_H__
#define __NAMED_MATRIX_H__

#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>
//#include <iterator>
#include <list>
#include <numeric>
#include <string>
#include <utility>
#include <vector>
#include <cmath>

#include "la/Matrix.h"

#include "ds/Trie.h"

#include "gzstream/gzstream.h"

namespace infl {

  using std::copy;
  using std::cout;
  using std::endl;
  using std::for_each;
  using std::ifstream;
  using std::istream;
  using std::list;
  using std::pair;
  using std::sort;
  using std::string;
  using std::vector;

  /**
   * 
   * String comparison operator for hash_map
   */
  struct eqstr {
    bool operator()(const char* s1, const char* s2) const {
      return strcmp(s1, s2) == 0;
    }
  };

  /** 
   * 
   * 
   * 
   */
  template<typename T> struct cmp :
    public std::binary_function<T, T, bool> {
    bool operator()(const T& it1, const T& it2)  {
      return it1.second > it2.second; //descending order sort
    }
  };

  /** 
   * 
   * 
   */
  template<typename T> struct lowerfunc :
    public std::unary_function<T, void> {
    void operator()(T& t) {
      t = tolower(t);
    }
  };

  /**
   * 
   * 
   */
  typedef pair<string, int> WORDCOUNT;
  typedef vector<WORDCOUNT> FREQTABLE;
  typedef vector<string> svector;

  typedef PMAP<string, int, hash> hsimap;
  typedef PMAP<string, Vector*, hash> hsvmap;

  /**
   * 
   * 
   */
  class NamedMatrix {
  public:
    typedef Trie<BinaryRule>::RULE_PMAP RULE_PMAP;
    typedef Trie<BinaryRule>::PPMV_PVEC PPMV_PVEC;

  private:
#ifdef DEBUG
    const static size_t DIM = 1000; /**<  */
    const static size_t RMAX = 300; /**<  */
    const static size_t RAND_SAMPLE = 50; /**<  */
    const static size_t PPMV_MIN = 50; /**<  */
#else
    const static size_t DIM = 2499; /**<  */
    const static size_t RMAX = 300; /**<  */
    const static size_t RAND_SAMPLE = 200; /**<  */
    const static size_t PPMV_MIN = 200;	/**<  */
#endif

  public:
    hsimap *idx_map,		/**<  */
      *full_idx_map,		/**<  */
      *frequency_dict,		/**<  */
      *ppmv_set;		/**<  */
    hsvmap *word_vector,	/**<  */
      *full_word_vector,	/**<  */
      *full_word_vector_projection; /**<  */
    Trie<BinaryRule>* tr;			/**<  */

    FREQTABLE ppmv_order;	/**<  */

    istream* is;		/**<  */

    string filename;		/**<  */

    Matrix m;			/**< (N+1)-by-2*(N+1) size matrix */
    int dim,			/**< Number of ppmv rows for matrix @p m */
      rmax,			/**< Reduced svd rank of @p m */
      rand_sample,		/**< Random sample size for NCS calculation */
      rule_max,
      ppmv_stem_min;

    RULE_PMAP* rule_pmap;	/**< pointer to @p trie::rule_pmap */
    PPMV_PVEC* ppmv_pvec;	/**< pointer to @p trie::ppmv_pvec */

  public:
    enum FTYPE {TXT, GZIP};
    bool verbose,		/**<  */
      lowercase,		/**<  */
      randomize;		/**<  */
    FTYPE ftype;		/**<  */

  public:
    NamedMatrix(istream&, int=DIM, int=RMAX, int=RAND_SAMPLE, int=PPMV_MIN,
	    bool=false, bool=true);
    NamedMatrix(istream*, int=DIM, int=RMAX, int=RAND_SAMPLE, int=PPMV_MIN,
	    bool=false, bool=true);
    NamedMatrix(const char*, int=DIM, int=RMAX, int=RAND_SAMPLE, int=PPMV_MIN,
	    bool=false, bool=true);
    ~NamedMatrix();

    void readstream();
    void readstream(istream&, FTYPE=GZIP);
    void readstream(istream*, FTYPE=GZIP);

    void calculate_semantics();

    void test_print();
    void test_print_data();
    
  private:
    inline void makelower(string& s) {
      for_each(s.begin(), s.end(), lowerfunc<char>());}
    
    inline void update(string& idx, int j, bool idx_found = false) {
      int i = idx_found ? (*idx_map)[idx] : dim;
      m(i, j)++;
      (*full_word_vector)[idx]->data[j]++;
    }

  private:
    void init();

    void build_ppmv_set();
    void normalize_cosine_score();
    void seed_trie(istream*);

    void wordsimilarity();

  private:
    void print(const char*);
  };

  /** 
   * 
   * 
   * @param ct 
   * 
   * @return 
   */
  template<typename Container>
    double mean(const Container& ct) {
#ifdef MDEBUG
    std::cerr << "Sum of the container is "
	      << std::accumulate(ct.begin(), ct.end(), 0.0) << std::endl;
    std::cerr << "Size of container is "
	      << static_cast<double>(ct.size()) << std::endl;
#endif
    return std::accumulate(ct.begin(), ct.end(), 0.0)
      / static_cast<double>(ct.size());
  }

  /** 
   * 
   * 
   * @param ct 
   * 
   * @return 
   */
  template<typename Container>
    double sd(const Container& ct) {
    double m = mean(ct);
    double s = 0;
    for(typename Container::const_iterator it = ct.begin();
	it != ct.end(); ++it) {
      s += pow(*it - m,2);
    }
    return sqrt(s / (ct.size() - 1));
  }
}

#endif
