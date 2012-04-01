/**
 * @file   algo.h
 * @author Taesun Moon <tsmoon@w-mob400-128-62-218-29.public.utexas.edu>
 * @date   Thu Feb 26 19:46:44 2009
 * 
 * @brief  
 * 
 * 
 */


#ifndef __STRING_ALGO_H__
#define __STRING_ALGO_H__

#include <locale>
#include <string>
#include <vector>
#include <cctype>

#include "Defs.h"

namespace infl {

  using std::string;
  using std::ctype;
  
  /** 
   * @brief Examines whether all characters in s1 are alphabetic
   * according to ct
   * 
   * @param s1 
   * @param ct ctype instance
   * 
   * @return True if all characters are alphabetic. False otherwise
   */
  template<typename Char>
  inline bool is_alpha(const string& s1, const std::ctype<Char>& ct) {
    const char* const c = s1.c_str();
    string::size_type st = s1.size();
    if(ct.scan_not(std::ctype_base::alpha, c, c+st) == c+st)
      return 1;
    else
      return 0;
  }

  /** 
   * @brief Examines whether all characters in s1 are alphabetic
   * according to ct
   * 
   * @param s1 
   * @param ct ctype instance
   * 
   * @return True if all characters are alphabetic. False otherwise
   */
  inline bool is_alpha(const string& s1) {
    for(size_t i = 0; i < s1.size(); ++i) {
      if(!isalpha(s1[i]))
	return 0;
    }
    return 1;
  }

  /** 
   * @brief Examines whether the string s1 ends with s2.
   * 
   * @param s1 Pivot string
   * @param s2 Potential suffix
   * 
   * @return bool
   */
  inline bool endswith(const string& s1, const string& s2) {
    size_t s1_len = s1.size() - 1, s2_len = s2.size() - 1;
    if(s1_len < s2_len)
      return false;

    while(s1[--s1_len] == s2[--s2_len])
      ;
    s2_len++;
    return (!s2_len) ? 1 : 0;
  }


  /** 
   * @brief Examines whether the string s1 starts with s2.
   * 
   * @param s1 Pivot string
   * @param s2 potential prefix
   * 
   * @return 
   */
  //
  inline bool startswith(const string& s1, const string& s2) {
    size_t s1_idx = 0, s2_idx = 0;
    if(s1.size() < s2.size())
      return false;

    while(s1[++s1_idx] == s2[++s2_idx])
      ;
    return (s2_idx == s2.size()) ? 1 : 0;
  }


  /** 
   * @brief Splits a string by a given single string delimiter. Returns
   * a std::vector of strings 
   * 
   * @param s String to be split
   * @param delimiter Default is a space. Any delimiter must be a single
   * character string.
   * 
   * @return Std::Vector of strings which were split by delimiter
   */
  inline std::vector<string> split(const string& s, const string& delimiter=" ") {
    std::vector<string> result;
    size_t beg_idx, end_idx = 0;
    beg_idx = 0;
    while(end_idx != s.npos) {
      end_idx = s.find(delimiter, beg_idx);
      string t_s = s.substr(beg_idx, end_idx - beg_idx);
      result.push_back(t_s);
      beg_idx = end_idx + 1;
    }
    return result;
  }

  /** 
   * @brief If s1 starts with s2, strip s2 from s1 and return s1. Else,
   * return s1 unmodified.
   * 
   * @param s1 Pivot string
   * @param s2 Potential prefix
   * 
   * @return String stripped of prefix s2
   */
  inline string strip_prefix(const string& s1, const string& s2)
  {
    if(startswith(s1, s2))
      {
	return s1.substr(s2.size(), s1.size() - s2.size());
      }
    else
      return s1;
  }


  /** 
   * @brief If s1 ends with s2, strip s2 from s1 and return s1. Else,
   * return s1 unmodified.
   * 
   * @param s1 Pivot string
   * @param s2 Potential suffix
   * 
   * @return String stripped of suffix s2
   */
  inline string strip_suffix(const string& s1, const string& s2) {
    if(endswith(s1, s2)) {
      return s1.substr(0, s1.size() - s2.size());
    }
    else
      return s1;
  }


  /** 
   * @brief Prefix s2 to s1
   * 
   * @param s1 Base
   * @param s2 Prefix
   * 
   * @return Combined string
   */
  inline string prefix(const string& s1, const string& s2) {
    return s2 + s1;
  }


  /** 
   * @brief Suffix s2 to s1
   * 
   * @param s1 Base
   * @param s2 Suffix
   * 
   * @return Combined string
   */
  inline string suffix(const string& s1, const string& s2) {
    return s1 + s2;
  }
  /** 
   * @brief In a string separated by "|", get string left of
   * delimiter. Else return input string.
   * 
   * @param s 
   * 
   * @return 
   */
  //
  inline string get_word(const string& s) {
    string::size_type idx;
    idx = s.find("|");
    if(idx != string::npos)
      return s.substr(0, idx);
    else
      return s;
  }


  /** 
   * 
   * @param s 
   * @param ct 
   * 
   * @return String with all vowels changed to "V"
   * 
   * Change all vowels in a @s to "V". Uses the "upper" mask in
   * @ct as a stopgap mask to filter vowels. See implementation of
   * AnsiVowelType in "ansi_ctype.cpp" for details 
   */
  template <class Char>
  inline string to_vowel(const string& s, const ctype<Char>& ct) {
    string result;
    for(LEN i = 0; i < s.size(); i++) {
      if(ct.is(std::ctype_base::upper, s[i]))
	result += 'V';
      else
	result += s[i];
    }
    return result;
  }


  /** 
   * 
   * @param s 
   * @param ct 
   * 
   * @return String with all vowels removed
   *
   * Delete all vowels in a string. Uses the "upper" mask in ctype as a
   * stopgap mask to filter vowels. See implementation of AnsiVowelType
   * in "ansi_ctype.cpp" for details 
   */
  template <class Char>
  inline string strip_vowels(const string& s, const ctype<Char>& ct) {
    string result;
    for(LEN i = 0; i < s.size(); i++) {
      if(!ct.is(std::ctype_base::upper, s[i]))
	result += s[i];
    }
    return result;
  }

}

#endif
