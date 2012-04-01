/**
 * @file   TermByDoc.h
 * @author Taesun Moon <tsmoon@wireless-128-62-236-156.public.utexas.edu>
 * @date   Mon Mar  2 16:13:48 2009
 * 
 * @brief  
 * 
 * 
 */

#ifndef __TERM_BY_DOC_H__
#define __TERM_BY_DOC_H__

#include <map>
#include <set>
#include <string>
#include <vector>

#include "io/IO.h"
#include "io/StringFilter.h"

#include "ds/Str2IdMap.h"
#include "ds/LabeledVector.h"

namespace infl {
  
  using std::string;

  template <typename T>
  class TBD {
  public:
#ifdef CONSERVE_MEM
    typedef std::vector<float> TERM_VECTOR;
    typedef LabelIdxVector<float> LIVECTOR;
#else
    typedef std::vector<T> TERM_VECTOR;
    typedef LabelIdxVector<T> LIVECTOR;
#endif
    typedef std::vector<TERM_VECTOR*> TMATRIX;
    typedef std::vector<LIVECTOR*> LIMATRIX;

  protected:
    StringFilter* sf;
    Dispenser disp;

  public:
    TBD(StringFilter* _sf, const char c[] = ".")
      : sf(_sf) {disp.load_dir(c);}
    virtual ~TBD() {}

    virtual void build_matrix() = 0;
    virtual LIMATRIX& get_matrix() = 0;
    virtual void make_log() = 0;
  };
  
  class TermByDoc : public TBD<double> {
  private:

    Str2IdMap<size_t> s2i;

    TMATRIX doc_by_term;	/**< The matrix used to hold the term
				   counts by document. Documents are
				   rows and terms are columns */
    LIMATRIX term_by_doc;	/**< The transpose of @p
				   doc_by_term. However, it is a
				   complete copy and imposes demands
				   on the memory */
    
    bool logify,		/**< If true, convert values in @p
				   doc_by_term to log values */
      converted_to_tbd;		/**< If true, the matrix @p
				   doc_by_term has been converted to
				   @p term_by_doc */

  public:
    TermByDoc(StringFilter*, char[] = ".", bool=true);
    ~TermByDoc();

  public:
    virtual void build_matrix();
    virtual void make_log();
    void transpose();

  public:
    virtual LIMATRIX& get_matrix(bool cleanup=true) {
      if(!converted_to_tbd) {
	transpose(); clean_mem();
      }
      return term_by_doc;
    }

    inline const Str2IdMap<size_t>& get_s2i() {
      return s2i;
    }

    inline size_t get_rowsize() {
      if(!converted_to_tbd) {
	transpose(); clean_mem();
      }
      return term_by_doc.size();
    }

    inline size_t get_colsize() {
      if(!converted_to_tbd) {
	transpose(); clean_mem();
      }
      return term_by_doc.at(0)->size();
    }

  private:
    inline void clean_mem() {
      for(size_t i = 0; i < doc_by_term.size(); ++i) {
	if(doc_by_term.at(i) != NULL)
	  delete doc_by_term.at(i);
	else
	  return;
	doc_by_term.at(i) = NULL;
      }
    }
  };

  class TermByDocAbs : public TBD<size_t>, public std::map<string,size_t> {
  protected:
    LIMATRIX limatrix;
    
  public:
    TermByDocAbs(StringFilter*, char[] = ".");
    virtual ~TermByDocAbs() {
      for(size_t i = 0; i < limatrix.size(); ++i)
	delete limatrix.at(i);
    }

    virtual void build_matrix();
    virtual LIMATRIX& get_matrix();
  };

  class TermByDocCount : public TermByDocAbs {
  public:
    TermByDocCount(StringFilter* _sf, char _c[] = ".")
      : TermByDocAbs(_sf, _c) {}
    virtual ~TermByDocCount() {
      for(size_t i = 0; i < limatrix.size(); ++i)
	delete limatrix.at(i);
    }

    virtual void build_matrix();
  };

  class TermCount : public TermByDocCount {
  public:
    TermCount(StringFilter* _sf, char _c[] = ".")
      : TermByDocCount(_sf, _c) {}
    virtual ~TermCount() {
      for(size_t i = 0; i < limatrix.size(); ++i)
	delete limatrix.at(i);
    }

    virtual LIMATRIX& get_matrix();
  };

  class TermByDocSparse : public TBD<double>, public std::map<string,LabelIdxMap*> {
  private:
    bool logify;

  public:
    TermByDocSparse(StringFilter* _sf, char _c[] = ".", bool _l = false)
      : TBD<double>(_sf, _c), logify(_l) {}
    ~TermByDocSparse() {
      for(iterator it = begin(); it != end(); ++it)
	delete it->second;
    }

    void build_matrix();
    LIMATRIX& get_matrix() {throw 0;}
    void make_log();
  };
}

#endif
