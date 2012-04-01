#ifdef DEBUG
#include <iostream>
#endif

#include "io/AnsiCType.h"

namespace infl {

  AnsiCType::AnsiCType(std::size_t refs)
    : CCType(ansi_table, false, refs)
  {
#if (defined (__i386__) || defined( __x86_64__ ))
    const mask	_A = std::ctype_base::alpha;
    const mask	_C = std::ctype_base::cntrl;
    const mask	_D = std::ctype_base::digit;
    const mask	_G = std::ctype_base::graph;
    const mask	_L = std::ctype_base::lower;
    const mask	_P = std::ctype_base::punct;
    const mask	_S = std::ctype_base::space;
    const mask	_U = std::ctype_base::upper;
    const mask	_X = std::ctype_base::xdigit;
    const mask	_R = std::ctype_base::print;

  static mask actype[] = {
      /* 0-7 */
      _C, _C, _C, _C, _C, _C, _C, _C,
      /* 8-15 */
      _C, _C | _S, _C | _S, _C | _S, _C | _S, _C | _S, _C ,_C,
      /* 16-23 */
      _C, _C, _C, _C, _C, _C, _C, _C,
      /* 24-31 */
      _C, _C, _C, _C, _C, _C, _C, _C,
      /* 32-39 */
      _S | _P,  _P,  _P,  _P,  _P,  _P,  _P,  _P,
      /* 40-47 */
      _P,  _P,  _P,  _P,  _P,  _P,  _P,  _P,
      /* 48-55 */
      _D, _D, _D, _D, _D, _D, _D, _D,
      /* 56-63 */
      _D, _D, _P, _P, _P, _P, _P, _P,
      /* 64-71 */
      _P, _U | _A | _X, _U | _A | _X, _U | _A | _X, _U | _A | _X, _U | _A | _X, _U | _A | _X, _U | _A,
      _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A,     
      /* 80-87 */ /* 72-79 */
      _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A,
      /* 88-95 */
      _U | _A, _U | _A, _U | _A, _P, _P, _P, _P, _P,
      /* 96-103 */
      _P, _L | _A | _X, _L | _A | _X, _L | _A | _X, _L | _A | _X, _L | _A | _X, _L | _A | _X, _L | _A,
      /* 104-111 */
      _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A,
      /* 112-119 */
      _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A,
      /* 120-127 */
      _L | _A, _L | _A, _L | _A, _P, _P, _P, _P, _C,
      /* 128-143 */
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      /* 144-159 */
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      /* 160-175 */
      _S, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P,
      /* 176-191 */
      _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P,
      /* 192-207 */
      _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A,
      /* 208-223 */
      _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _P, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _U | _A, _L | _A,
      /* 224-239 */
      _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A,
      /* 240-255 */
      _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _P, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A, _L | _A
    };
    mask* T = actype;
#else
    mask* T = classic_table();
#endif
    std::copy(T, T + table_size, ansi_table);

    for(int i = 192; i < 223; i++)
      {
	if(i != 214)
	  ansi_table[i] = (mask) (alpha | upper);
      }
    for(int i = 223; i < 256; i++)
      {
	if(i != 247)
	  ansi_table[i] = (mask) (alpha | lower);
      }
  }
  

  unsigned char AnsiCType::aclower(unsigned char c) const {
    if(c < 0x7F)
      return tolower(c);
    else if( upper & ansi_table[c])
      return c + (224-192);
    else
      return c;
  }

  unsigned char AnsiCType::acupper(unsigned char c) const {
    if(c < 0x7F)
      return toupper(c);
    else if( lower & ansi_table[c])
      return c - (224-192);
    else
      return c;
  }

  const unsigned char*
  AnsiCType::
  ac_scan_not(mask __m, const unsigned char* __low, const unsigned char* __high) const
  {
    if (ansi_table)
      while (__low < __high
	     && (ansi_table[static_cast<unsigned char>(*__low)] & __m) != 0)
	++__low;
    else
      while (__low < __high && this->is(__m, *__low) != 0)
	++__low;
    return __low;
  }

  AnsiVowelType::AnsiVowelType(std::size_t refs)
    : std::ctype<char>(ansi_table, false, refs)
  {
    //For convenience, using the "upper" bitmask
    //however, a mask for finding vowels

    ansi_table[(int) 'a'] = (mask) upper;
    ansi_table[(int) 'e'] = (mask) upper;
    ansi_table[(int) 'i'] = (mask) upper;
    ansi_table[(int) 'o'] = (mask) upper;
    ansi_table[(int) 'u'] = (mask) upper;

    for(int i = 0xA0; i < 0xF0; i++)
      ansi_table[i] = (mask) upper;
    for(int i = 0xF2; i < 0xF7; i++)
      ansi_table[i] = (mask) upper;
    for(int i = 0xF8; i < 0xFD; i++)
      ansi_table[i] = (mask) upper;
  }
}
