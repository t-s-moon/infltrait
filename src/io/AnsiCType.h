#ifndef __ANSI_CTYPE_H__
#define __ANSI_CTYPE_H__

#include <locale>

namespace infl {

  class CCType : public std::ctype<char> {
  public:
    explicit
    CCType(const mask* __table = 0, bool __del = false, size_t __refs = 0)
      : std::ctype<char>(__table, __del, __refs) {}

    virtual unsigned char aclower(unsigned char) const = 0;
    virtual unsigned char acupper(unsigned char) const = 0;
    virtual const unsigned char*
    ac_scan_not(mask, const unsigned char*, const unsigned char*) const = 0;
  };
  
  class AnsiCType : public CCType
  {
  public:
    AnsiCType(std::size_t refs = 0);
    unsigned char aclower(unsigned char) const;
    unsigned char acupper(unsigned char) const;
    const unsigned char*
    ac_scan_not(mask, const unsigned char*, const unsigned char*) const;
    
  private:
    mask ansi_table[table_size];
  };

  class AnsiVowelType : public std::ctype<char>
  {
  public:
    AnsiVowelType(std::size_t refs = 0);

  private:
    mask ansi_table[table_size];
  };
}

#endif
