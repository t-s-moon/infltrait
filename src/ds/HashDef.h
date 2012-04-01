/**
 * @file   HashDef.h
 * @author Taesun Moon <tsmoon@wireless-128-62-236-156.public.utexas.edu>
 * @date   Mon Mar  2 16:05:36 2009
 * 
 * @brief  Definitions for all hash map calls in library
 * 
 * 
 */


#ifndef __HASH_DEF_H__
#define __HASH_DEF_H__

#include <string>

#ifdef SGIHASH
#include <ext/hash_map>
#else
#include <tr1/unordered_map>
#endif

namespace infl {

#ifdef SGIHASH
  typedef __gnu_cxx::hash<const char*> chash;
#else
  struct chash {
    inline size_t operator()(const char* __s) {
      unsigned long __h = 0;
      for ( ; *__s; ++__s)
	__h = 5 * __h + *__s;
      return size_t(__h);
    }
  };
#endif

  /** 
   * 
   * 
   * 
   * @return 
   */
  struct hash {
    size_t operator()( const std::string& x ) const {
      return chash()( x.c_str() );
    }
  };

}

#endif
