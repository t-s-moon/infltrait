#ifndef __STRING_FILTER_H__
#define __STRING_FILTER_H__

#include <algorithm>
#include <functional>
#include <string>

#include "io/AnsiCType.h"
#include "io/StringAlgo.h"

namespace infl {

  using std::string;
  
  enum FILTER_TYPE {ENGLISH_FILTER, GERMAN_FILTER, USP_FILTER, REVERSE_FILTER, NO_FILTER};

  class StringFilter {
  protected:
    inline bool is_alpha(const string& s1, const CCType* ct)
    {
      const unsigned char* c = reinterpret_cast<const unsigned char*>(s1.c_str());
      string::size_type st = s1.size();
      if(ct->ac_scan_not(std::ctype_base::alpha, c, c+st) == c+st)
	return 1;
      else
	return 0;
    }

  protected:
    const CCType* ct;
    std::locale loc;
    
    bool lowercase;
    bool alpha;

    template <typename _Char>
    struct lower : public std::unary_function<_Char, void> {
      const CCType* ct;
      lower(StringFilter* sf) {
	ct = sf->ct;
      }
      void operator() (_Char& _c) {
	_c = ct->aclower(_c);
      }
    };
      
  public:
    StringFilter() {}
    StringFilter(const CCType* _ct, bool _l = true, bool _a = true)
      : ct(_ct), lowercase(_l), alpha(_a) {}
    virtual ~StringFilter() {}
    virtual string filter(string& s) {return s;}
  };


  /**
   * 
   * 
   */
  class UspFilter : public StringFilter {
    string filter(string& s) {
      return s;
    }
  };


  /**
   * 
   * 
   */
  class ReverseUspFilter : public StringFilter {
    string filter(string& s) {
      std::reverse(s.begin(), s.end());
      return s;
    }
  };

  /**
   * 
   * 
   */
  class EnglishFilter : public StringFilter {
    struct lower : public std::unary_function<char, void> {
      void operator() (char& _c) {
	_c = tolower(_c);
      }
    };
    
  public:
    EnglishFilter(bool _l = true, bool _a = true)
      : StringFilter(NULL, _l, _a) {}

    string filter(string& _s) {
      if(infl::is_alpha(_s)) {
	std::for_each(_s.begin(), _s.end(), lower());
      } else {
	_s = "";
      }
      return _s;
    }
  };


  /**
   * 
   * 
   */
  class GermanFilter : public StringFilter {
  public:
    GermanFilter(bool _l = true, bool _a = true)
      : StringFilter(NULL, _l, _a) {
      loc = std::locale(std::locale::classic(), new AnsiCType);
      ct = &(std::use_facet<AnsiCType>(loc));
    }

    string filter(string& _s) {
      if(alpha && is_alpha(_s, ct)) {
	std::for_each(_s.begin(), _s.end(), lower<char>(this));
      } else if (alpha) {
	_s = "";
      }
      return _s;
    }
  };
}

#endif
